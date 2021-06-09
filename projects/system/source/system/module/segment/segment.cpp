#include "segment.hpp"

namespace solution
{
	namespace system
	{
		namespace module
		{
			std::time_t Segment::standard_time(const std::string & type, Direction direction) const
			{
				RUN_LOGGER(logger);

				try
				{
					switch (direction)
					{
					case Direction::north:
					{
						return m_standard_times.at(type).north;
					}
					case Direction::south:
					{
						return m_standard_times.at(type).south;
					}
					default:
					{
						throw std::runtime_error("unknown direction");
					}
					}

					
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			void Segment::train_arrived()
			{
				RUN_LOGGER(logger);

				try
				{
					m_has_train = true;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			void Segment::train_departured()
			{
				RUN_LOGGER(logger);

				try
				{
					m_has_train = false;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::is_available() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (!m_has_train && state != State::locked);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::add_northern_adgacent_segment(const std::string & name)
			{
				RUN_LOGGER(logger);

				try
				{
					return m_northern_adjacent_segments.insert(name).second;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::add_southern_adgacent_segment(const std::string & name)
			{
				RUN_LOGGER(logger);

				try
				{
					return m_southern_adjacent_segments.insert(name).second;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			void Segment::set_standard_time(const std::string & type, Time time)
			{
				RUN_LOGGER(logger);

				try
				{
					m_standard_times[type] = time;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

		} // namespace module

	} // namespace system

} // namespace solution