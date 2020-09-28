#ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP
#define SOLUTION_SYSTEM_AGENTS_ROUTE_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
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
					Record() noexcept = default;

					template < typename Station, typename Enable =
						std::enable_if_t < std::is_convertible_v < Station, std::string > > >
					explicit Record(Station && station_v, std::time_t arrival_v, std::time_t departure_v) :
						station(std::forward < Station > (station_v)), arrival(arrival_v), departure(departure_v)
					{}

					~Record() noexcept = default;

					std::string station;

					std::time_t arrival   = 0;
					std::time_t departure = 0;
				};

			public:

				using records_container_t = std::vector < Record > ;

			public:

				Route() noexcept = default;

				template < typename Id, typename Records, typename Enable = 
					std::enable_if_t < 
						std::is_convertible_v < Id, id_t > &&
						std::is_convertible_v < Records, records_container_t > > >
				explicit Route(Id && id, Records && records) :
					m_id(std::forward < Id > (id)), m_records(std::forward < Records > (records))
				{}

				~Route() noexcept = default;

			public:

				const auto & id() const noexcept
				{
					return m_id;
				}

				const auto & records() const noexcept
				{
					return m_records;
				}

			public:

				bool empty() const;

			public:

				static boost::uuids::string_generator string_generator;

			private:

				const id_t m_id;

			private:

				records_container_t m_records;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP