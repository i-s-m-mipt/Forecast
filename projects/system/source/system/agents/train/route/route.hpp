#ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP
#define SOLUTION_SYSTEM_AGENTS_ROUTE_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			class route_exception : public std::exception
			{
			public:

				explicit route_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit route_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~route_exception() noexcept = default;
			};

			class Route
			{
			private:

				using id_t = boost::uuids::uuid;

			public:

				struct Record
				{
					std::string station;

					std::time_t arrival;
					std::time_t departure;
				};

			public:

				using records_container_t = std::vector < Record > ;

			public:
				explicit Route(const std::string & id, const records_container_t & records) :
					m_id(m_string_generator(id)), m_records(records)
				{}

				~Route() noexcept = default;

			private:

				const id_t m_id;

			private:

				records_container_t m_records;

			private:

				mutable boost::uuids::string_generator m_string_generator;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP