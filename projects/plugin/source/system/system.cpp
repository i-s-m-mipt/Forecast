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
				m_interval /= seconds_in_minute; // в минутах

				if (m_interval == 0LL)
				{
					m_interval = 1LL; // минимум 1 минута
				}

				make_segments(input_segments); // загружаем граф

				m_head = std::make_unique < Node > (); // создаем голову кипариса

				m_head->segments = m_segments; // инициализиуем ее

				m_leafs.push_back(m_head.get()); // первый лист - сама голова

				make_routes(input_routes); // создаем исходные нитки

				make_locks(input_locks); // загружаем запреты
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

		// Преобразование данных из dll во внутренний формат
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
						// все время в минутах
						m_segments.at(node.name).set_standard_time(record.type, Segment::Time{ 
							record.time / seconds_in_minute, record.timeRev / seconds_in_minute });
					}
				}

				// задаем смежные сегменты
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

		// Преобразование данных из dll во внутренний формат
		void System::make_routes(const input_routes_t & input_routes)
		{
			RUN_LOGGER(logger);

			try
			{
				std::set < std::time_t > starts; // времена начала ниток

				auto index = 0U;

				for (const auto & input_route : input_routes)
				{
					starts.insert(input_route.StartTime / seconds_in_minute); // в минутах

					const auto begin = input_route.points.front().name;
					const auto end   = input_route.points.back ().name;

					// конструируем нитку во внутреннем представлении маршрута
					m_routes.push_back(std::make_pair(input_route.StartTime / seconds_in_minute,
						Train(index++, input_route.type, get_direction(begin, end), begin, end, input_route.priority)));

					// у этой нитки есть исполненный фрагмент?
					if (!input_route.idPoints.empty())
					{
						m_time_begin = input_route.idStartTime;

						for (const auto & point : input_route.idPoints)
						{
							m_time_begin += point.dt;
						}

						m_time_begin /= seconds_in_minute; // изменяем время начала моделирования

						// обновляем головной узел
						m_head->trains.push_back(m_routes.back().second);
						m_head->trains.back().move(input_route.idPoints.back().name);
						m_head->trains.back().set_segment_time(input_route.idPoints.back().dt / seconds_in_minute);
						m_head->segments.at(input_route.idPoints.back().name).train_arrived();
						m_head->has_event = true;
					}
				}

				if (m_time_begin == 0LL) // если не было исполненного движения
				{
					m_time_begin = *std::begin(starts);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		// Преобразование данных из dll во внутренний формат
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

		// Определение, в каком направлении нужно двигаться от begin до end
		// По сути, это BFS, применим для линейных графов, а для графов со
		// сложной топологией, типа кольца или восьмерки, требуется корректировка,
		// но см. мои комментарии про вырожденные случаи в hpp
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
				make_tree(); // построить кипарис или одну его ветку в вырожденном случае

				make_charts(); // восстановить корректные нитки по кипарису с конца

				m_done_flag.store(true); // просигнализировао завершении для dll
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		// Основная функция алгоритма
		void System::make_tree()
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout("progress.txt", std::ios::out); // только для наблюдения и отладки

				// основной цикл построения кипариса, по нему можно обновлять прогресс бар
				for (auto time = m_time_begin; (time - m_time_begin < time_limit) && 
					(m_leafs.front()->completed_routes_counter < std::size(m_routes)) && !m_interrupt_flag.load(); ++time)
				{
					auto leafs_copy = std::move(m_leafs);

					m_leafs.clear();

					for (auto i = 0U; i < std::size(leafs_copy); ++i) // один лист в вырожденном случае
					{
						auto node = leafs_copy.front(); // извлекаем лист

						leafs_copy.pop_front(); // удаляем его из очереди

						make_trains(time, node); // создаем поезда, которые появились на этом шаге

						update_segments(time, node); // обновляем состояния сегментов

						// сортировка поездов в текущемм узле по приоритетам и отклонениям,
						// этого достаточно чтобы выделить наиболее оптимальную ветку кипариса
						// в вырожденном случае, т.е. итоговый приоритет движения отдается тем
						// поездам, которые окажутся первыми в этом отсортированном массиве
						std::sort(std::begin(node->trains), std::end(node->trains),
							[](const auto & lhs, const auto & rhs) 
							{ 
								return ((lhs.priority > rhs.priority) || 
									((lhs.priority == rhs.priority) && (lhs.deviation() > rhs.deviation())));
							});

						execute_commands(time, node); // выполняем команды в порядке сортировки

						node->update_deviation(); // обновляем отклонение

						make_new_nodes(node); // расширем кипарис (один узел в вырожденном случае)
					}

					fout << time - m_time_begin << std::endl; // для отладки или в прогресс бар
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		// Формирование итоговых графиков по кипарису
		void System::make_charts()
		{
			RUN_LOGGER(logger);

			try
			{
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

		// Создание новых поездов, которые сформировались в этот момент времени
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

		// ОБновление статусов сегментов в соответствии с запретами
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

		// Выполнение заданных поездам команд
		void System::execute_commands(std::time_t time, Node * node) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & segment : node->segments)
				{
					segment.second.update_status(time); // виртуальная занятость, интервал
				}

				std::for_each(std::begin(node->trains), std::end(node->trains),
					[this, time, node](const auto & train) 
					{
						if (train.command == Train::Command::skip) // поезда, завершившие маршрут
						{
							node->segments.at(train.segment()).train_departured(time);

							++node->completed_routes_counter;
						}
					});

				// потом вообще удаляем их
				node->trains.erase(std::remove_if(std::begin(node->trains), std::end(node->trains), 
					[](const auto & train) { return (train.command == Train::Command::skip); }), std::end(node->trains));

				for (auto & train : node->trains)
				{
					switch (train.command)
					{
					case Train::Command::stay: // команды ожидания
					case Train::Command::wait: // команды ожидания
					{
						train.stay(node->segments.at(train.segment()).standard_time(train.type, train.direction));

						break;
					}
					case Train::Command::move: // двигаться если есть возможность
					{
						if (train.segment() != train.end) // если не доехали до конца
						{
							switch (train.direction)
							{
							case Direction::north:
							{
								// куда можно
								const auto & segments = node->segments.at(train.segment()).northern_adjacent_segments();

								// впереди конец?
								bool has_end = (std::find(std::begin(segments), std::end(segments), train.end) != std::end(segments));

								// он доступен?
								if (has_end && node->segments.at(train.end).is_available() && !node->segments.at(train.segment()).is_locked())
								{
									node->segments.at(train.segment()).train_departured(time);

									train.move(train.end);

									node->segments.at(train.segment()).train_arrived();

									node->has_event = true;
								}
								else // не доступен или не конец
								{
									for (const auto & segment : segments)
									{
										// выбираем доступный вариант, но следим, чтобы не появился дедлок
										if (node->segments.at(segment).is_available() && 
											!has_deadlock(node, train.segment(), train.direction) &&
											!node->segments.at(train.segment()).is_locked() &&
											!has_deadend(node, segment, train.end, train.direction))
										{
											node->segments.at(train.segment()).train_departured(time);

											train.move(segment);

											if (has_end)
											{
												train.at_end = true; // вручную выставляем флаг конца
											}

											node->segments.at(segment).train_arrived();

											node->has_event = true;

											break;
										}
									}
								}

								break;
							}
							case Direction::south: // симметрично
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
											!node->segments.at(train.segment()).is_locked() &&
											!has_deadend(node, segment, train.end, train.direction))
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

		// Проверка, приведет ли движение к дедлоку
		bool System::has_deadlock(Node * node, std::string segment, Direction direction) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (direction)
				{
				case Direction::north: // здесь вносится корректировка для пакетного пропуска
				{
					while (true) // если один свободный, то продолжаем
					{
						if (std::size(node->segments.at(segment).northern_adjacent_segments()) == 0U)
						{
							return false; // не дедлок если тупик с выходом во вне
						}

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

							if (counter >= 2) // если свободных два, то не тупик
							{
								return false;
							}
							else if (counter == 0U) // если свободных ноль, то точно тупик
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
				case Direction::south: // симметрично
				{
					while (true)
					{
						if (std::size(node->segments.at(segment).northern_adjacent_segments()) == 0U)
						{
							return false;
						}

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

		// Проверка на наличие тупика
		bool System::has_deadend(Node * node, std::string next_segment,
			std::string last_segment, Direction direction) const
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
						auto size = std::size(node->segments.at(next_segment).northern_adjacent_segments());

						if (size == 0U)
						{
							if (next_segment != last_segment)
							{
								return true;
							}
							else
							{
								return false;
							}
						}
						else if (size > 1U)
						{
							return false;
						}
						else
						{
							next_segment = node->segments.at(next_segment).northern_adjacent_segments().front();
						}
					}

					break;
				}
				case Direction::south:
				{
					while (true)
					{
						auto size = std::size(node->segments.at(next_segment).southern_adjacent_segments());

						if (size == 0U)
						{
							if (next_segment != last_segment)
							{
								return true;
							}
							else
							{
								return false;
							}
						}
						else if (size > 1U)
						{
							return false;
						}
						else
						{
							next_segment = node->segments.at(next_segment).southern_adjacent_segments().front();
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

		// Ветвление дерева кипариса
		void System::make_new_nodes(Node * node)
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < trains_container_t > variants;

				variants.push_back(node->trains); // изначально один вариант

				auto index = 0U;

				for (const auto & train : node->trains)
				{
					const auto standard_time = 
						node->segments.at(train.segment()).standard_time(train.type, train.direction);

					if (train.has_completed_route(standard_time)) // поезд можно исключить из вариантов
					{
						for (auto & variant : variants)
						{
							variant[index].command = Train::Command::skip; // во всех вариантах исключаем
						}

						++index;

						continue;
					}

					if (!train.is_ready(standard_time)) // если поезд не готов к действиям
					{
						for (auto & variant : variants)
						{
							variant[index].command = Train::Command::stay; // то везде stay
						}
					}
					else
					{
						// Здесь всем поездам дается команда move, т.к. в вырожденном
						// случае линейного графа они сортируются по приоритетам и отклонениям
						// и это дает оптимальное решение. В случае с графами со сложной топологией
						// здесь следует добавлять вариант с командой wait и тем самым получать
						// ветвление дерева. Также надо изменить показатель максимальной ширины
						// дерева с 1 до большего значения, в зависимости от топологии.
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

				for (const auto & variant : variants) // 1 вариант в вырожденном случае
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