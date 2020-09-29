#include "train.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
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