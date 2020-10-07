#ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP
#define SOLUTION_SYSTEM_AGENTS_TRAIN_HPP

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
			private:

				using id_t = boost::uuids::uuid;

			public:

				enum class State
				{
					wait = 0,
					move,
					stop_wait,
					stop_move
				};

			private:

				using thread_t = std::vector < std::pair < id_t, std::time_t > > ;

			public:

				template < typename Id, typename Name, typename Code, typename Type, typename Enable =
					std::enable_if_t <
						std::is_convertible_v < Id, id_t > &&
						std::is_convertible_v < Name, std::string > &&
						std::is_convertible_v < Code, std::string > &&
						std::is_convertible_v < Type, std::string > > >
				explicit Train(
					Id && id, Name && name, Code && code, Type && type, double weight_k, Id && route_id, 
					std::size_t speed, std::size_t length, Id && current_segment_id, Id && previous_segment_id) :
						m_id(std::forward < Id > (id)), m_name(std::forward < Name > (name)), m_code(std::forward < Code > (code)), 
						m_type(std::forward < Type > (type)), m_weight_k(weight_k), m_route_id(std::forward < Id > (route_id)), 
						m_speed(speed), m_length(length), m_current_segment_id(std::forward < Id > (current_segment_id)),
						m_previous_segment_id(std::forward < Id > (previous_segment_id))
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

				const auto & name() const noexcept
				{
					return m_name;
				}

				const auto weight_k() const noexcept
				{
					return m_weight_k;
				}

				const auto & route_id() const noexcept
				{
					return m_route_id;
				}

				const auto & current_segment_id() const noexcept
				{
					return m_current_segment_id;
				}

				const auto & previous_segment_id() const noexcept
				{
					return m_previous_segment_id;
				}

				const auto deviation() const noexcept
				{
					return m_deviation;
				}

				const auto route() const noexcept
				{
					return m_route;
				}

				const auto state() const noexcept
				{
					return m_state;
				}

				const auto movement_time() const noexcept
				{
					return m_movement_time;
				}

				const auto & thread() const noexcept
				{
					return m_thread;
				}

			public:

				void set_route(const Route & route)
				{
					m_route = std::make_shared < Route > (route.id(), route.records());
				}

				void update_state(State state);

				void update_current_segment_id(const id_t & next_segment_id);

				void continue_movement(std::size_t segment_length);

				void reduce_route_time(std::time_t delta, const std::string & current_station = "");

				void update_deviation(const std::string & current_station);

				std::time_t current_total_deviation() const;

			public:

				static boost::uuids::string_generator string_generator;

			private:

				const id_t m_id;
				
				const std::string m_name;
				const std::string m_code;
				const std::string m_type;

				const double m_weight_k;

				const id_t m_route_id;

				const std::size_t m_speed; // (m/min)

				const std::size_t m_length; // (m)

			private:

				id_t m_current_segment_id;
				id_t m_previous_segment_id;

				std::time_t m_deviation = 0; // (min)

				std::shared_ptr < Route > m_route = nullptr;

				State m_state = State::wait;

				std::time_t m_movement_time = 0; // (min)

				std::time_t m_total_movement_time = 0; // (min)

				thread_t m_thread; // ÌËÚÍ‡ √»ƒ
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP