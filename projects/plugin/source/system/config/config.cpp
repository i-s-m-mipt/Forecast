#include "config.hpp"

namespace solution
{
	namespace plugin
	{
		id_t generate_null_id()
		{
			return detail::null_generator_t()();
		}

		id_t generate_random_id()
		{
			return detail::random_generator_t()();
		}

		id_t generate_string_id(const std::string & id)
		{
			return detail::string_generator_t()(id);
		}

	} // namespace plugin

} // namespace solution