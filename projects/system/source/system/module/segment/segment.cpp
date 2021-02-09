#include "segment.hpp"

namespace solution
{
	namespace system
	{
		namespace module
		{
			void Segment::train_arrived() const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_size == m_capacity)
					{
						throw std::overflow_error("too many trains on segment");
					}

					++m_size;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			void Segment::train_departured() const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_size == 0U)
					{
						throw std::underflow_error("not enough trains on segment");
					}

					--m_size;
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
					return (m_size < m_capacity && m_state != State::locked);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::add_adgacent_segment(const id_t & id) const
			{
				RUN_LOGGER(logger);

				try
				{
					return m_adjacent_segments.insert(id).second;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

		} // namespace module

	} // namespace system

} // namespace solution