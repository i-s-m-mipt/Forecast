#ifndef SOLUTION_SYSTEM_TEACHER_HPP
#define SOLUTION_SYSTEM_TEACHER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#define BOOST_PYTHON_STATIC_LIB
#define BOOST_USE_WINDOWS_H

//#include <boost/asio.hpp>
#include <boost/extended/serialization/json.hpp>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/python.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <Windows.h>

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

			using systems_container_t = std::unordered_map < System::id_t, double, boost::hash < System::id_t > > ;

			using json_t = boost::extended::serialization::json;

		private:

			struct Key
			{
				static inline const std::string id		  = "id";
				static inline const std::string deviation = "deviation";
			};

		private:

			using shared_memory_t = boost::interprocess::managed_shared_memory;

		public:

			explicit Teacher(std::size_t n_systems, std::size_t n_generations = 1000U) :
				m_n_generations(n_generations)
			{
				initialize(n_systems);
			}

			~Teacher() noexcept = default;

		private:

			void initialize(std::size_t n_systems);

			void uninitialize();

		private:

			void make_initialization_data() const;

			void send_initialization_data(const std::string & data) const;

		public:

			void run();

			void stop() const;

		private:

			void run_systems();

			void extract_generation_deviations();

			void evaluate_systems(std::size_t generation_index) const;

			void print_generation_statistics(std::size_t generation_index, 
				const std::vector < double > & deviations) const;

			void send_evaluation_data(const std::string & data) const;

		public:

			static void save_deviation(const System & system);

		private:

			static inline const std::string process_name = "system";

		private:

			static inline const std::string shared_memory_name = "generation";
			static inline const std::size_t shared_memory_size = 65536U;

		private:

			const std::size_t m_n_generations;

		private:

			systems_container_t m_systems;

			shared::Python m_python;

			boost::python::object m_module_g;
			boost::python::object m_module_h;

			shared_memory_t m_shared_memory;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_TEACHER_HPP