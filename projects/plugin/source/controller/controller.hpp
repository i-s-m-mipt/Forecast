#ifndef SOLUTION_PLUGIN_CONTROLLER_HPP
#define SOLUTION_PLUGIN_CONTROLLER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#define BOOST_USE_WINDOWS_H

#include <boost/extended/serialization/json.hpp>

#include <Windows.h>

#include "../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace plugin
	{
		class controller_exception : public std::exception
		{
		public:

			explicit controller_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit controller_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~controller_exception() noexcept = default;
		};

		class Controller
		{
		private:

			using thread_t = std::vector < std::pair < std::string, std::time_t > > ;

			using gid_t = std::vector < std::pair < std::string, thread_t > > ;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t gid_data = "gid.data";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Segment
					{
						static inline const std::string name = "name";
						static inline const std::string time = "time";
					};

					struct Train
					{
						static inline const std::string name   = "name";
						static inline const std::string thread = "thread";
					};
				};

			public:

				static void load(gid_t & gid);

			private:

				static void load(const path_t & path, json_t & object);
			};

		public:

			Controller() noexcept = default;

			~Controller() noexcept = default;

		public:

			const auto & gid() const noexcept
			{
				return m_gid;
			}

		public:

			void run();

			void stop() const;

		private:

			void run_system() const;

		private:

			gid_t m_gid;
		};

	} // namespace plugin

} // namespace solution

#endif // #ifndef SOLUTION_PLUGIN_CONTROLLER_HPP