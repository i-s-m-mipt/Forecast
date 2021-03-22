#include "segment.hpp"

namespace solution
{
	namespace system
	{
		namespace module
		{
			void Segment::train_arrived(std::size_t position) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_size == m_capacity)
					{
						throw std::overflow_error("too many trains on segment");
					}

					if (m_positions.at(position))
					{
						throw std::overflow_error("too many trains on position");
					}

					++m_size;

					m_positions.at(position) = true;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			void Segment::train_departured(std::size_t position) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_size == 0U)
					{
						throw std::underflow_error("not enough trains on segment");
					}

					if (!m_positions.at(position))
					{
						throw std::underflow_error("not enough trains on position");
					}

					--m_size;

					m_positions.at(position) = false;
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

			std::size_t Segment::get_free_position() const
			{
				RUN_LOGGER(logger);

				try
				{
					auto result = std::find(std::begin(m_positions), std::end(m_positions), false);

					if (result == std::end(m_positions))
					{
						throw std::overflow_error("no free position");
					}

					return std::distance(std::begin(m_positions), result);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

		} // namespace module

	} // namespace system

} // namespace solution