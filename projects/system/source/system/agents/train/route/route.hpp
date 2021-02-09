#ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP
#define SOLUTION_SYSTEM_AGENTS_ROUTE_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "../../../config/config.hpp"

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
			public:

				enum class Direction
				{
					south,
					north,
				};

			public:

				enum class Type
				{
					casual,
					special,
				};

			public:

				struct Point
				{
					Point() noexcept = default;

					template < typename Id, typename Enable =
						std::enable_if_t < std::is_convertible_v < Id, id_t > > >
					explicit Point(Id && segment_id_v, std::time_t arrival_v, std::time_t staying_v) :
						segment_id(std::forward < Id > (segment_id_v)), arrival(arrival_v), staying(staying_v)
					{}

					~Point() noexcept = default;

					id_t segment_id;

					std::time_t arrival;
					std::time_t staying;
				};

			public:

				using points_container_t = std::vector < Point > ;

			public:

				Route() noexcept = default;

				template < typename Id, typename Points, typename Enable = 
					std::enable_if_t < 
						std::is_convertible_v < Id, id_t > &&
						std::is_convertible_v < Points, points_container_t > > >
				explicit Route(Id && id, std::time_t start_time, Direction direction, 
					double weight_k, Points && points, Type type = Type::casual) :
						m_id(std::forward < Id > (id)), m_start_time(start_time), m_direction(direction),
						m_weight_k(weight_k), m_points(std::forward < Points > (points)), m_type(type)
				{}

				~Route() noexcept = default;

			public:

				const auto & id() const noexcept
				{
					return m_id;
				}

				const auto start_time() const noexcept
				{
					return m_start_time;
				}

				const auto direction() const noexcept
				{
					return m_direction;
				}

				const auto weight_k() const noexcept
				{
					return m_weight_k;
				}

				const auto & points() const noexcept
				{
					return m_points;
				}

				const auto type() const noexcept
				{
					return m_type;
				}

			private:

				id_t m_id;
				std::time_t m_start_time;
				Direction m_direction;
				double m_weight_k;
				points_container_t m_points;
				Type m_type;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_ROUTE_HPP