#include "config.hpp"

namespace solution
{
	namespace system
	{
		id_t generate_null_id()
		{
			RUN_LOGGER(logger);

			try
			{
				return detail::null_generator_t()();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < std::runtime_error > (logger, exception);
			}
		}

		id_t generate_random_id()
		{
			RUN_LOGGER(logger);

			try
			{
				return detail::random_generator_t()();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < std::runtime_error > (logger, exception);
			}
		}

		id_t generate_string_id(const std::string & id)
		{
			RUN_LOGGER(logger);

			try
			{
				return detail::string_generator_t()(id);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < std::runtime_error > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution