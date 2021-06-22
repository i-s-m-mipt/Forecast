#include "train.hpp"

namespace solution
{
	namespace plugin
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
					return (m_segment_time >= standard_time);
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
					return ((m_segment == end) || at_end);
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
					++m_segment_time;

					if (m_segment_time > standard_time)
					{
						m_deviation += priority * (m_segment_time - standard_time);
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::move(const std::string & segment)
			{
				RUN_LOGGER(logger);

				try
				{
					m_segment = segment;

					m_segment_time = 0LL;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

		} // namespace agents

	} // namespace plugin

} // namespace solution