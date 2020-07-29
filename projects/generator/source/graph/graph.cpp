#include "graph.hpp"

namespace solution
{
	namespace generator
	{
		void Graph::Data::save(const stations_container_t & stations)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & station : stations)
				{
					json_t element;

					element[Key::Station::name]     = station.name;

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
					element[Key::Segment::first_station] = segment.first_station;
					element[Key::Segment::second_station] = segment.second_station;
					element[Key::Segment::length] = segment.length;
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
					"LONG DAER",
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

						m_segments.push_back({ id, station_name, station_name, 1, 1 });

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

						m_segments.push_back({ id, station_name, station_name, 1, 1 });

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

						m_segments.push_back({ id, station_name, station_name, 1, 1 });

						m_stations.back().segments.push_back(id);
					}
				}

				// =============================================================

				m_segments.push_back({ generator(), "BARAD-DUR", "MOUNT DOOM", 50, 1 });
				m_segments.push_back({ generator(), "ISENMOUTHE", "MOUNT DOOM", 50, 1 });
				m_segments.push_back({ generator(), "BARAD-DUR", "ISENMOUTHE", 75, 1 });
				m_segments.push_back({ generator(), "THE BLACK GATE", "ISENMOUTHE", 75, 1 });
				m_segments.push_back({ generator(), "THE BLACK GATE", "OSGILIATH", 200, 1 });
				m_segments.push_back({ generator(), "MINAS MORGUL", "MOUNT DOOM", 50, 1 });
				m_segments.push_back({ generator(), "MINAS MORGUL", "OSGILIATH", 25, 1 });
				m_segments.push_back({ generator(), "MINAS TIRITH", "OSGILIATH", 25, 1 });
				m_segments.push_back({ generator(), "PELARGIR", "OSGILIATH", 100, 1 });
				m_segments.push_back({ generator(), "PELARGIR", "EDHELLOND", 225, 1 });
				m_segments.push_back({ generator(), "PELARGIR", "ERECH", 250, 1 });
				m_segments.push_back({ generator(), "ERECH", "EDHELLOND", 75, 1 });
				m_segments.push_back({ generator(), "DOL AMROTH", "EDHELLOND", 50, 1 });
				m_segments.push_back({ generator(), "MINAS TIRITH", "EDORAS", 250, 1 });
				m_segments.push_back({ generator(), "HELMS DEEP", "EDORAS", 75, 1 });
				m_segments.push_back({ generator(), "HELMS DEEP", "ISENGARD", 75, 1 });
				m_segments.push_back({ generator(), "HELMS DEEP", "THARBAD", 300, 1 });
				m_segments.push_back({ generator(), "THARBAD", "ISENGARD", 250, 1 });
				m_segments.push_back({ generator(), "LONG DAER", "THARBAD", 200, 1 });
				m_segments.push_back({ generator(), "SARN FORD", "THARBAD", 150, 1 });
				m_segments.push_back({ generator(), "MORIA GATE", "THARBAD", 175, 1 });
				m_segments.push_back({ generator(), "MORIA GATE", "DIMRILL DALE", 25, 1 });
				m_segments.push_back({ generator(), "DOL GULDUR", "DIMRILL DALE", 200, 1 });
				m_segments.push_back({ generator(), "LAST BRIDGE", "THARBAD", 200, 1 });
				m_segments.push_back({ generator(), "LAST BRIDGE", "RIVENDELL", 100, 1 });
				m_segments.push_back({ generator(), "HIGH PASS", "RIVENDELL", 25, 1 });
				m_segments.push_back({ generator(), "HIGH PASS", "GOBLIN GATE", 25, 1 });
				m_segments.push_back({ generator(), "HIGH PASS", "OLD FORD", 50, 1 });
				m_segments.push_back({ generator(), "BEORN", "OLD FORD", 25, 1 });
				m_segments.push_back({ generator(), "BEORN", "WOOD ELVES", 125, 1 });
				m_segments.push_back({ generator(), "ESGAROTH", "WOOD ELVES", 25, 1 });
				m_segments.push_back({ generator(), "EREBOR", "WOOD ELVES", 25, 1 });
				m_segments.push_back({ generator(), "EREBOR", "ESGAROTH", 25, 1 });
				m_segments.push_back({ generator(), "MOUNT GUNDABAD", "GOBLIN GATE", 100, 1 });
				m_segments.push_back({ generator(), "MOUNT GUNDABAD", "CARN DUM", 100, 1 });
				m_segments.push_back({ generator(), "ESGAROTH", "OLD FORD", 150, 1 });
				m_segments.push_back({ generator(), "LAST BRIDGE", "WEATHERTOP", 75, 1 });
				m_segments.push_back({ generator(), "BREE", "WEATHERTOP", 50, 1 });
				m_segments.push_back({ generator(), "BREE", "SARN FORD", 50, 1 });
				m_segments.push_back({ generator(), "BREE", "FORNOST", 50, 1 });
				m_segments.push_back({ generator(), "HOBBITON", "SARN FORD", 75, 1 });
				m_segments.push_back({ generator(), "HOBBITON", "BREE", 75, 1 });
				m_segments.push_back({ generator(), "HOBBITON", "GREY HAVENS", 75, 1 });
				m_segments.push_back({ generator(), "FORLOND", "GREY HAVENS", 75, 1 });
				m_segments.push_back({ generator(), "HARLOND", "GREY HAVENS", 50, 1 });

				verify();

				std::cout << "Stations total: " << m_stations.size() << std::endl;
				std::cout << "Segments total: " << m_segments.size() << std::endl;

				Data::save(m_stations);
				Data::save(m_segments);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

		void Graph::verify() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & segment : m_segments)
				{
					if (std::find_if(m_stations.begin(), m_stations.end(),
						[&segment](const auto & station)
					{
						return (station.name == segment.first_station);
					}) == m_stations.end())
					{
						throw graph_exception("cannot find first station: \"" + segment.first_station + "\"");
					}

					if (std::find_if(m_stations.begin(), m_stations.end(),
						[&segment](const auto& station)
					{
						return (station.name == segment.second_station);
					}) == m_stations.end())
					{
						throw graph_exception("cannot find second station: \"" + segment.first_station + "\"");
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < graph_exception > (logger, exception);
			}
		}

	} // namespace generator

} // namespace solution