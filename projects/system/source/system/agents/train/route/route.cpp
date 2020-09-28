#include "route.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			boost::uuids::string_generator Route::string_generator;

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

		} // namespace agents

	} // namespace system

} // namespace solution