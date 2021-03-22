#include "system.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		void System::Data::load(segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_segments;
				
				load(File::segments_data, raw_segments);

				for (const auto & raw_segment : raw_segments)
				{
					auto id       = raw_segment[Key::Segment::id      ].get < std::string > ();
					auto name     = raw_segment[Key::Segment::name    ].get < std::string > ();
					auto capacity = raw_segment[Key::Segment::capacity].get < std::size_t > ();
					auto index    = raw_segment[Key::Segment::index   ].get < std::size_t > ();

					segments.insert(Segment(generate_string_id(id), std::move(name), capacity, index));
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::load(routes_container_t & routes)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_routes;

				load(File::routes_data, raw_routes);

				for (const auto & raw_route : raw_routes)
				{
					auto id         = raw_route[Key::Route::id        ].get < std::string > ();
					auto start_time = raw_route[Key::Route::start_time].get < std::time_t > ();
					auto direction  = raw_route[Key::Route::direction ].get < std::size_t > ();
					auto weight_k   = raw_route[Key::Route::weight_k  ].get < double > ();
					auto raw_points = raw_route[Key::Route::points    ];

					Route::points_container_t points;

					for (const auto & raw_point : raw_points)
					{
						points.emplace_back(generate_string_id(
							raw_point[Key::Route::segment_id].get < std::string > ()),
							raw_point[Key::Route::arrival   ].get < std::time_t > (),
							raw_point[Key::Route::staying   ].get < std::time_t > (),
							raw_point[Key::Route::position  ].get < std::size_t > ());
					}

					auto route = std::make_shared < Route > (generate_string_id(id), 
						start_time, static_cast < Route::Direction > (direction), weight_k, std::move(points));

					routes[route->id()] = route;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::load(const path_t & path, json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw system_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw system_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
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

				generate_routes();
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
				m_thread_pool.join();
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
				auto index = 0U;

				m_segments.insert(Segment(generate_random_id(), "A",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "A-B", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "B",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "B-C", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "C",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "C-D", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "D",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "D-E", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "E",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "E-F", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "F",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "F-G", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "G",   5U, index++));
				m_segments.insert(Segment(generate_random_id(), "G-H", 1U, index++));
				m_segments.insert(Segment(generate_random_id(), "H",   5U, index++));

				const auto & name_segments_interface = m_segments.get < name_segments_interface_index > ();

				name_segments_interface.find("A"  )->add_adgacent_segment(name_segments_interface.find("A-B")->id());
				name_segments_interface.find("A-B")->add_adgacent_segment(name_segments_interface.find("A"  )->id());
				name_segments_interface.find("A-B")->add_adgacent_segment(name_segments_interface.find("B"  )->id());
				name_segments_interface.find("B"  )->add_adgacent_segment(name_segments_interface.find("A-B")->id());
				name_segments_interface.find("B"  )->add_adgacent_segment(name_segments_interface.find("B-C")->id());
				name_segments_interface.find("B-C")->add_adgacent_segment(name_segments_interface.find("B"  )->id());
				name_segments_interface.find("B-C")->add_adgacent_segment(name_segments_interface.find("C"  )->id());
				name_segments_interface.find("C"  )->add_adgacent_segment(name_segments_interface.find("B-C")->id());
				name_segments_interface.find("C"  )->add_adgacent_segment(name_segments_interface.find("C-D")->id());
				name_segments_interface.find("C-D")->add_adgacent_segment(name_segments_interface.find("C"  )->id());
				name_segments_interface.find("C-D")->add_adgacent_segment(name_segments_interface.find("D"  )->id());
				name_segments_interface.find("D"  )->add_adgacent_segment(name_segments_interface.find("C-D")->id());
				name_segments_interface.find("D"  )->add_adgacent_segment(name_segments_interface.find("D-E")->id());
				name_segments_interface.find("D-E")->add_adgacent_segment(name_segments_interface.find("D"  )->id());
				name_segments_interface.find("D-E")->add_adgacent_segment(name_segments_interface.find("E"  )->id());
				name_segments_interface.find("E"  )->add_adgacent_segment(name_segments_interface.find("D-E")->id());
				name_segments_interface.find("E"  )->add_adgacent_segment(name_segments_interface.find("E-F")->id());
				name_segments_interface.find("E-F")->add_adgacent_segment(name_segments_interface.find("E"  )->id());
				name_segments_interface.find("E-F")->add_adgacent_segment(name_segments_interface.find("F"  )->id());
				name_segments_interface.find("F"  )->add_adgacent_segment(name_segments_interface.find("E-F")->id());
				name_segments_interface.find("F"  )->add_adgacent_segment(name_segments_interface.find("F-G")->id());
				name_segments_interface.find("F-G")->add_adgacent_segment(name_segments_interface.find("F"  )->id());
				name_segments_interface.find("F-G")->add_adgacent_segment(name_segments_interface.find("G"  )->id());
				name_segments_interface.find("G"  )->add_adgacent_segment(name_segments_interface.find("F-G")->id());
				name_segments_interface.find("G"  )->add_adgacent_segment(name_segments_interface.find("G-H")->id());
				name_segments_interface.find("G-H")->add_adgacent_segment(name_segments_interface.find("G"  )->id());
				name_segments_interface.find("G-H")->add_adgacent_segment(name_segments_interface.find("H"  )->id());
				name_segments_interface.find("H"  )->add_adgacent_segment(name_segments_interface.find("G-H")->id());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::generate_routes()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & name_interface = m_segments.get < name_segments_interface_index > ();

				Route::points_container_t points;

				points.push_back(Route::Point(name_interface.find("A"  )->id(), 0U,   10U, 0U));
				points.push_back(Route::Point(name_interface.find("A-B")->id(), 10U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("B"  )->id(), 40U,  10U, 0U));
				points.push_back(Route::Point(name_interface.find("B-C")->id(), 50U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("C"  )->id(), 80U,  10U, 0U));
				points.push_back(Route::Point(name_interface.find("C-D")->id(), 90U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 120U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 130U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 160U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("E-F")->id(), 170U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("F"  )->id(), 200U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("F-G")->id(), 210U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("G"  )->id(), 240U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("G-H")->id(), 250U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("H"  )->id(), 280U, 10U, 0U));

				for (auto i = 1U; i <= 8U; ++i)
				{
					auto route = std::make_shared < Route > (
						generate_random_id(), i * 60U, Route::Direction::south, 1.0, points, Route::Type::type_green);

					m_routes[route->id()] = route;
				}

				points.clear();

				points.push_back(Route::Point(name_interface.find("H"  )->id(), 0U,   10U, 0U));
				points.push_back(Route::Point(name_interface.find("G-H")->id(), 10U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("G"  )->id(), 40U,  10U, 0U));
				points.push_back(Route::Point(name_interface.find("F-G")->id(), 50U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("F"  )->id(), 80U,  10U, 0U));
				points.push_back(Route::Point(name_interface.find("E-F")->id(), 90U,  30U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 120U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 130U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 160U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("C-D")->id(), 170U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("C"  )->id(), 200U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("B-C")->id(), 210U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("B"  )->id(), 240U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("A-B")->id(), 250U, 30U, 0U));
				points.push_back(Route::Point(name_interface.find("A"  )->id(), 280U, 10U, 0U));

				for (auto i = 1U; i <= 8U; ++i)
				{
					auto route = std::make_shared < Route > (
						generate_random_id(), i * 60U, Route::Direction::north, 0.4, points, Route::Type::type_red);

					m_routes[route->id()] = route;
				}

				points.clear();

				points.push_back(Route::Point(name_interface.find("B"  )->id(), 0U,   20U, 0U));
				points.push_back(Route::Point(name_interface.find("B-C")->id(), 20U,  50U, 0U));
				points.push_back(Route::Point(name_interface.find("C"  )->id(), 70U,  20U, 0U));
				points.push_back(Route::Point(name_interface.find("C-D")->id(), 90U,  50U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 140U, 20U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 160U, 50U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 210U, 20U, 0U));

				auto route_1 = std::make_shared < Route > (
					generate_random_id(), 50U, Route::Direction::south, 0.8, points, Route::Type::type_blue);

				m_routes[route_1->id()] = route_1;

				points.clear();

				points.push_back(Route::Point(name_interface.find("F"  )->id(), 0U,   5U, 0U));
				points.push_back(Route::Point(name_interface.find("E-F")->id(), 5U,  10U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 15U,  5U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 20U, 10U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 30U,  5U, 0U));

				auto route_2 = std::make_shared < Route > (
					generate_random_id(), 300U, Route::Direction::north, 0.6, points, Route::Type::type_yellow);

				m_routes[route_2->id()] = route_2;

				points.clear();

				points.push_back(Route::Point(name_interface.find("B"  )->id(), 0U,   15U, 0U));
				points.push_back(Route::Point(name_interface.find("B-C")->id(), 15U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("C"  )->id(), 30U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("C-D")->id(), 45U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 60U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 75U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 90U,  15U, 0U));
				points.push_back(Route::Point(name_interface.find("E-F")->id(), 105U, 15U, 0U));
				points.push_back(Route::Point(name_interface.find("F"  )->id(), 120U, 15U, 0U));
				points.push_back(Route::Point(name_interface.find("F-G")->id(), 135U, 15U, 0U));
				points.push_back(Route::Point(name_interface.find("G"  )->id(), 150U, 15U, 0U));

				auto route_3 = std::make_shared < Route > (
					generate_random_id(), 460U, Route::Direction::south, 0.8, points, Route::Type::type_blue);

				m_routes[route_3->id()] = route_3;

				points.clear();

				points.push_back(Route::Point(name_interface.find("H"  )->id(), 0U,   12U, 0U));
				points.push_back(Route::Point(name_interface.find("G-H")->id(), 12U,  18U, 0U));
				points.push_back(Route::Point(name_interface.find("G"  )->id(), 30U,  12U, 0U));
				points.push_back(Route::Point(name_interface.find("F-G")->id(), 42U,  18U, 0U));
				points.push_back(Route::Point(name_interface.find("F"  )->id(), 60U,  12U, 0U));
				points.push_back(Route::Point(name_interface.find("E-F")->id(), 72U,  18U, 0U));
				points.push_back(Route::Point(name_interface.find("E"  )->id(), 90U,  12U, 0U));
				points.push_back(Route::Point(name_interface.find("D-E")->id(), 102U, 18U, 0U));
				points.push_back(Route::Point(name_interface.find("D"  )->id(), 120U, 12U, 0U));

				auto route_4 = std::make_shared < Route >(
					generate_random_id(), 500U, Route::Direction::north, 0.6, points, Route::Type::type_yellow);

				m_routes[route_4->id()] = route_4;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::load()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load(m_segments);
				Data::load(m_routes);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::reinitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				clear();

				initialize();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::clear()
		{
			RUN_LOGGER(logger);

			try
			{
				m_segments.clear();
				m_trains.clear();
				m_routes.clear();
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
				std::thread(&System::show, this, true).detach();

				enter_bans();

				std::cin.ignore();

				std::cout << "Run system ? (y/n) ";

				if (getchar() == 'y')
				{
					auto id = generate_random_id();

					m_strategies.insert({ id, 0.0, time_begin, std::make_shared < trains_container_t > (m_trains) });

					Task task(this, std::move(id), time_begin, commands_container_t{}, m_segments);

					boost::asio::post(m_thread_pool, std::move(task));

					show(false);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::process(const id_t & strategy_id, std::time_t time, 
			commands_container_t & commands, segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				decltype(auto) trains = extract_trains(strategy_id);

				const auto & id_segments_interface = 
					segments.get < id_segments_interface_index > ();

				while (time < time_limit)
				{
					make_trains(time, segments, trains, commands);

					active_segments_t active_segments;

					if (time % 10LL == 0LL) // TODO
					{
						for (const auto & segment : id_segments_interface)
						{
							active_segments.insert(segment.id());
						}
					}

					update_segments(segments, time, active_segments);

					execute_commands(segments, trains, commands, active_segments);

					++time;

					auto variants = make_commands(segments, trains, commands, active_segments);
					
					auto deviation = compute_total_deviation(trains, time);

					update_strategy(strategy_id, deviation, time);

					if (is_strategy_promising(strategy_id, deviation))
					{
						if (std::size(variants) > 1U)
						{
							wait(time);

							launch_new_strategies(variants, deviation, time, trains, segments);
						}
					}
					else
					{
						break;
					}
				}

				if (time < time_limit)
				{
					delete_strategy(strategy_id);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::register_event(const id_t & source, const segments_container_t & segments,
			active_segments_t & active_segments) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & id_segments_interface = 
					segments.get < id_segments_interface_index > ();

				std::queue < std::pair < id_t, std::size_t > > queue;

				queue.push(std::make_pair(source, 0U));

				active_segments_t visited_nodes;

				while (!queue.empty())
				{
					const auto & node = queue.front();
					
					queue.pop();

					visited_nodes.insert(node.first);

					active_segments.insert(node.first);

					const auto & adjacent_segments = 
						id_segments_interface.find(node.first)->adjacent_segments();

					for (const auto id : adjacent_segments)
					{
						if ((node.second < bfs_limit) && (visited_nodes.find(id) != std::end(visited_nodes)))
						{
							queue.push(std::make_pair(id, node.second + 1U));
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::show(bool is_initial) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto delta_left = 25.0f;

				const auto delta  = 25.0f;
				
				const auto width  = 1200U + 2 * static_cast < unsigned int > (delta) + static_cast < unsigned int > (delta_left);
				const auto height = 720U  + 4 * static_cast < unsigned int > (delta);

				sf::RenderWindow window(sf::VideoMode(width + 5U, height), "Demonstration");

				std::vector < Line > static_lines;

				static_lines.emplace_back(
					Line::point_t(Line::vector_t(delta + delta_left, delta), sf::Color::Black),
					Line::point_t(Line::vector_t(width - delta, delta), sf::Color::Black));

				static_lines.emplace_back(
					Line::point_t(Line::vector_t(delta + delta_left, height - delta), sf::Color::Black),
					Line::point_t(Line::vector_t(width - delta, height - delta), sf::Color::Black));

				static_lines.emplace_back(
					Line::point_t(Line::vector_t(delta + delta_left, 2.0f * delta), sf::Color::Black),
					Line::point_t(Line::vector_t(delta + delta_left, height - 2.0f * delta), sf::Color::Black));

				static_lines.emplace_back(
					Line::point_t(Line::vector_t(width - delta, 2.0f * delta), sf::Color::Black),
					Line::point_t(Line::vector_t(width - delta, height - 2.0f * delta), sf::Color::Black));

				sf::Font font;

				font.loadFromFile("system/font/consolas.ttf");

				std::vector < sf::Text > letters;

				std::vector < std::string > station_names = {
					"BAG", "MAA", "AGU", "NAR", "OOR", "LUN", "CHO", "SHI" };

				for (auto i = 0U; i < 8U; ++i)
				{
					sf::Text letter(station_names[i], font);

					letter.setCharacterSize(16);
					letter.setStyle(sf::Text::Bold);
					letter.setFillColor(sf::Color::Black);
					letter.setPosition(sf::Vector2f(8.0f, 2.0f * delta + i * 100.0f));

					letters.push_back(std::move(letter));
				}

				std::vector < sf::Text > numbers;

				for (auto i = 0U; i < 21U; ++i)
				{
					sf::Text number(std::to_string(i * 60U), font);

					number.setCharacterSize(12);
					number.setFillColor(sf::Color::Black);
					number.setPosition(sf::Vector2f(delta + delta_left + i * 60.0f - 4.0f, 4.0f));

					numbers.push_back(std::move(number));
				}

				for (auto i = 0U; i < 21U; ++i)
				{
					sf::Text number(std::to_string(i * 60U), font);

					number.setCharacterSize(12);
					number.setFillColor(sf::Color::Black);
					number.setPosition(sf::Vector2f(delta + delta_left + i * 60.0f - 4.0f, height - 17.0f));

					numbers.push_back(std::move(number));
				}

				for (auto i = 0U; i < 21U; ++i)
				{
					static_lines.emplace_back(
						Line::point_t(Line::vector_t(delta + delta_left + i * 60.0f, delta - 5.0f), sf::Color::Black),
						Line::point_t(Line::vector_t(delta + delta_left + i * 60.0f, delta + 5.0f), sf::Color::Black));

					static_lines.emplace_back(
						Line::point_t(Line::vector_t(delta + delta_left + i * 60.0f, height - delta - 5.0f), sf::Color::Black),
						Line::point_t(Line::vector_t(delta + delta_left + i * 60.0f, height - delta + 5.0f), sf::Color::Black));
				}

				for (auto i = 0U; i < 8U; ++i)
				{
					for (auto j = 0U; j < 5U; ++j)
					{
						static_lines.emplace_back(
							Line::point_t(Line::vector_t(delta + delta_left - 5.0f, 2.0f * delta + i * 100.0f + j * 5.0f), sf::Color::Black),
							Line::point_t(Line::vector_t(delta + delta_left + 5.0f, 2.0f * delta + i * 100.0f + j * 5.0f), sf::Color::Black));

						static_lines.emplace_back(
							Line::point_t(Line::vector_t(width - delta - 5.0f, 2.0f * delta + i * 100.0f + j * 5.0f), sf::Color::Black),
							Line::point_t(Line::vector_t(width - delta + 5.0f, 2.0f * delta + i * 100.0f + j * 5.0f), sf::Color::Black));
					}
				}

				std::vector < sf::CircleShape > dots;

				for (auto i = 0U; i <= 300U; ++i)
				{
					for (auto j = 0U; j < 8U; ++j)
					{
						for (auto k = 0U; k < 5U; ++k)
						{
							sf::CircleShape dot(0.5f);

							dot.setFillColor(sf::Color::Black);

							dot.setPosition(sf::Vector2f(delta + delta_left + i * 4.0f, 2.0f * delta + j * 100.0f + k * 5.0f));

							dots.push_back(dot);
						}
					}
				}

				const auto & id_segments_interface =
					m_segments.get < id_segments_interface_index > ();

				while (window.isOpen())
				{
					sf::Event event;

					while (window.pollEvent(event))
					{
						if (event.type == sf::Event::Closed)
						{
							window.close();
						}
					}

					std::vector < Line > lines;

					if (is_initial)
					{
						show_initial(lines, delta, delta_left, width, height);
					}
					else
					{
						show_current(lines, delta, delta_left, width, height);
					}

					std::vector < sf::RectangleShape > bans;

					{
						std::scoped_lock lock(m_mutex_bans);

						for (const auto & ban : m_bans)
						{
							sf::RectangleShape rectangle(sf::Vector2f(
								static_cast < float > (ban.end - ban.begin), 100.0f - 20.0f));

							rectangle.setPosition(sf::Vector2f(delta + delta_left + ban.begin,
								2.0f * delta + (id_segments_interface.find(ban.segment_id)->index() / 2U) * 100.0f + 20.0f));

							rectangle.setFillColor(sf::Color(255, 0, 0, 63));
							rectangle.setOutlineColor(sf::Color::Red);

							bans.push_back(std::move(rectangle));
						}
					}

					window.clear(sf::Color::White);

					for (const auto & static_line : static_lines)
					{
						window.draw(static_line.points, 2U, sf::Lines);
					}

					for (const auto & line : lines)
					{
						window.draw(line.points, 2U, sf::Lines);
					}

					for (const auto & dot : dots)
					{
						window.draw(dot);
					}

					for (const auto & ban : bans)
					{
						window.draw(ban);
					}

					for (const auto & letter : letters)
					{
						window.draw(letter);
					}

					for (const auto & number : numbers)
					{
						window.draw(number);
					}

					window.display();

					std::this_thread::yield();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::show_initial(std::vector < Line > & lines, float delta, float delta_left,
			unsigned int width, unsigned int height) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & id_segments_interface =
					m_segments.get < id_segments_interface_index > ();

				for (const auto [id, route] : m_routes)
				{
					for (auto i = 0U; i < std::size(route->points()); ++i)
					{
						const auto & point = route->points()[i];

						const auto index = id_segments_interface.find(point.segment_id)->index();

						const auto capacity = id_segments_interface.find(point.segment_id)->capacity();

						if (capacity > 1U)
						{
							lines.emplace_back(
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

							set_color(lines, route);

							lines.emplace_back(
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f + 1.0f)),
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f + 1.0f)));

							set_color(lines, route);

							switch (route->direction())
							{
							case Route::Direction::south:
							{
								if (i != 0U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
											2.0f * delta + (index / 2U) * 100.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, route);

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, route);
								}

								if (i != std::size(route->points()) - 1U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

									set_color(lines, route);

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

									set_color(lines, route);
								}

								break;
							}
							case Route::Direction::north:
							{
								if (i != 0U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, route);

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, route);
								}

								if (i != std::size(route->points()) - 1U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
											2.0f * delta + (index / 2U) * 100.0f)));

									set_color(lines, route);

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f)));

									set_color(lines, route);
								}

								break;
							}
							default:
							{
								throw std::runtime_error("unknown route direction: " +
									static_cast < int > (route->direction()));

								break;
							}
							}
						}
						else
						{
							switch (route->direction())
							{
							case Route::Direction::south:
							{
								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)));

								set_color(lines, route);

								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)));

								set_color(lines, route);

								break;
							}
							case Route::Direction::north:
							{
								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time(),
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

								set_color(lines, route);

								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + 1.0f,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + route->start_time() + point.staying + 1.0f,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

								set_color(lines, route);

								break;
							}
							default:
							{
								throw std::runtime_error("unknown route direction: " +
									static_cast < int > (route->direction()));

								break;
							}
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::show_current(std::vector < Line > & lines, float delta, float delta_left,
			unsigned int width, unsigned int height) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				const auto & id_segments_interface =
					m_segments.get < id_segments_interface_index > ();
				const auto & deviation_strategies_interface =
					m_strategies.get < deviation_strategies_interface_index > ();

				auto time = deviation_strategies_interface.begin()->time;

				lines.emplace_back(
					Line::point_t(Line::vector_t(delta + delta_left + time, 2.0f * delta)),
					Line::point_t(Line::vector_t(delta + delta_left + time, height - 2.0f * delta)));

				lines.back().points[0].color = sf::Color::Black;
				lines.back().points[1].color = sf::Color::Black;

				std::cout <<
					"time: " << std::setw(4) << std::right << time << ' ' <<
					"size: " << std::setw(4) << std::right << deviation_strategies_interface.size() << ' ' <<
					"deviation: " << std::setprecision(6) << std::fixed <<
						deviation_strategies_interface.begin()->deviation << std::endl;

				for (const auto & [id, train] : *(deviation_strategies_interface.begin()->trains))
				{
					for (auto i = 0U; i < std::size(train.gid()); ++i)
					{
						const auto & point = train.gid()[i];

						const auto index = id_segments_interface.find(point.segment_id)->index();

						const auto capacity = id_segments_interface.find(point.segment_id)->capacity();

						if (capacity > 1U)
						{
							lines.emplace_back(
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

							set_color(lines, train.route());

							lines.emplace_back(
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f + 1.0f)),
								Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
									2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f + 1.0f)));

							set_color(lines, train.route());

							switch (train.route()->direction())
							{
							case Route::Direction::south:
							{
								if (i != 0U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
											2.0f * delta + (index / 2U) * 100.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, train.route());

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, train.route());
								}

								if (i != std::size(train.gid()) - 1U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

									set_color(lines, train.route());

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

									set_color(lines, train.route());
								}

								break;
							}
							case Route::Direction::north:
							{
								if (i != 0U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, train.route());

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)));

									set_color(lines, train.route());
								}								

								if (i != std::size(train.gid()) - 1U)
								{
									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
											2.0f * delta + (index / 2U) * 100.0f)));

									set_color(lines, train.route());

									lines.emplace_back(
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f + point.position * 5.0f)),
										Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
											2.0f * delta + (index / 2U) * 100.0f)));

									set_color(lines, train.route());
								}

								break;
							}
							default:
							{
								throw std::runtime_error("unknown route direction: " +
									static_cast < int > (train.route()->direction()));

								break;
							}
							}
						}
						else
						{
							switch (train.route()->direction())
							{
							case Route::Direction::south:
							{
								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)));

								set_color(lines, train.route());

								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)));

								set_color(lines, train.route());

								break;
							}
							case Route::Direction::north:
							{
								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

								set_color(lines, train.route());

								lines.emplace_back(
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + 1.0f,
										2.0f * delta + ((index + 1U) / 2U) * 100.0f)),
									Line::point_t(Line::vector_t(delta + delta_left + point.arrival + point.staying + 1.0f,
										2.0f * delta + (index / 2U) * 100.0f + 20.0f)));

								set_color(lines, train.route());

								break;
							}
							default:
							{
								throw std::runtime_error("unknown route direction: " +
									static_cast < int > (train.route()->direction()));

								break;
							}
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::set_color(std::vector < Line > & lines, std::shared_ptr < Route > route) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (route->type())
				{
				case Route::Type::type_green:
				{
					lines.back().points[0].color = sf::Color::Green;
					lines.back().points[1].color = sf::Color::Green;

					break;
				}
				case Route::Type::type_red:
				{
					lines.back().points[0].color = sf::Color::Red;
					lines.back().points[1].color = sf::Color::Red;

					break;
				}
				case Route::Type::type_blue:
				{
					lines.back().points[0].color = sf::Color::Blue;
					lines.back().points[1].color = sf::Color::Blue;

					break;
				}
				case Route::Type::type_yellow:
				{
					lines.back().points[0].color = sf::Color(255, 0, 255);
					lines.back().points[1].color = sf::Color(255, 0, 255);

					break;
				}
				default:
				{
					throw std::runtime_error("unknown route direction: " +
						static_cast < int > (route->direction()));

					break;
				}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		System::trains_container_t & System::extract_trains(const id_t & strategy_id) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				const auto & id_strategies_interface = 
					m_strategies.get < id_strategies_interface_index > ();

				auto trains = id_strategies_interface.find(strategy_id)->trains;

				return *trains;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::make_trains(std::time_t time, const segments_container_t & segments, 
			trains_container_t & trains, commands_container_t & commands) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & id_segments_interface =
					segments.get < id_segments_interface_index > ();

				for (const auto [id, route] : m_routes)
				{
					if (route->start_time() == time)
					{
						auto train_id = generate_random_id();

						auto position = id_segments_interface.find(route->points().at(0).segment_id)->get_free_position();

						trains.emplace(train_id, Train(train_id, id));
						trains[train_id].set_route(route, position);
						commands[train_id] = Command::stay;

						id_segments_interface.find(route->points().at(0U).segment_id)->train_arrived(position);
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::execute_commands(const segments_container_t & segments, trains_container_t & trains, 
			const commands_container_t & commands, active_segments_t & active_segments) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & id_segments_interface =
					segments.get < id_segments_interface_index > ();

				for (const auto & [id, command] : commands)
				{
					switch (command)
					{
					case Command::stay:
					case Command::wait:
					{
						trains[id].stay();

						break;
					}
					case Command::move:
					{
						if (!trains[id].has_completed_move())
						{
							if (id_segments_interface.find(trains[id].next_segment_id())->is_available())
							{
								auto current_position = trains[id].gid().back().position;

								id_segments_interface.find(trains[id].current_segment_id())->train_departured(current_position);

								auto new_position = id_segments_interface.find(trains[id].next_segment_id())->get_free_position();

								trains[id].move(new_position);

								id_segments_interface.find(trains[id].current_segment_id())->train_arrived(new_position);

								register_event(trains[id].current_segment_id(), segments, active_segments);
							}

							trains[id].stay();
						}

						break;
					}
					case Command::skip:
					{
						break;
					}
					default:
					{
						throw std::logic_error("unknown command: " + 
							std::to_string(static_cast < int > (command)));

						break;
					}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		System::variants_container_t System::make_commands(const segments_container_t & segments, const trains_container_t & trains,
			commands_container_t & commands, const active_segments_t & active_segments) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & id_segments_interface =
					segments.get < id_segments_interface_index > ();

				variants_container_t variants;

				variants.push_back(commands);

				for (const auto & [id, train] : trains)
				{
					if (commands[id] == Command::skip)
					{
						continue;
					}

					if (train.has_completed_route())
					{
						commands[id] = Command::skip;

						for (auto & variant : variants)
						{
							variant[id] = Command::skip;
						}

						auto current_position = train.gid().back().position;

						id_segments_interface.find(train.current_segment_id())->train_departured(current_position);

						continue;
					}

					if (!train.is_ready())
					{
						commands[id] = Command::stay;

						for (auto & variant : variants)
						{
							variant[id] = Command::stay;
						}
					}
					else
					{
						if ((commands[id] == Command::wait) && (active_segments.find(
							train.current_segment_id()) == std::end(active_segments)))
						{
							continue;
						}
						else
						{
							auto size = std::size(variants);

							for (auto i = 0U; i < size; ++i)
							{
								variants[i][id] = Command::move;

								variants.push_back(variants[i]);

								variants.back()[id] = Command::wait;
							}
						}
					}
				}

				return variants;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		double System::compute_total_deviation(const trains_container_t & trains, std::time_t time) const
		{
			RUN_LOGGER(logger);

			try
			{
				double deviation = 0.0;

				for (const auto & [id, train] : trains)
				{
					deviation += train.deviation();
				}

				return (deviation / time);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::update_strategy(const id_t & strategy_id, double deviation, std::time_t time)
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				auto & id_strategies_interface =
					m_strategies.get < id_strategies_interface_index > ();

				auto iterator = id_strategies_interface.find(strategy_id);

				id_strategies_interface.modify(iterator, 
					[time, deviation](Strategy & strategy)
					{
						strategy.deviation = deviation;
						strategy.time      = time;
					});
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::delete_strategy(const id_t & strategy_id)
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				const auto & id_strategies_interface =
					m_strategies.get < id_strategies_interface_index > ();

				auto iterator = id_strategies_interface.find(strategy_id);

				m_strategies.erase(iterator);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::is_strategy_promising(const id_t & strategy_id, double deviation) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				const auto & deviation_strategies_interface =
					m_strategies.get < deviation_strategies_interface_index > ();

				return (std::distance(deviation_strategies_interface.begin(),
					deviation_strategies_interface.upper_bound(deviation)) < strategies_limit);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::wait(std::time_t time) const
		{
			RUN_LOGGER(logger);

			try
			{
				while (true)
				{
					{
						std::scoped_lock lock(m_mutex);

						const auto & time_strategies_interface =
							m_strategies.get < time_strategies_interface_index > ();

						if (time < time_strategies_interface.begin()->time + time_delta)
						{
							break;
						}
					}

					std::this_thread::yield();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::launch_new_strategies(const variants_container_t & variants, double deviation, 
			std::time_t time, const trains_container_t & trains, const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				std::scoped_lock lock(m_mutex);

				for (const auto & variant : variants)
				{
					auto id = generate_random_id();

					m_strategies.insert({ id, deviation, time, 
						std::make_shared < trains_container_t > (trains) });

					Task task(this, std::move(id), time, variant, segments);

					boost::asio::post(m_thread_pool, std::move(task));
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::enter_bans()
		{
			RUN_LOGGER(logger);

			try
			{
				auto N = 0U;

				std::cout << "Enter number of bans: ";

				std::cin >> N;

				std::string name;
				std::time_t begin;
				std::time_t end;

				const auto & name_segments_interface =
					m_segments.get < name_segments_interface_index > ();

				for (auto i = 0U; i < N; ++i)
				{
					std::cin >> name >> begin >> end;

					{
						std::scoped_lock lock(m_mutex_bans);

						m_bans.push_back(Ban{ name_segments_interface.find(name)->id(), begin, end });
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::update_segments(segments_container_t & segments, 
			std::time_t time, active_segments_t & active_segments)
		{
			RUN_LOGGER(logger);

			try
			{
				// TODO: use m_mutex_bans

				const auto & id_segments_interface =
					segments.get < id_segments_interface_index > ();

				for (const auto & ban : m_bans)
				{
					if (time == ban.begin)
					{
						id_segments_interface.find(ban.segment_id)->set_state(Segment::State::locked);
					}

					if (time == ban.end)
					{
						id_segments_interface.find(ban.segment_id)->set_state(Segment::State::normal);

						register_event(ban.segment_id, segments, active_segments);
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