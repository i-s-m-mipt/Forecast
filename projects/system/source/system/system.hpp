#ifndef SOLUTION_SYSTEM_SYSTEM_HPP
#define SOLUTION_SYSTEM_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#if !defined(NDEBUG)
#  define ENABLE_DEBUG_CONSOLE_OUTPUT true
#else
#  define ENABLE_DEBUG_CONSOLE_OUTPUT false
#endif

#include <algorithm>
#include <atomic>
#include <deque>
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "agents/train/train.hpp"
#include "config/config.hpp"
#include "module/segment/segment.hpp"

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

		class System
		{
		private:

			using Segment = module::Segment;

			using segments_container_t = std::unordered_map < std::string, Segment > ;

			using Train = agents::Train;

			using trains_container_t = std::vector < Train > ;

			using threads_container_t = std::multimap < std::time_t, Train > ;

		private:

			struct Lock
			{
				std::string segment;

				std::time_t begin;
				std::time_t end;
			};

		private:

			using locks_container_t = std::vector < Lock > ;

		private:

			struct Node
			{
				void update_deviation();

				segments_container_t segments;

				trains_container_t trains;

				bool has_event = false;

				double deviation = 0.0;

				Node * parent = nullptr;

				std::vector < Node * > children;
			};

		private:

			struct Point
			{
				std::string name;

				std::time_t dt;
			};

		private:

			struct Thread
			{
				std::time_t start;
				std::string type;
				double weight_k;

				std::vector < Point > points;
			};

		private:

			using gid_t = std::unordered_map < id_t, Thread, detail::id_hash_t > ;

		public:

			System() : m_head(nullptr)
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

			void generate_segments();

			void generate_threads();

		public:

			void run();

		private:

			void process();

			void make_trains(std::time_t time, Node * node) const;

			void update_segments(std::time_t time, Node * node) const;

			void execute_commands(Node * node) const;

			void make_new_nodes(Node * node);

		private:

			static inline constexpr std::time_t time_begin = 0LL;
			static inline constexpr std::time_t time_limit = 360LL;

			static inline constexpr std::size_t strategies_limit = 128U;

		private:

			segments_container_t m_segments;

			threads_container_t m_threads;

			locks_container_t m_locks;

			Node * m_head;

			std::deque < Node * > m_leafs;

			gid_t m_gid;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_SYSTEM_HPP