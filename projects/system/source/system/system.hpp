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

#include <boost/bimap.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <SFML/Graphics.hpp>

#include "../../../shared/source/logger/logger.hpp"

#include "module/segment/segment.hpp"
#include "agents/train/train.hpp"
#include "agents/train/route/route.hpp"

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

			using segments_container_t = boost::multi_index::multi_index_container <
				Segment, boost::multi_index::indexed_by <
					boost::multi_index::hashed_unique < boost::multi_index::const_mem_fun <
						Segment, const id_t &, &Segment::id >, detail::id_hash_t >,
					boost::multi_index::hashed_unique < boost::multi_index::const_mem_fun <
						Segment, const std::string &, &Segment::name > > > > ;

			using Train = agents::Train;

			using trains_container_t = std::unordered_map < id_t, Train, detail::id_hash_t > ;

			using Route = agents::Route;

			using routes_container_t = std::unordered_map < id_t, std::shared_ptr < Route >, detail::id_hash_t > ;

			using active_segments_t = std::unordered_set < id_t, detail::id_hash_t > ;

			using json_t = nlohmann::json;

		private:

			struct Strategy
			{
				const id_t id;

				double deviation;
				std::time_t time;

				std::shared_ptr < trains_container_t > trains;
			};

		private:

			using strategies_container_t = boost::multi_index::multi_index_container <
				Strategy, boost::multi_index::indexed_by <
					boost::multi_index::hashed_unique < boost::multi_index::member <
						Strategy, const id_t, &Strategy::id >, detail::id_hash_t >,
					boost::multi_index::ordered_non_unique < boost::multi_index::member <
						Strategy, double, &Strategy::deviation > >,
					boost::multi_index::ordered_non_unique < boost::multi_index::member <
						Strategy, std::time_t, &Strategy::time > > > > ;

		private:

			enum class Command
			{
				stay,
				skip,
				move,
				wait,
			};

		private:

			using commands_container_t = std::unordered_map < id_t, Command, detail::id_hash_t > ;

			using variants_container_t = std::vector < commands_container_t > ;

		private:

			class Data
			{
			private:

				using json_t = nlohmann::json;

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t segments_data = "system/data/segments.data";
					static inline const path_t routes_data   = "system/data/routes.data";
					static inline const path_t trains_data   = "system/data/trains.data";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Segment
					{
						static inline const std::string id		 = "id";
						static inline const std::string name	 = "name";
						static inline const std::string capacity = "capacity";
						static inline const std::string index    = "index";
					};

					struct Route
					{
						static inline const std::string id		   = "id";
						static inline const std::string start_time = "start_time";
						static inline const std::string direction  = "direction";
						static inline const std::string weight_k   = "weight_k";
						static inline const std::string points	   = "points";
						static inline const std::string segment_id = "segment_id";
						static inline const std::string arrival	   = "arrival";
						static inline const std::string staying    = "staying";
					};
				};

			public:

				static void load(segments_container_t & segments);

				static void load(routes_container_t & routes);

			private:

				static void load(const path_t & path, json_t & object);

				static void save(const path_t & path, const json_t & object);
			};

		private:

			struct Line
			{
				using point_t  = sf::Vertex;
				using vector_t = sf::Vector2f;

				explicit Line(point_t point_0, point_t point_1)				
				{
					points[0] = point_0;
					points[1] = point_1;
				}

				~Line() noexcept = default;

				point_t points[2];
			};

		private:

			struct Ban
			{
				id_t segment_id;

				std::time_t begin;
				std::time_t end;
			};

		private:

			using bans_container_t = std::vector < Ban > ;

		public:

			System()
			{
				initialize();
			}

			~System() noexcept = default;

		private:

			void initialize();

		private:

			void generate_segments();

			void generate_routes();

		private:

			void load();

		public:

			void reinitialize();

		private:

			void clear();

		public:

			void run();

		private:

			void process(id_t strategy_id, std::time_t time, commands_container_t commands,
				segments_container_t segments);

			void register_event(const id_t & source, const segments_container_t & segments,
				active_segments_t & active_segments) const;

			void show(bool is_initial = true) const;

			void show_initial(std::vector < Line > & lines, float delta,
				unsigned int width, unsigned int height) const;

			void show_current(std::vector < Line > & lines, float delta,
				unsigned int width, unsigned int height) const;

			trains_container_t & extract_trains(const id_t & strategy_id) const;

			void make_trains(std::time_t time, const segments_container_t & segments, 
				trains_container_t & trains, commands_container_t & commands) const;

			void execute_commands(const segments_container_t & segments, trains_container_t & trains, 
				const commands_container_t & commands, active_segments_t & active_segments) const;

			variants_container_t make_commands(const segments_container_t & segments, const trains_container_t & trains,
				commands_container_t & commands, const active_segments_t & active_segments) const;

			double compute_total_deviation(const trains_container_t & trains, std::time_t time) const;

			void update_strategy(const id_t & strategy_id, double deviation, std::time_t time);

			void delete_strategy(const id_t & strategy_id);

			bool is_strategy_promising(const id_t & strategy_id, double deviation) const;

			void wait(std::time_t time) const;

			void launch_new_strategies(const variants_container_t & variants, double deviation, 
				std::time_t time, const trains_container_t & trains, const segments_container_t & segments);

			void enter_bans();

			void update_segments(segments_container_t & segments, 
				std::time_t time, active_segments_t & active_segments);

		private:

			static inline constexpr std::size_t   id_segments_interface_index = 0U;
			static inline constexpr std::size_t name_segments_interface_index = 1U;

			static inline constexpr std::size_t        id_strategies_interface_index = 0U;
			static inline constexpr std::size_t deviation_strategies_interface_index = 1U;
			static inline constexpr std::size_t      time_strategies_interface_index = 2U;

			static inline constexpr std::time_t time_limit = 1200LL;
			static inline constexpr std::time_t time_delta = 120LL;

			static inline constexpr std::size_t strategies_limit = 512U;

			static inline constexpr std::size_t bfs_limit = 4U;

		private:

			segments_container_t m_segments;

			trains_container_t m_trains;

			routes_container_t m_routes;

			strategies_container_t m_strategies;

			bans_container_t m_bans;

		private:

			mutable std::mutex m_mutex;

			mutable std::mutex m_mutex_bans; // TODO: make shared mutex
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_SYSTEM_HPP