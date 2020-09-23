#include "segment.hpp"

namespace solution
{
	namespace system
	{
		namespace module
		{
			void Segment::initialize(const std::vector < std::string > & adjacent_segments)
			{
				RUN_LOGGER(logger);

				try
				{
					m_adjacent_segments.reserve(adjacent_segments.size());

					for (const auto & id : adjacent_segments)
					{
						m_adjacent_segments.push_back(m_string_generator(id));
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

		} // namespace module

	} // namespace system

} // namespace solution