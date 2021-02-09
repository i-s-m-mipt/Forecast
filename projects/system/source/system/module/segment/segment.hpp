#ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP
#define SOLUTION_SYSTEM_MODULE_SEGMENT_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "../../config/config.hpp"

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
			public:

				using segments_container_t = std::unordered_set < id_t, detail::id_hash_t > ;

			public:

				enum class State // TODO: add reduced speed
				{
					normal,
					locked
				};

			public:

				template < typename Id, typename Name, typename Enable = 
					std::enable_if_t <
						std::is_convertible_v < Id, id_t > &&
						std::is_convertible_v < Name, std::string > > >
				explicit Segment(Id && id, Name && name, std::size_t capacity, std::size_t index) :
					m_id(std::forward < Id > (id)), m_name(std::forward < Name > (name)),
					m_size(0U), m_capacity(capacity), m_state(State::normal), m_index(index)
				{}

				~Segment() noexcept = default;

			public:

				const auto & id() const noexcept
				{
					return m_id;
				}

				const auto & name() const noexcept
				{
					return m_name;
				}

				const auto size() const noexcept
				{
					return m_size;
				}

				const auto capacity() const noexcept
				{
					return m_capacity;
				}

				const auto state() const noexcept
				{
					return m_state;
				}

				const auto index() const noexcept
				{
					return m_index;
				}

				const auto & adjacent_segments() const noexcept
				{
					return m_adjacent_segments;
				}

			public:

				void train_arrived() const; // TODO: remove const

				void train_departured() const; // TODO: remove const

				bool is_available() const;

				void set_state(State state) const // TODO: remove const
				{
					m_state = state;
				}

				bool add_adgacent_segment(const id_t & id) const;

			private:

				id_t m_id;
				std::string m_name;
				std::size_t m_capacity;
				std::size_t m_index;

			private:

				mutable State m_state; // TODO: remove mutable
				mutable std::size_t m_size; // TODO: remove mutable
				mutable segments_container_t m_adjacent_segments;
			};

		} // namespace module

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP