#include "train.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			void Train::initialize()
			{
				RUN_LOGGER(logger);

				try
				{
					m_gid.emplace_back(m_current_segment_id, 0);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::update_state(State state)
			{
				RUN_LOGGER(logger);

				try
				{
					m_state = state;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::update_current_segment_id(const id_t & next_segment_id)
			{
				RUN_LOGGER(logger);

				try
				{
					m_previous_segment_id = m_current_segment_id;
					m_current_segment_id = next_segment_id;

					m_gid.emplace_back(m_current_segment_id, 0);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::continue_movement(std::size_t segment_length)
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_state == State::move)
					{
						++m_movement_time;

						++m_total_movement_time;

						if (m_speed * m_movement_time >= segment_length)
						{
							m_movement_time = 0;

							m_state = State::stop_move;
						}
					}
					else
					{
						throw std::logic_error("unable to continue movement");
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::reduce_route_time(std::time_t delta, const std::string & current_station)
			{
				RUN_LOGGER(logger);

				try
				{
					++m_gid.back().second;

					if (current_station.empty())
					{
						m_route->reduce_time_on_railway(delta);
					}
					else
					{
						m_state = (m_route->reduce_time_on_station(delta, current_station) ? State::stop_wait : State::wait);
					}					
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::update_deviation(const std::string & current_station)
			{
				RUN_LOGGER(logger);

				try
				{
					m_deviation += m_route->reduce_time_on_station_arrival(current_station);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			std::time_t Train::current_total_deviation() const
			{
				RUN_LOGGER(logger);

				try
				{
					std::time_t deviation = m_deviation;

					for (const auto & record : m_route->records())
					{
						if (record.arrival < 0)
						{
							deviation += std::abs(record.arrival);
						}
					}

					deviation -= m_total_movement_time;

					return deviation;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			boost::uuids::string_generator Train::string_generator;

		} // namespace agents

	} // namespace system

} // namespace solution