#ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP
#define SOLUTION_SYSTEM_AGENTS_TRAIN_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <cassert>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "../../../../shared/source/logger/logger.hpp"

#include "route/route.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			class train_exception : public std::exception
			{
			public:

				explicit train_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit train_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~train_exception() noexcept = default;
			};

			class Train
			{
			public:

				using Point = Route::Point;

				using gid_t = Route::points_container_t;

			public:

				Train() = default;

				template < typename Id_1, typename Id_2, typename Enable =
					std::enable_if_t < 
						std::is_convertible_v < Id_1, id_t > && 
						std::is_convertible_v < Id_2, id_t > > >
				explicit Train(Id_1 && id, Id_2 && route_id) :
					m_id(std::forward < Id_1 > (id)), m_route_id(std::forward < Id_2 > (route_id))
				{
					initialize();
				}

				~Train() noexcept = default;

			private:

				void initialize();

			public:

				const auto & id() const noexcept
				{
					return m_id;
				}

				const auto & route_id() const noexcept
				{
					return m_route_id;
				}

				const auto route() const noexcept
				{
					return m_route;
				}

				const auto deviation() const noexcept
				{
					return m_deviation;
				}

				const auto & gid() const noexcept
				{
					return m_gid;
				}

				id_t current_segment_id() const;

				id_t next_segment_id() const;

			public:

				void set_route(std::shared_ptr < Route > route);

			public:

				bool is_ready() const;

				void stay();

				void move();

				bool has_completed_move() const;

				bool has_completed_route() const;				

			private:

				id_t m_id;
				id_t m_route_id;

				std::size_t m_current_point_index = 0U;
				std::time_t m_current_staying_time = 0U;
				std::time_t m_total_movement_time = 0U;

				std::shared_ptr < Route > m_route = nullptr;

				double m_deviation = 0.0;

				gid_t m_gid;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP