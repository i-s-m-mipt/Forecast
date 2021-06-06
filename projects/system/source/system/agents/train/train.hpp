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

				using Segment = module::Segment;

			public:

				enum class Direction
				{
					north,
					south
				};

			public:

				Train() = default;

				template < typename I, typename T, typename P, typename Enable =
					std::enable_if_t < 
						std::is_convertible_v < I, id_t > && 
						std::is_convertible_v < T, std::string > && 
						std::is_convertible_v < P, std::string > > >
				explicit Train(I && id_v, T && type_v, Direction direction_v, P && begin_v, P && end_v, double weight_k_v) :
					id(std::forward < I > (id_v)), type(std::forward < I > (type_v)), direction(direction_v), 
					begin(std::forward < P > (begin_v)), end(std::forward < P > (end_v)), weight_k(weight_k_v),
					m_deviation(0.0), m_position(begin), m_position_time(0LL)
				{
					initialize();
				}

				~Train() noexcept = default;

			private:

				void initialize();

			public:

				const auto deviation() const noexcept
				{
					return m_deviation;
				}

				const auto & position() const noexcept
				{
					return m_position;
				}

			public:

				bool is_ready(std::time_t standard_time) const;

				void stay(std::time_t standard_time);

				void move(const std::string & position) const;

			public:

				const id_t id;
				const std::string type;
				const Direction direction;
				const std::string begin;
				const std::string end;
				const double weight_k;

			private:

				double m_deviation;

			private:

				mutable std::string m_position;
				mutable std::time_t m_position_time;
			};

		} // namespace agents

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_AGENTS_TRAIN_HPP