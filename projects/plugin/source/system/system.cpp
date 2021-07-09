#include "system.hpp"

namespace solution
{
	namespace plugin
	{
		using Severity = shared::Logger::Severity;

		void System::Node::update_deviation()
		{
			RUN_LOGGER(logger);

			try
			{
				deviation = 0.0;

				for (const auto & train : trains)
				{
					deviation += train.deviation();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::initialize(
			const input_segments_t & input_segments,
			const input_routes_t   & input_routes,
			const input_locks_t    & input_locks)
		{
			RUN_LOGGER(logger);

			try
			{
				m_interval /= seconds_in_minute;

				if (m_interval == 0LL)
				{
					m_interval = 1LL;
				}

				make_segments(input_segments);

				m_head = std::make_unique < Node > ();

				m_head->segments = m_segments;

				m_leafs.push_back(m_head.get());

				make_routes(input_routes);

				make_locks(input_locks);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_segments(const input_segments_t & input_segments)
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & node : input_segments.nodes)
				{
					m_segments.emplace(std::piecewise_construct, 
						std::forward_as_tuple(node.name), std::forward_as_tuple(node.name, m_interval));

					for (const auto & record : node.standard_times)
					{
						m_segments.at(node.name).set_standard_time(record.type, Segment::Time{ 
							record.time / seconds_in_minute, record.timeRev / seconds_in_minute });
					}
				}

				for (const auto & line : input_segments.lines)
				{
					switch (line.srcDirection)
					{
					case Line::LineDirection::Direction0:
					{
						m_segments.at(line.srcNode).append_northern_adjacent_segment(line.dstNode);

						break;
					}
					case Line::LineDirection::Direction1:
					{
						m_segments.at(line.srcNode).append_southern_adjacent_segment(line.dstNode);

						break;
					}
					default:
					{
						throw std::runtime_error("unknown direction");
					}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_routes(const input_routes_t & input_routes)
		{
			RUN_LOGGER(logger);

			try
			{
				std::set < std::time_t > starts;

				auto index = 0U;

				for (const auto & input_route : input_routes)
				{
					starts.insert(input_route.StartTime / seconds_in_minute);

					const auto begin = input_route.points.front().name;
					const auto end   = input_route.points.back ().name;

					m_routes.push_back(std::make_pair(input_route.StartTime / seconds_in_minute,
						Train(index++, input_route.type, get_direction(begin, end), begin, end, input_route.priority)));
				}

				m_time_begin = *std::begin(starts);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_locks(const input_locks_t & input_locks)
		{
			RUN_LOGGER(logger);

			try
			{
				m_locks.reserve(std::size(input_locks));

				for (const auto & input_lock : input_locks)
				{
					m_locks.push_back(Lock{ input_lock.name, 
						input_lock.from / seconds_in_minute, input_lock.to / seconds_in_minute });
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		Direction System::get_direction(const std::string & begin, const std::string & end) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::deque < std::pair < std::string, Direction > > segments;

				segments.push_back(std::make_pair(begin, Direction::north));
				segments.push_back(std::make_pair(begin, Direction::south));

				std::unordered_set < std::string > visited_segments;

				visited_segments.insert(begin);

				while (!segments.empty())
				{
					auto segment = segments.front();

					segments.pop_front();

					switch (segment.second)
					{
					case Direction::north:
					{
						for (const auto & next_segment : m_segments.at(segment.first).northern_adjacent_segments())
						{
							if (visited_segments.find(next_segment) == std::end(visited_segments))
							{
								segments.push_back(std::make_pair(next_segment, Direction::north));

								visited_segments.insert(next_segment);
							}
						}

						break;
					}
					case Direction::south:
					{
						for (const auto & next_segment : m_segments.at(segment.first).southern_adjacent_segments())
						{
							if (visited_segments.find(next_segment) == std::end(visited_segments))
							{
								segments.push_back(std::make_pair(next_segment, Direction::south));

								visited_segments.insert(next_segment);
							}
						}

						break;
					}
					default:
					{
						throw std::runtime_error("unknown direction");
					}
					}

					if (segment.first == end)
					{
						return segment.second;
					}
				}

				return Direction::error;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);

				return Direction::error;
			}
		}

		void System::run()
		{
			RUN_LOGGER(logger);

			try
			{
				make_tree();

				make_charts();

				m_done_flag.store(true);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_tree()
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout("progress.txt", std::ios::out);

				for (auto time = m_time_begin; (time - m_time_begin < time_limit) && 
					(m_leafs.front()->completed_routes_counter < std::size(m_routes)) && !m_interrupt_flag.load(); ++time)
				{
					auto leafs_copy = std::move(m_leafs);

					m_leafs.clear();

					for (auto i = 0U; i < std::size(leafs_copy); ++i)
					{
						auto node = leafs_copy.front();

						leafs_copy.pop_front();

						make_trains(time, node);

						update_segments(time, node);

						std::sort(std::begin(node->trains), std::end(node->trains),
							[](const auto & lhs, const auto & rhs) { return lhs.deviation() > rhs.deviation(); });

						execute_commands(time, node);

						node->update_deviation();

						make_new_nodes(node);
					}

					fout << time - m_time_begin << std::endl;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_charts()
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout("progress.txt", std::ios::out | std::ios::app);

				std::vector < Node * > path;

				std::sort(std::begin(m_leafs), std::end(m_leafs),
					[](const auto & lhs, const auto & rhs) { return (lhs->deviation < rhs->deviation); });

				path.push_back(m_leafs.front());

				while (path.back()->parent)
				{
					path.push_back(path.back()->parent);
				}

				fout << "length: " << std::size(path) << std::endl;

				std::reverse(std::begin(path), std::end(path));

				auto delta = 0LL;

				m_charts.resize(std::size(m_routes));

				for (const auto node : path)
				{
					for (const auto & train : node->trains)
					{
						if (m_charts[train.index].points.empty())
						{
							m_charts[train.index].start    = m_time_begin + delta;
							m_charts[train.index].type     = train.type;
							m_charts[train.index].priority = train.priority;

							m_charts[train.index].points.push_back(Point{ train.segment(), 0LL });
						}

						if (m_charts[train.index].points.back().segment == train.segment())
						{
							++m_charts[train.index].points.back().time;
						}
						else
						{
							m_charts[train.index].points.push_back(Point{ train.segment(), 1LL });
						}
					}

					++delta;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_trains(std::time_t time, Node * node)
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & route : m_routes)
				{
					if (route.first == time)
					{
						const auto begin = route.second.begin;

						if (node->segments.at(begin).is_available())
						{
							node->trains.push_back(route.second);

							node->segments.at(begin).train_arrived();

							node->has_event = true;
						}
						else
						{
							route.first += 2LL;
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::update_segments(std::time_t time, Node * node) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & lock : m_locks)
				{
					if (time == lock.begin)
					{
						node->segments.at(lock.segment).state = Segment::State::locked;

						node->has_event = true;
					}

					if (time == lock.end)
					{
						node->segments.at(lock.segment).state = Segment::State::normal;

						node->has_event = true;
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::execute_commands(std::time_t time, Node * node) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & segment : node->segments)
				{
					segment.second.update_status(time);
				}

				std::for_each(std::begin(node->trains), std::end(node->trains),
					[this, time, node](const auto & train) 
					{
						if (train.command == Train::Command::skip)
						{
							node->segments.at(train.segment()).train_departured(time);

							++node->completed_routes_counter;
						}
					});

				node->trains.erase(std::remove_if(std::begin(node->trains), std::end(node->trains), 
					[](const auto & train) { return (train.command == Train::Command::skip); }), std::end(node->trains));

				for (auto & train : node->trains)
				{
					switch (train.command)
					{
					case Train::Command::stay:
					case Train::Command::wait:
					{
						train.stay(node->segments.at(train.segment()).standard_time(train.type, train.direction));

						break;
					}
					case Train::Command::move:
					{
						if (train.segment() != train.end)
						{
							switch (train.direction)
							{
							case Direction::north:
							{
								const auto & segments = node->segments.at(train.segment()).northern_adjacent_segments();

								bool has_end = (std::find(std::begin(segments), std::end(segments), train.end) != std::end(segments));

								if (has_end && node->segments.at(train.end).is_available() && !node->segments.at(train.segment()).is_locked())
								{
									node->segments.at(train.segment()).train_departured(time);

									train.move(train.end);

									node->segments.at(train.segment()).train_arrived();

									node->has_event = true;
								}
								else
								{
									for (const auto & segment : segments)
									{
										if (node->segments.at(segment).is_available() && 
											!has_deadlock(node, train.segment(), train.direction) &&
											!node->segments.at(train.segment()).is_locked())
										{
											node->segments.at(train.segment()).train_departured(time);

											train.move(segment);

											if (has_end)
											{
												train.at_end = true;
											}

											node->segments.at(segment).train_arrived();

											node->has_event = true;

											break;
										}
									}
								}

								break;
							}
							case Direction::south:
							{
								const auto & segments = node->segments.at(train.segment()).southern_adjacent_segments();

								bool has_end = (std::find(std::begin(segments), std::end(segments), train.end) != std::end(segments));

								if (has_end && node->segments.at(train.end).is_available() && !node->segments.at(train.segment()).is_locked())
								{
									node->segments.at(train.segment()).train_departured(time);

									train.move(train.end);

									node->segments.at(train.segment()).train_arrived();

									node->has_event = true;
								}
								else
								{
									for (const auto & segment : segments)
									{
										if (node->segments.at(segment).is_available() && 
											!has_deadlock(node, train.segment(), train.direction) && 
											!node->segments.at(train.segment()).is_locked())
										{
											node->segments.at(train.segment()).train_departured(time);

											train.move(segment);

											if (has_end)
											{
												train.at_end = true;
											}

											node->segments.at(segment).train_arrived();

											node->has_event = true;

											break;
										}
									}
								}

								break;
							}
							default:
							{
								throw std::runtime_error("unknown direction");
							}
							}

							train.stay(node->segments.at(train.segment()).standard_time(train.type, train.direction));
						}

						break;
					}
					case Train::Command::skip:
					{
						break;
					}
					default:
					{
						throw std::runtime_error("unknown command");
					}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::has_deadlock(Node * node, std::string segment, Direction direction) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (direction)
				{
				case Direction::north:
				{
					while (true)
					{
						auto available_segment = 
							node->segments.at(segment).northern_adjacent_segments().front();

						if (std::size(node->segments.at(segment).northern_adjacent_segments()) > 1U)
						{
							auto counter = 0U;

							for (const auto & s : node->segments.at(segment).northern_adjacent_segments())
							{
								if (node->segments.at(s).is_available())
								{
									available_segment = s;

									++counter;
								}
							}

							if (counter >= 2)
							{
								return false;
							}
							else if (counter == 0U)
							{
								return true;
							}
						}

						segment = available_segment;

						if (!node->segments.at(segment).is_available())
						{
							return true;
						}
					}

					break;
				}
				case Direction::south:
				{
					while (true)
					{
						auto available_segment =
							node->segments.at(segment).southern_adjacent_segments().front();

						if (std::size(node->segments.at(segment).southern_adjacent_segments()) > 1U)
						{
							auto counter = 0U;

							for (const auto & s : node->segments.at(segment).southern_adjacent_segments())
							{
								if (node->segments.at(s).is_available())
								{
									available_segment = s;

									++counter;
								}
							}

							if (counter >= 2)
							{
								return false;
							}
							else if (counter == 0U)
							{
								return true;
							}
						}

						segment = available_segment;

						if (!node->segments.at(segment).is_available())
						{
							return true;
						}
					}

					break;
				}
				default:
				{
					throw std::runtime_error("unknown direction");
				}
				}

				return false;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);

				return false;
			}
		}

		void System::make_new_nodes(Node * node)
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < trains_container_t > variants;

				variants.push_back(node->trains);

				auto index = 0U;

				for (const auto & train : node->trains)
				{
					const auto standard_time = 
						node->segments.at(train.segment()).standard_time(train.type, train.direction);

					if (train.has_completed_route(standard_time))
					{
						for (auto & variant : variants)
						{
							variant[index].command = Train::Command::skip;
						}

						++index;

						continue;
					}

					if (!train.is_ready(standard_time))
					{
						for (auto & variant : variants)
						{
							variant[index].command = Train::Command::stay;
						}
					}
					else
					{
						if ((train.command == Train::Command::wait) && (!node->has_event))
						{
							++index;

							continue;
						}
						else
						{
							auto size = std::size(variants);

							for (auto i = 0U; i < size; ++i)
							{
								variants[i][index].command = Train::Command::move;
							}
						}
					}

					++index;
				}

				for (const auto & variant : variants)
				{
					auto new_node = std::make_unique < Node > ();

					new_node->segments = node->segments;
					new_node->trains = variant;
					new_node->deviation = node->deviation;
					new_node->parent = node;
					new_node->completed_routes_counter = node->completed_routes_counter;

					node->children.push_back(std::move(new_node));

					m_leafs.push_back(node->children.back().get());

					if (std::size(m_leafs) > strategies_limit)
					{
						std::sort(std::begin(m_leafs), std::end(m_leafs),
							[](const auto & lhs, const auto & rhs) { return (lhs->deviation < rhs->deviation); });

						m_leafs.erase(std::next(std::begin(m_leafs), strategies_limit), std::end(m_leafs));
					}
				}

				node->segments.clear();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

	} // namespace plugin

} // namespace solution