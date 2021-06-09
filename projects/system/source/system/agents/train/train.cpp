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
					// ...
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::is_ready(std::time_t standard_time) const
			{
				RUN_LOGGER(logger);

				try
				{
					return (m_position_time >= standard_time);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::has_completed_movement() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (m_position == end);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::has_completed_route(std::time_t standard_time) const
			{
				RUN_LOGGER(logger);

				try
				{
					return (has_completed_movement() && is_ready(standard_time));
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::stay(std::time_t standard_time)
			{
				RUN_LOGGER(logger);

				try
				{
					++m_position_time;

					if (m_position_time > standard_time)
					{
						m_deviation += weight_k;
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::move(const std::string & position)
			{
				RUN_LOGGER(logger);

				try
				{
					m_position = position;
					m_position_time = 0LL;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

		} // namespace agents

	} // namespace system

} // namespace solution