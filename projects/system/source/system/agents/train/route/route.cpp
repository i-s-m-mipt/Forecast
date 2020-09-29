#include "route.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			bool Route::empty() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (std::find_if(m_records.begin(), m_records.end(), [](const auto & record)
					{
						return (record.arrival != 0 || record.departure != 0);
					}) == m_records.end());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < route_exception > (logger, exception);
				}
			}

			void Route::reduce_time_on_railway(std::time_t delta)
			{
				RUN_LOGGER(logger);

				try
				{
					for (auto & record : m_records)
					{
						if (record.arrival == 0 && record.departure == 0)
						{
							continue;
						}

						record.arrival -= delta;
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < route_exception > (logger, exception);
				}
			}

			bool Route::reduce_time_on_station(std::time_t delta, const std::string & current_station)
			{
				RUN_LOGGER(logger);

				try
				{
					bool is_ready_for_departure = false;

					for (auto & record : m_records)
					{
						if (record.arrival == 0 && record.departure == 0)
						{
							continue;
						}

						if (record.station != current_station)
						{
							record.arrival -= delta;
						}
						else
						{
							record.departure = std::max(static_cast < std::time_t > (0), record.departure - delta);

							if (record.departure == 0)
							{
								is_ready_for_departure = true;
							}
						}
					}

					return is_ready_for_departure;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < route_exception > (logger, exception);
				}
			}

			std::time_t Route::reduce_time_on_station_arrival(const std::string & current_station)
			{
				RUN_LOGGER(logger);

				try
				{
					auto iterator = std::find_if(m_records.begin(), m_records.end(), [&current_station](const auto & record)
					{
						return (record.station == current_station);
					});

					if (iterator == m_records.end())
					{
						throw std::domain_error("invalid station name: " + current_station);
					}

					const auto deviation = std::abs(iterator->arrival);

					iterator->arrival = 0;

					return deviation;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < route_exception > (logger, exception);
				}
			}

			boost::uuids::string_generator Route::string_generator;

		} // namespace agents

	} // namespace system

} // namespace solution