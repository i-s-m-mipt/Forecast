#ifndef SOLUTION_PLUGIN_CONFIG_HPP
#define SOLUTION_PLUGIN_CONFIG_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>

#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace plugin
	{
		using id_t = boost::uuids::uuid;

		namespace detail
		{
			using id_hash_t = boost::hash < id_t > ;

			using null_generator_t = boost::uuids::nil_generator;

			using random_generator_t = boost::uuids::random_generator;

			using string_generator_t = boost::uuids::string_generator;
		}

		id_t generate_null_id();

		id_t generate_random_id();

		id_t generate_string_id(const std::string & id);

		enum class Direction
		{
			north,
			south,
			error
		};

	} // namespace plugin

} // namespace solution

#endif // #ifndef SOLUTION_PLUGIN_CONFIG_HPP