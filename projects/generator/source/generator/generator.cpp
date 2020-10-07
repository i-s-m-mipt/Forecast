#include "generator.hpp"

namespace solution
{
	namespace generator
	{
		void Generator::Data::save(const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & segment : segments)
				{
					json_t element;

					json_t adjacent_segments;

					for (const auto & id : segment.adjacent_segments)
					{
						adjacent_segments.push_back(boost::uuids::to_string(id));
					}

					element[Key::Segment::id]                = boost::uuids::to_string(segment.id);
					element[Key::Segment::type]              = static_cast < std::size_t > (segment.type);
					element[Key::Segment::name]              = segment.name;
					element[Key::Segment::station]			 = segment.station;
					element[Key::Segment::length]            = segment.length;
					element[Key::Segment::adjacent_segments] = adjacent_segments;

					array.push_back(element);
				}

				save(File::segments_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::Data::save(const routes_container_t & routes)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & route : routes)
				{
					json_t element;

					element[Key::Route::id] = boost::uuids::to_string(route.second.id);

					json_t records;

					for (const auto & record : route.second.records)
					{
						json_t t;

						t[Key::Route::station]   = record.station;
						t[Key::Route::arrival]   = record.arrival;
						t[Key::Route::departure] = record.departure;

						records.push_back(std::move(t));
					}

					element[Key::Route::records] = std::move(records);

					array.push_back(element);
				}

				save(File::routes_data, array);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::Data::save(const trains_container_t & trains)
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & train : trains)
				{
					json_t element;

					element[Key::Train::id] = boost::uuids::to_string(train.id);
					element[Key::Train::name] = train.name;
					element[Key::Train::code] = train.code;
					element[Key::Train::type] = train.type;
					element[Key::Train::weight_k] = train.weight_k;
					element[Key::Train::route_id] = boost::uuids::to_string(train.route_id);
					element[Key::Train::speed] = train.speed;
					element[Key::Train::length] = train.length;
					element[Key::Train::current_segment_id] = boost::uuids::to_string(train.current_segment_id);
					element[Key::Train::previous_segment_id] = boost::uuids::to_string(train.previous_segment_id);

					array.push_back(element);
				}

