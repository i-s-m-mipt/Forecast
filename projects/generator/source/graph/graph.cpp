#include "graph.hpp"

namespace solution
{
	namespace generator
	{
		Graph::Route::id_t Graph::Route::last_id = 0;

		std::time_t Graph::Route::get_station_delay(Type type) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (type)
				{
				case Type::cargo:
					return 30;
					break;
				case Type::military:
					return 10;
					break;
				case Type::passenger:
					return 5;
					break;
				default:
					throw graph_exception("unknown train type");
					break;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		double Graph::Route::get_movement_factor(Type type) const
		{
			RUN_LOGGER(logger);

			try
			{
				switch (type)
				{
				case Type::cargo:
					return 0.5;
					break;
				case Type::military:
					return 0.8;
					break;
				case Type::passenger:
					return 1.0;
					break;
				default:
					throw graph_exception("unknown train type");
					break;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::Route::initialize(const std::vector < std::string > & route,
			const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				if (route.size() < 2)
				{
					throw graph_exception("route is too short");
				}

				m_timetable.push_back(std::make_pair(route[0], 0));

				for (auto i = 1U; i < route.size(); ++i)
				{
					auto iterator = std::find_if(segments.begin(), segments.end(),
						[&route, i](const auto & segment)
					{
						return
							(segment.stations.first == route[i - 1] && segment.stations.second == route[i]) ||
							(segment.stations.first == route[i]     && segment.stations.second == route[i - 1]);
					});

					if (iterator == segments.end())
					{
						throw graph_exception("route contains invalid segment: " +
							route[i - 1] + " --> " + route[i]);
					}

					std::time_t delta = m_timetable.back().second + m_station_delay +
						static_cast < std::time_t > (std::ceil(iterator->length * m_movement_factor));

					m_timetable.push_back(std::make_pair(route[i], delta));
				}
			}
			catch (const std::exception& exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::Data::save(const stations_container_t & stations)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & station : stations)
				{
					json_t element;

					element[Key::Station::name] = station.name;

					for (const auto & segment : station.segments)
					{
						element[Key::Station::segments].push_back(boost::uuids::to_string(segment));
					}

					array.push_back(element);
				}

				save(File::stations_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::Data::save(const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & segment : segments)
				{
					json_t element;

					element[Key::Segment::id] = boost::uuids::to_string(segment.id);
					element[Key::Segment::station_1]   = segment.stations.first;
					element[Key::Segment::station_2]   = segment.stations.second;
					element[Key::Segment::length]      = segment.length;
					element[Key::Segment::train_limit] = segment.train_limit;

					array.push_back(element);
				}

				save(File::segments_data, array);
			}
			catch (const std::exception& exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::Data::save(const routes_container_t & routes)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & route : routes)
				{
					json_t element;

					element[Key::Route::id] = route.id();
					element[Key::Route::type] = static_cast < int > (route.type());

					json_t timetable;

					for (const auto & record : route.timetable())
					{
						json_t entry;

						entry[Key::Route::station] = record.first;
						entry[Key::Route::delta]   = record.second;

						timetable.push_back(entry);
					}

					element[Key::Route::timetable] = timetable;

					array.push_back(element);
				}

				save(File::routes_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw graph_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::generate()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto small_station_size  = 3U;
				const auto medium_station_size = 4U;
				const auto large_station_size  = 5U;

				std::vector < std::string > small_station_names = 
				{
					"ISENMOUTHE",
					"THE BLACK GATE",
					"OSGILIATH",
					"ERECH",
					"DOL AMROTH",
					"HELMS DEEP",
					"ISENGARD",
					"LOND DAER",
					"DIMRILL DALE",
					"DOL GULDUR",
					"SARN FORD",
					"WEATHERTOP",
					"LAST BRIDGE",
					"HIGH PASS",
					"GOBLIN GATE",
					"OLD FORD",
					"BEORN",
					"WOOD ELVES",
					"EREBOR",
					"MOUNT GUNDABAD",
					"CARN DUM",
					"FORNOST",
					"FORLOND",
					"HARLOND",
				};
				std::vector < std::string > medium_station_names = 
				{
					"MINAS MORGUL",
					"PELARGIR",
					"EDHELLOND",
					"THARBAD",
					"MORIA GATE",
					"BREE",
					"ESGAROTH",
					"HOBBITON",
					"GREY HAVENS",
				};
				std::vector < std::string > large_station_names = 
				{
					"BARAD-DUR",
					"MOUNT DOOM",
					"MINAS TIRITH",
					"EDORAS",
					"RIVENDELL",
				};

				boost::uuids::random_generator_mt19937 generator;

				// =============================================================

				for (const auto & station_name : small_station_names)
				{
					m_stations.push_back({ station_name, {} });

					for (auto i = 0U; i < small_station_size; ++i)
					{
						auto id = generator();

						m_segments.push_back({ id, std::make_pair(station_name, station_name), 1, 1 });

						m_stations.back().segments.push_back(id);
					}
				}

				// =============================================================

				for (const auto & station_name : medium_station_names)
				{
					m_stations.push_back({ station_name, {} });

					for (auto i = 0U; i < medium_station_size; ++i)
					{
						auto id = generator();

						m_segments.push_back({ id, std::make_pair(station_name, station_name), 1, 1 });

						m_stations.back().segments.push_back(id);
					}
				}

				// =============================================================

				for (const auto & station_name : large_station_names)
				{
					m_stations.push_back({ station_name, {} });

					for (auto i = 0U; i < large_station_size; ++i)
					{
						auto id = generator();

						m_segments.push_back({ id, std::make_pair(station_name, station_name), 1, 1 });

						m_stations.back().segments.push_back(id);
					}
				}

				// =============================================================

				m_segments.push_back({ generator(), std::make_pair("BARAD-DUR", "MOUNT DOOM"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("ISENMOUTHE", "MOUNT DOOM"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("BARAD-DUR", "ISENMOUTHE"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("THE BLACK GATE", "ISENMOUTHE"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("THE BLACK GATE", "OSGILIATH"), 200, 1 });
				m_segments.push_back({ generator(), std::make_pair("MINAS MORGUL", "MOUNT DOOM"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("MINAS MORGUL", "OSGILIATH"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("MINAS TIRITH", "OSGILIATH"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("PELARGIR", "OSGILIATH"), 100, 1 });
				m_segments.push_back({ generator(), std::make_pair("PELARGIR", "EDHELLOND"), 225, 1 });
				m_segments.push_back({ generator(), std::make_pair("PELARGIR", "ERECH"), 250, 1 });
				m_segments.push_back({ generator(), std::make_pair("ERECH", "EDHELLOND"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("DOL AMROTH", "EDHELLOND"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("MINAS TIRITH", "EDORAS"), 250, 1 });
				m_segments.push_back({ generator(), std::make_pair("HELMS DEEP", "EDORAS"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("HELMS DEEP", "ISENGARD"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("HELMS DEEP", "THARBAD"), 300, 1 });
				m_segments.push_back({ generator(), std::make_pair("THARBAD", "ISENGARD"), 250, 1 });
				m_segments.push_back({ generator(), std::make_pair("LOND DAER", "THARBAD"), 200, 1 });
				m_segments.push_back({ generator(), std::make_pair("SARN FORD", "THARBAD"), 150, 1 });
				m_segments.push_back({ generator(), std::make_pair("MORIA GATE", "THARBAD"), 175, 1 });
				m_segments.push_back({ generator(), std::make_pair("MORIA GATE", "DIMRILL DALE"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("DOL GULDUR", "DIMRILL DALE"), 200, 1 });
				m_segments.push_back({ generator(), std::make_pair("LAST BRIDGE", "THARBAD"), 200, 1 });
				m_segments.push_back({ generator(), std::make_pair("LAST BRIDGE", "RIVENDELL"), 100, 1 });
				m_segments.push_back({ generator(), std::make_pair("HIGH PASS", "RIVENDELL"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("HIGH PASS", "GOBLIN GATE"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("HIGH PASS", "OLD FORD"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("BEORN", "OLD FORD"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("BEORN", "WOOD ELVES"), 125, 1 });
				m_segments.push_back({ generator(), std::make_pair("ESGAROTH", "WOOD ELVES"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("EREBOR", "WOOD ELVES"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("EREBOR", "ESGAROTH"), 25, 1 });
				m_segments.push_back({ generator(), std::make_pair("MOUNT GUNDABAD", "GOBLIN GATE"), 100, 1 });
				m_segments.push_back({ generator(), std::make_pair("MOUNT GUNDABAD", "CARN DUM"), 100, 1 });
				m_segments.push_back({ generator(), std::make_pair("ESGAROTH", "OLD FORD"), 150, 1 });
				m_segments.push_back({ generator(), std::make_pair("LAST BRIDGE", "WEATHERTOP"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("BREE", "WEATHERTOP"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("BREE", "SARN FORD"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("BREE", "FORNOST"), 50, 1 });
				m_segments.push_back({ generator(), std::make_pair("HOBBITON", "SARN FORD"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("HOBBITON", "BREE"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("HOBBITON", "GREY HAVENS"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("FORLOND", "GREY HAVENS"), 75, 1 });
				m_segments.push_back({ generator(), std::make_pair("HARLOND", "GREY HAVENS"), 50, 1 });

				verify_segments();

				std::cout << "Stations total: " << m_stations.size() << std::endl;
				std::cout << "Segments total: " << m_segments.size() << std::endl;

				Data::save(m_stations);
				Data::save(m_segments);

				generate_routes();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::verify_segments() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & segment : m_segments)
				{
					if (std::find_if(m_stations.begin(), m_stations.end(),
						[&segment](const auto & station)
					{
						return (station.name == segment.stations.first);
					}) == m_stations.end())
					{
						throw graph_exception("cannot find first station: \"" + segment.stations.first + "\"");
					}

					if (std::find_if(m_stations.begin(), m_stations.end(),
						[&segment](const auto& station)
					{
						return (station.name == segment.stations.second);
					}) == m_stations.end())
					{
						throw graph_exception("cannot find second station: \"" + segment.stations.second + "\"");
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::generate_routes()
		{
			RUN_LOGGER(logger);

			try
			{
				m_routes.push_back(Route(Route::Type::cargo, 
					{
						"EREBOR", "ESGAROTH", "OLD FORD", "HIGH PASS", "RIVENDELL",
						"LAST BRIDGE", "WEATHERTOP", "BREE", "HOBBITON", "GREY HAVENS"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::military,
					{
						"CARN DUM", "MOUNT GUNDABAD", "GOBLIN GATE", "HIGH PASS", "RIVENDELL",
						"LAST BRIDGE", "WEATHERTOP", "BREE" 
					}, m_segments));

				m_routes.push_back(Route(Route::Type::passenger,
					{
						"EREBOR", "WOOD ELVES", "BEORN", "OLD FORD", "HIGH PASS", "RIVENDELL"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::passenger,
					{
						"FORNOST", "BREE", "SARN FORD", "THARBAD", "LOND DAER"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::passenger,
					{
						"FORLOND", "GREY HAVENS", "HOBBITON", "BREE"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::passenger,
					{
						"HARLOND", "GREY HAVENS", "HOBBITON", "BREE"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::military,
					{
						"LOND DAER", "THARBAD", "MORIA GATE", "DIMRILL DALE", "DOL GULDUR"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::cargo,
					{
						"GREY HAVENS", "HOBBITON", "SARN FORD", "THARBAD", "HELMS DEEP",
						"EDORAS", "MINAS TIRITH" 
					}, m_segments));

				m_routes.push_back(Route(Route::Type::passenger,
					{
						"EDORAS", "HELMS DEEP", "ISENGARD"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::military,
					{
						"MINAS TIRITH", "OSGILIATH", "MINAS MORGUL", "MOUNT DOOM", "BARAD-DUR",
						"ISENMOUTHE", "THE BLACK GATE", "OSGILIATH", "MINAS TIRITH"
					}, m_segments));

				m_routes.push_back(Route(Route::Type::cargo,
					{
						"MINAS TIRITH", "OSGILIATH", "PELARGIR", "EDHELLOND"
					}, m_segments));

				Data::save(m_routes);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

	} // namespace generator

} // namespace solution