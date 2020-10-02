#ifndef SOLUTION_SYSTEM_TEACHER_HPP
#define SOLUTION_SYSTEM_TEACHER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define BOOST_PYTHON_STATIC_LIB

#include <boost/extended/serialization/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/python.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../../../shared/source/logger/logger.hpp"
#include "../../../shared/source/python/python.hpp"
#include "../../../shared/source/timer/timer.hpp"

#include "../system/system.hpp"

namespace solution 
{
	namespace system
	{
		class teacher_exception : public std::exception
		{
		public:

			explicit teacher_exception(const std::string & message) noexcept : 
				std::exception(message.c_str()) 
			{}

			explicit teacher_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~teacher_exception() noexcept = default;
		};

		class Teacher
		{
		private:

			using systems_container_t = std::vector < System > ;

			using json_t = boost::extended::serialization::json;

		private:

			struct Key
			{
				static inline const std::string id		  = "id";
				static inline const std::string deviation = "deviation";
			};

		public:

			explicit Teacher(std::size_t n_systems, std::size_t n_generations = 1000U) :
				m_n_generations(n_generations)
			{
				initialize(n_systems);
			}

			~Teacher() noexcept = default;

		private:

			void initialize(std::size_t n_systems);

		public:

			void run();

			void stop() const;

		private:

			void run_systems() const;

			void evaluate_systems(std::size_t generation_index) const;

			void print_generation_statistics(std::size_t generation_index, 
				const std::vector < double > & deviations) const;

			void send_evaluation_data(const std::string & data) const;

			void reinitialize_systems();

		private:

			const std::size_t m_n_generations;

		private:

			systems_container_t m_systems;

			shared::Python m_python;

			boost::python::object m_module_f;
			boost::python::object m_module_g;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_TEACHER_HPP