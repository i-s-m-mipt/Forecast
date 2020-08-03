#ifndef SOLUTION_SYSTEM_SYSTEM_HPP
#define SOLUTION_SYSTEM_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <stdexcept>
#include <string>

#include <boost/extended/application/service.hpp>
#include <boost/extended/serialization/json.hpp>

#include "../../../shared/source/logger/logger.hpp"

namespace solution 
{
	namespace system
	{
		class system_exception : public std::exception
		{
		public:

			explicit system_exception(const std::string & message) noexcept : 
				std::exception(message.c_str()) 
			{}

			explicit system_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~system_exception() noexcept = default;
		};

		class System : public boost::extended::application::service
		{
		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct File
				{
					using path_t = boost::filesystem::path;

					static inline const path_t actions_data = "system/data/actions.data";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Action
					{
						static inline const std::string path = "path";
						static inline const std::string name = "name";
					};
				};

			public:

				static void load(	   actions_container_t & actions);

				static void save(const actions_container_t & actions);

			private:

				static void load(const path_t & path,		json_t & object);

				static void save(const path_t & path, const json_t & object);
			};

		public:

			System()
			{
				initialize();
			}

			~System() noexcept
			{
				try
				{
					uninitialize();
				}
				catch (...) 
				{
					// std::abort();
				}
			}

		private:

			void initialize();

			void uninitialize();

		private:

			void load();

			void save();

		public:

			virtual void run() override;

			virtual void stop() override;

		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_SYSTEM_HPP