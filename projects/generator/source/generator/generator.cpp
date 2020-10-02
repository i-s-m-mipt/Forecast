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
				const auto n_stations = 10U; // количество станций на линейном графе, фикс.

				m_stations.reserve(n_stations);

				for (auto i = 0U; i < n_stations; ++i)
				{
					m_stations.push_back("Station_" + std::to_string(i));
				}
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

				const auto n_reserves = 3U;  // количество станционных путей на станции, фикс.
				const auto n_railways = 10U; // количество блок-участков на перегоне, фикс.

				boost::uuids::random_generator_mt19937 generator;
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