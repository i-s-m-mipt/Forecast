#include "system.hpp"

namespace solution
{
	namespace system
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

		void System::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				generate_segments();

				generate_threads();

				m_head = new Node;

				m_head->segments = m_segments;

				m_leafs.push_back(m_head);
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

		void System::generate_segments()
		{
			RUN_LOGGER(logger);

			try
			{
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("A1"), std::forward_as_tuple("A1"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("A2"), std::forward_as_tuple("A2"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("A3"), std::forward_as_tuple("A3"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("AB"), std::forward_as_tuple("AB"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("B1"), std::forward_as_tuple("B1"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("B2"), std::forward_as_tuple("B2"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("B3"), std::forward_as_tuple("B3"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("BC"), std::forward_as_tuple("BC"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("C1"), std::forward_as_tuple("C1"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("C2"), std::forward_as_tuple("C2"));
				m_segments.emplace(std::piecewise_construct, std::forward_as_tuple("C3"), std::forward_as_tuple("C3"));

				m_segments.at("A1").add_northern_adgacent_segment("AB");
				m_segments.at("A2").add_northern_adgacent_segment("AB");
				m_segments.at("A3").add_northern_adgacent_segment("AB");
				m_segments.at("AB").add_northern_adgacent_segment("B1");
				m_segments.at("AB").add_northern_adgacent_segment("B2");
				m_segments.at("AB").add_northern_adgacent_segment("B3");
				m_segments.at("B1").add_northern_adgacent_segment("BC");
				m_segments.at("B2").add_northern_adgacent_segment("BC");
				m_segments.at("B3").add_northern_adgacent_segment("BC");
				m_segments.at("BC").add_northern_adgacent_segment("C1");
				m_segments.at("BC").add_northern_adgacent_segment("C2");
				m_segments.at("BC").add_northern_adgacent_segment("C3");

				m_segments.at("AB").add_southern_adgacent_segment("A1");
				m_segments.at("AB").add_southern_adgacent_segment("A2");
				m_segments.at("AB").add_southern_adgacent_segment("A3");
				m_segments.at("B1").add_southern_adgacent_segment("AB");
				m_segments.at("B2").add_southern_adgacent_segment("AB");
				m_segments.at("B3").add_southern_adgacent_segment("AB");
				m_segments.at("BC").add_southern_adgacent_segment("B1");
				m_segments.at("BC").add_southern_adgacent_segment("B2");
				m_segments.at("BC").add_southern_adgacent_segment("B3");
				m_segments.at("C1").add_southern_adgacent_segment("BC");
				m_segments.at("C2").add_southern_adgacent_segment("BC");
				m_segments.at("C3").add_southern_adgacent_segment("BC");

				m_segments.at("A1").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("A1").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("A2").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("A2").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("A3").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("A3").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("AB").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("AB").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("B1").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("B1").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("B2").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("B2").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("B3").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("B3").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("BC").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("BC").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("C1").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("C1").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("C2").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("C2").set_standard_time("2", Segment::Time{ 10LL, 10LL });
				m_segments.at("C3").set_standard_time("1", Segment::Time{ 10LL, 10LL });
				m_segments.at("C3").set_standard_time("2", Segment::Time{ 10LL, 10LL });
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::generate_threads()
		{
			RUN_LOGGER(logger);

			try
			{
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(0LL), 
					std::forward_as_tuple("1", Train::Direction::north, "A1", "B1", 1.0));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(5LL),
					std::forward_as_tuple("1", Train::Direction::north, "A2", "C2", 1.0));

				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(0LL),
					std::forward_as_tuple("2", Train::Direction::south, "C1", "A1", 0.5));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(0LL),
					std::forward_as_tuple("2", Train::Direction::south, "C2", "A2", 0.5));

				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(100LL),
					std::forward_as_tuple("1", Train::Direction::north, "A1", "B1", 1.0));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(105LL),
					std::forward_as_tuple("1", Train::Direction::north, "A2", "C2", 1.0));

				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(106LL),
					std::forward_as_tuple("2", Train::Direction::south, "C1", "A1", 0.5));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(110LL),
					std::forward_as_tuple("2", Train::Direction::south, "C2", "A2", 0.5));

				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(220LL),
					std::forward_as_tuple("1", Train::Direction::north, "A1", "B1", 1.0));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(225LL),
					std::forward_as_tuple("1", Train::Direction::north, "A2", "C2", 1.0));

				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(230LL),
					std::forward_as_tuple("2", Train::Direction::south, "C1", "A1", 0.5));
				m_threads.emplace(std::piecewise_construct, std::forward_as_tuple(235LL),
					std::forward_as_tuple("2", Train::Direction::south, "C2", "A2", 0.5));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run()
		{
			RUN_LOGGER(logger);

			try
			{
					process();

					std::vector < Node * > path;

					std::sort(std::begin(m_leafs), std::end(m_leafs),
						[](const auto & lhs, const auto & rhs) { return (lhs->deviation < rhs->deviation); });

					path.push_back(m_leafs.front());

					while (path.back()->parent)
					{
						path.push_back(path.back()->parent);
					}

					std::reverse(std::begin(path), std::end(path));

					auto delta = 0LL;

					for (const auto node : path)
					{
						for (const auto & train : node->trains)
						{
							if (m_gid.find(train.id) == std::end(m_gid))
							{
								m_gid[train.id] = Thread();

								m_gid[train.id].start    = time_begin + delta;
								m_gid[train.id].type     = train.type;
								m_gid[train.id].weight_k = train.weight_k;

								m_gid[train.id].points.push_back(Point{ train.position(), 0LL });
							}

							if (m_gid[train.id].points.back().name == train.position())
							{
								++m_gid[train.id].points.back().dt;
							}
							else
							{
								m_gid[train.id].points.push_back(Point{ train.position(), 1LL });
							}
						}

						++delta;
					}

					for (const auto & [id, thread] : m_gid)
					{
						std::cout << thread.start << ' ' << thread.type << ' ' << thread.weight_k << std::endl;

						for (const auto & point : thread.points)
						{
							std::cout << point.name << ' ' << point.dt << std::endl;
						}

						std::cout << std::endl;
					}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::process()
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto time = time_begin; time < time_limit; ++time)
				{
					auto size = std::size(m_leafs);

					if (size > strategies_limit)
					{
						std::sort(std::begin(m_leafs), std::end(m_leafs),
							[](const auto & lhs, const auto & rhs) { return (lhs->deviation < rhs->deviation); });

						m_leafs.erase(std::next(std::begin(m_leafs), strategies_limit), std::end(m_leafs));

						size = strategies_limit;
					}

					for (auto i = 0U; i < size; ++i)
					{
						auto node = m_leafs.front();

						m_leafs.pop_front();

						make_trains(time, node);

						update_segments(time, node);

						execute_commands(node);

						node->update_deviation();

						make_new_nodes(node);
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_trains(std::time_t time, Node * node) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & thread : m_threads)
				{
					const auto begin = thread.second.begin;

					if (thread.first == time && node->segments.at(begin).is_available())
					{
						node->trains.push_back(thread.second);

						node->segments.at(begin).train_arrived();

						node->has_event = true;
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::execute_commands(Node * node) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::for_each(std::begin(node->trains), std::end(node->trains),
					[node](const auto & train) 
					{
						if (train.command == Train::Command::skip)
						{
							node->segments.at(train.position()).train_departured();
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
						train.stay(node->segments.at(train.position()).standard_time(train.type, train.direction));

						break;
					}
					case Train::Command::move:
					{
						if (train.position() != train.end)
						{
							switch (train.direction)
							{
							case Train::Direction::north:
							{
								const auto & segments = node->segments.at(train.position()).northern_adgacent_segments();

								bool has_end = (std::find(std::begin(segments), std::end(segments), train.end) != std::end(segments));

								for (const auto & segment : segments)
								{
									if (has_end && segment != train.end)
									{
										continue;
									}

									if (node->segments.at(segment).is_available())
									{
										node->segments.at(train.position()).train_departured();

										train.move(segment);

										node->segments.at(train.position()).train_arrived();

										node->has_event = true;

										break;
									}
								}

								break;
							}
							case Train::Direction::south:
							{
								const auto & segments = node->segments.at(train.position()).southern_adgacent_segments();

								bool has_end = (std::find(std::begin(segments), std::end(segments), train.end) != std::end(segments));

								for (const auto & segment : segments)
								{
									if (has_end && segment != train.end)
									{
										continue;
									}

									if (node->segments.at(segment).is_available())
									{
										node->segments.at(train.position()).train_departured();

										train.move(segment);

										node->segments.at(train.position()).train_arrived();

										node->has_event = true;

										break;
									}
								}

								break;
							}
							default:
							{
								throw std::runtime_error("unknown direction");
							}
							}

							train.stay(node->segments.at(train.position()).standard_time(train.type, train.direction));
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
						node->segments.at(train.position()).standard_time(train.type, train.direction);

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

								variants.push_back(variants[i]);

								variants.back()[index].command = Train::Command::wait;
							}
						}
					}

					++index;
				}

				for (const auto & variant : variants)
				{
					auto new_node = new Node;

					new_node->segments = node->segments;
					new_node->trains = variant;
					new_node->deviation = node->deviation;
					new_node->parent = node;

					node->children.push_back(new_node);

					m_leafs.push_back(new_node);
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

	} // namespace system

} // namespace solution