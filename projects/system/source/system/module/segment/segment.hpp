#ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP
#define SOLUTION_SYSTEM_MODULE_SEGMENT_HPP

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
		namespace module
		{
			class segment_exception : public std::exception
			{
			public:

				explicit segment_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit segment_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~segment_exception() noexcept = default;
			};

			class Segment
			{
			private:

				using id_t = boost::uuids::uuid;

				using segments_container_t = std::vector < id_t > ;

			public:

				enum class Type
				{
					station,
					railway
				};

			public:
				explicit Segment(
					const std::string & id, Type type, 
					const std::string & name, std::size_t length,
					const std::vector < std::string > & adjacent_segments) :
					m_id(m_string_generator(id)), m_type(type), m_name(name), m_length(length)
				{
					initialize(adjacent_segments);
				}

				~Segment() noexcept = default;

			private:

				void initialize(const std::vector < std::string > & adjacent_segments);

			private:

				const id_t m_id;
				const Type m_type;
				const std::string m_name;
				const std::size_t m_length; // (m)

			private:

				segments_container_t m_adjacent_segments;

			private:

				mutable boost::uuids::string_generator m_string_generator;
			};

		} // namespace module

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP