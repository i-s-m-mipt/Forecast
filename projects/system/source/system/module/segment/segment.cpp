#include "segment.hpp"

namespace solution
{
	namespace system
	{
		namespace module
		{
			void Segment::train_arrived(const id_t & train_id)
			{
				RUN_LOGGER(logger);

				try
				{
					m_train_id = train_id;
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
					m_train_id = nil_generator();
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::has_train() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (!m_train_id.is_nil());
				}
				catch (const std::exception& exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			bool Segment::is_available_to_move() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (!has_train() && m_state == State::normal);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			boost::uuids::string_generator Segment::string_generator;

			boost::uuids::nil_generator Segment::nil_generator;

		} // namespace module

	} // namespace system

} // namespace solution