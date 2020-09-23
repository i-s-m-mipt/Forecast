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
				explicit Train(
					const std::string & id,   const std::string & name, 
					const std::string & code, const std::string & type, double weight_k,
					const std::string & route_id, std::size_t speed, std::size_t length,
					const std::string & current_segment_id, const std::string & previous_segment_id = "") :
						m_id(m_string_generator(id)), m_name(name), m_code(code), m_type(type), m_weight_k(weight_k),
						m_route_id(m_string_generator(route_id)), m_speed(speed), m_length(length), 
						m_current_segment_id(m_string_generator(current_segment_id)),
						m_previous_segment_id(previous_segment_id.empty() ? m_nil_generator() : m_string_generator(previous_segment_id))
				{}

				~Train() noexcept = default;

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

			private:

				mutable boost::uuids::string_generator m_string_generator;
				mutable boost::uuids::nil_generator m_nil_generator;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP