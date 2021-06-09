#ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP
#define SOLUTION_SYSTEM_MODULE_SEGMENT_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../agents/train/train.hpp"

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

				using Direction = agents::Train::Direction;

				using segments_container_t = std::unordered_set < std::string > ;

			public:

				enum class State // TODO: add reduced speed
				{
					normal,
					locked
				};

			public:

				struct Time
				{
					std::time_t north;
					std::time_t south;
				};

			public:

				template < typename Name, typename Enable = 
					std::enable_if_t < std::is_convertible_v < Name, std::string > > >
				explicit Segment(Name && name_v) :
					name(std::forward < Name > (name_v)), state(State::normal), m_has_train(false)
				{}

				~Segment() noexcept = default;

			public:

				const auto & northern_adgacent_segments() const noexcept
				{
					return m_northern_adjacent_segments;
				}

				const auto & southern_adgacent_segments() const noexcept
				{
					return m_southern_adjacent_segments;
				}

				std::time_t standard_time(const std::string & type, Direction direction) const;

			public:

				void train_arrived();

				void train_departured();

				bool is_available() const;

				bool add_northern_adgacent_segment(const std::string & name);

				bool add_southern_adgacent_segment(const std::string & name);

				void set_standard_time(const std::string & type, Time time);

			public: // const

				std::string name;

			public:

				State state;

			private:

				segments_container_t m_northern_adjacent_segments;
				segments_container_t m_southern_adjacent_segments;

				std::unordered_map < std::string, Time > m_standard_times;

			private:

				mutable bool m_has_train;
			};

		} // namespace module

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_MODULE_SEGMENT_HPP