				save(File::trains_data, array);
			}
			catch (const std::exception& exception)
			{
				shared::catch_handler < generator_exception >(logger, exception);
			}
		}

		void Generator::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw generator_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::run()
		{
			RUN_LOGGER(logger);

			try
			{
				generate_stations();

				generate_segments();

				connect_segments();

				generate_routes();

				generate_trains();

				Data::save(m_segments);
				Data::save(m_routes);
				Data::save(m_trains);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::generate_stations()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto n_stations = 6U; // количество станций на линейном графе, фикс.

				m_stations.reserve(n_stations);

				//for (auto i = 0U; i < n_stations; ++i)
				//{
				//	m_stations.push_back("Station_" + std::to_string(i));
				//}

				m_stations.push_back("BAG");
				m_stations.push_back("MAA");
				m_stations.push_back("AGU");
				m_stations.push_back("NAE");
				m_stations.push_back("OOR");
				m_stations.push_back("LUN");
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::generate_segments()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto n_stations = m_stations.size();
				boost::uuids::random_generator generator;

				/*
				const auto n_reserves = 3U;  // количество станционных путей на станции, фикс.
				const auto n_railways = 10U; // количество блок-участков на перегоне, фикс.

				boost::uuids::random_generator generator;
				Segment::Type type = Segment::Type::station;
				auto station_index = 0U;
				const auto length = 1000U; // (m)

				for (auto i = 0U, j = 0U; i < n_stations * n_reserves + (n_stations - 1) * n_railways; ++i, ++j)
				{
					if (j == n_reserves && type == Segment::Type::station)
					{
						j -= n_reserves;
						type = Segment::Type::railway;
					}

					if (j == n_railways && type == Segment::Type::railway)
					{
						j -= n_railways;
						type = Segment::Type::station;

						++station_index;
					}

					Segment segment;

					segment.id     = generator();
					segment.type   = type;
					segment.name   = (type == Segment::Type::station ? m_stations[station_index] : "");
					segment.length = length;

					m_segments.push_back(segment);
				}
				*/

				Segment segment;

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS4P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS5P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS6P_BAG";
				segment.station = "BAG";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::railway;
				segment.name = "SLZ_GPC@MAA";
				segment.station = "";
				segment.length = 10000;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P@MAA";
				segment.station = "MAA";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P@MAA";
				segment.station = "MAA";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P@MAA";
				segment.station = "MAA";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS4P@MAA";
				segment.station = "MAA";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS5P@MAA";
				segment.station = "MAA";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::railway;
				segment.name = "SLZ_GPC@AGU";
				segment.station = "";
				segment.length = 10000;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P@AGU";
				segment.station = "AGU";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P@AGU";
				segment.station = "AGU";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P@AGU";
				segment.station = "AGU";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::railway;
				segment.name = "SLZ_GPC_NAE";
				segment.station = "";
				segment.length = 10000;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P_NAE";
				segment.station = "NAE";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P_NAE";
				segment.station = "NAE";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P_NAE";
				segment.station = "NAE";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::railway;
				segment.name = "SLZ_GPC@OOR";
				segment.station = "";
				segment.length = 10000;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P@OOR";
				segment.station = "OOR";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P@OOR";
				segment.station = "OOR";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P@OOR";
				segment.station = "OOR";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::railway;
				segment.name = "SLZ_GPC@LUN";
				segment.station = "";
				segment.length = 10000;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS1P@LUN";
				segment.station = "LUN";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS2P@LUN";
				segment.station = "LUN";
				segment.length = 600;

				m_segments.push_back(segment);

				segment.id = generator();
				segment.type = Segment::Type::station;
				segment.name = "TS3P@LUN";
				segment.station = "LUN";
				segment.length = 600;

				m_segments.push_back(segment);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::connect_segments()
		{
			RUN_LOGGER(logger);

			try
			{
				/*
				for (auto i = 0U; i < m_segments.size(); ++i)
				{
					if (m_segments[i].type == Segment::Type::station)
					{
						int j = i - 1;

						for (; j >= 0 && m_segments[j].type == Segment::Type::station; --j);

						if (j != -1)
						{
							m_segments[i].adjacent_segments.push_back(m_segments[j].id);
						}

						int k = i + 1;

						for (; k < m_segments.size() && m_segments[k].type == Segment::Type::station; ++k);

						if (k != m_segments.size())
						{
							m_segments[i].adjacent_segments.push_back(m_segments[k].id);
						}
					}
					else
					{
						if (m_segments[i - 1].type == Segment::Type::station)
						{
							for (int j = i - 1; j >= 0 && m_segments[j].type == Segment::Type::station; --j)
							{
								m_segments[i].adjacent_segments.push_back(m_segments[j].id);
							}
						}
						else
						{
							m_segments[i].adjacent_segments.push_back(m_segments[i - 1].id);
						}

						if (m_segments[i + 1].type == Segment::Type::station)
						{
							for (int j = i + 1; j < m_segments.size() && m_segments[j].type == Segment::Type::station; ++j)
							{
								m_segments[i].adjacent_segments.push_back(m_segments[j].id);
							}
						}
						else
						{
							m_segments[i].adjacent_segments.push_back(m_segments[i + 1].id);
						}
					}
				}
				*/

				m_segments[0].adjacent_segments.push_back(m_segments[6].id);
				m_segments[1].adjacent_segments.push_back(m_segments[6].id);
				m_segments[2].adjacent_segments.push_back(m_segments[6].id);
				m_segments[3].adjacent_segments.push_back(m_segments[6].id);
				m_segments[4].adjacent_segments.push_back(m_segments[6].id);
				m_segments[5].adjacent_segments.push_back(m_segments[6].id);

				m_segments[6].adjacent_segments.push_back(m_segments[0].id);
				m_segments[6].adjacent_segments.push_back(m_segments[1].id);
				m_segments[6].adjacent_segments.push_back(m_segments[2].id);
				m_segments[6].adjacent_segments.push_back(m_segments[3].id);
				m_segments[6].adjacent_segments.push_back(m_segments[4].id);
				m_segments[6].adjacent_segments.push_back(m_segments[5].id);
				m_segments[6].adjacent_segments.push_back(m_segments[7].id);
				m_segments[6].adjacent_segments.push_back(m_segments[8].id);
				m_segments[6].adjacent_segments.push_back(m_segments[9].id);
				m_segments[6].adjacent_segments.push_back(m_segments[10].id);
				m_segments[6].adjacent_segments.push_back(m_segments[11].id);

				m_segments[7].adjacent_segments.push_back(m_segments[6].id);
				m_segments[7].adjacent_segments.push_back(m_segments[12].id);
				m_segments[8].adjacent_segments.push_back(m_segments[6].id);
				m_segments[8].adjacent_segments.push_back(m_segments[12].id);
				m_segments[9].adjacent_segments.push_back(m_segments[6].id);
				m_segments[9].adjacent_segments.push_back(m_segments[12].id);
				m_segments[10].adjacent_segments.push_back(m_segments[6].id);
				m_segments[10].adjacent_segments.push_back(m_segments[12].id);
				m_segments[11].adjacent_segments.push_back(m_segments[6].id);
				m_segments[11].adjacent_segments.push_back(m_segments[12].id);

				m_segments[12].adjacent_segments.push_back(m_segments[7].id);
				m_segments[12].adjacent_segments.push_back(m_segments[8].id);
				m_segments[12].adjacent_segments.push_back(m_segments[9].id);
				m_segments[12].adjacent_segments.push_back(m_segments[10].id);
				m_segments[12].adjacent_segments.push_back(m_segments[11].id);
				m_segments[12].adjacent_segments.push_back(m_segments[13].id);
				m_segments[12].adjacent_segments.push_back(m_segments[14].id);
				m_segments[12].adjacent_segments.push_back(m_segments[15].id);

				m_segments[13].adjacent_segments.push_back(m_segments[12].id);
				m_segments[13].adjacent_segments.push_back(m_segments[16].id);
				m_segments[14].adjacent_segments.push_back(m_segments[12].id);
				m_segments[14].adjacent_segments.push_back(m_segments[16].id);
				m_segments[15].adjacent_segments.push_back(m_segments[12].id);
				m_segments[15].adjacent_segments.push_back(m_segments[16].id);

				m_segments[16].adjacent_segments.push_back(m_segments[13].id);
				m_segments[16].adjacent_segments.push_back(m_segments[14].id);
				m_segments[16].adjacent_segments.push_back(m_segments[15].id);
				m_segments[16].adjacent_segments.push_back(m_segments[17].id);
				m_segments[16].adjacent_segments.push_back(m_segments[18].id);
				m_segments[16].adjacent_segments.push_back(m_segments[19].id);

				m_segments[17].adjacent_segments.push_back(m_segments[20].id);
				m_segments[17].adjacent_segments.push_back(m_segments[16].id);
				m_segments[18].adjacent_segments.push_back(m_segments[20].id);
				m_segments[18].adjacent_segments.push_back(m_segments[16].id);
				m_segments[19].adjacent_segments.push_back(m_segments[20].id);
				m_segments[19].adjacent_segments.push_back(m_segments[16].id);

				m_segments[20].adjacent_segments.push_back(m_segments[21].id);
				m_segments[20].adjacent_segments.push_back(m_segments[22].id);
				m_segments[20].adjacent_segments.push_back(m_segments[23].id);
				m_segments[20].adjacent_segments.push_back(m_segments[17].id);
				m_segments[20].adjacent_segments.push_back(m_segments[18].id);
				m_segments[20].adjacent_segments.push_back(m_segments[19].id);

				m_segments[21].adjacent_segments.push_back(m_segments[20].id);
				m_segments[21].adjacent_segments.push_back(m_segments[24].id);
				m_segments[22].adjacent_segments.push_back(m_segments[20].id);
				m_segments[22].adjacent_segments.push_back(m_segments[24].id);
				m_segments[23].adjacent_segments.push_back(m_segments[20].id);
				m_segments[23].adjacent_segments.push_back(m_segments[24].id);

				m_segments[24].adjacent_segments.push_back(m_segments[21].id);
				m_segments[24].adjacent_segments.push_back(m_segments[22].id);
				m_segments[24].adjacent_segments.push_back(m_segments[23].id);
				m_segments[24].adjacent_segments.push_back(m_segments[25].id);
				m_segments[24].adjacent_segments.push_back(m_segments[26].id);
				m_segments[24].adjacent_segments.push_back(m_segments[27].id);

				m_segments[25].adjacent_segments.push_back(m_segments[24].id);
				m_segments[26].adjacent_segments.push_back(m_segments[24].id);
				m_segments[27].adjacent_segments.push_back(m_segments[24].id);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::generate_routes()
		{
			RUN_LOGGER(logger);

			try
			{
				boost::uuids::random_generator_mt19937 generator;

				Route route;

				std::time_t delay = 0;

				{
					route.id = generator();

					route.records.reserve(m_stations.size());

					delay = 10;

					for (auto i = 0U; i < m_stations.size(); ++i)
					{
						route.records.push_back({ m_stations[i], i * 20 + i * delay, delay });
					}

					m_routes["ALPHA"] = std::move(route);
				}

				{
					route.id = generator();

					route.records.reserve(m_stations.size());

					delay = 5;

					for (int i = static_cast < int > (m_stations.size()) - 1, j = 0; i >= 0; --i, ++j)
					{
						route.records.push_back({ m_stations[i], j * 10 + j * delay, delay });
					}

					std::reverse(route.records.begin(), route.records.end());

					m_routes["OMEGA"] = std::move(route);
				}

				{
					route.id = generator();

					route.records.reserve(m_stations.size());

					delay = 10;

					for (auto i = 0U; i < m_stations.size(); ++i)
					{
						route.records.push_back({ m_stations[i], i * 10 + i * delay, delay });
					}

					m_routes["ECHO"] = std::move(route);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

		void Generator::generate_trains()
		{
			RUN_LOGGER(logger);

			try
			{
				boost::uuids::random_generator_mt19937 generator;
				boost::uuids::nil_generator nil_generator;

				Train train;

				{
					train.id = generator();
					train.name = "ALPHA";
					train.code = "0000";
					train.type = "default";
					train.weight_k = 1.0;
					train.route_id = m_routes[train.name].id;
					train.speed = 500; // (m/min)
					train.length = 500; // (m)
					train.current_segment_id = m_segments.front().id;
					train.previous_segment_id = nil_generator();

					m_trains.push_back(std::move(train));
				}

				{
					train.id = generator();
					train.name = "OMEGA";
					train.code = "0001";
					train.type = "default";
					train.weight_k = 1.0;
					train.route_id = m_routes[train.name].id;
					train.speed = 1000; // (m/min)
					train.length = 500; // (m)
					train.current_segment_id = m_segments.back().id;
					train.previous_segment_id = nil_generator();

					m_trains.push_back(std::move(train));
				}

				{
					train.id = generator();
					train.name = "ECHO";
					train.code = "0002";
					train.type = "default";
					train.weight_k = 1.0;
					train.route_id = m_routes[train.name].id;
					train.speed = 1000; // (m/min)
					train.length = 500; // (m)
					train.current_segment_id = m_segments[1].id; // !
					train.previous_segment_id = nil_generator();

					m_trains.push_back(std::move(train));
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < generator_exception > (logger, exception);
			}
		}

	} // namespace generator

} // namespace solution