#ifndef SOLUTION_SYSTEM_SYSTEM_HPP
#define SOLUTION_SYSTEM_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <cassert>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/extended/serialization/json.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

			using id_t = boost::uuids::uuid;

			using Segment = module::Segment;

			// using segments_container_t = std::vector < std::shared_ptr < Segment > > ;

			using segments_container_t = std::unordered_map < id_t, std::shared_ptr < Segment >, boost::hash < id_t > > ;

			using Train = agents::Train;

			// using trains_container_t = std::vector < std::shared_ptr < Train > >;

			using trains_container_t = std::unordered_map < id_t, std::shared_ptr < Train >, boost::hash < id_t > > ;

			using Route = agents::Route;

			// using routes_container_t = std::vector < std::shared_ptr < Route > >;

			using routes_container_t = std::unordered_map < id_t, std::shared_ptr < Route >, boost::hash < id_t > > ;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t segments_data = "system/data/segments.data";
					static inline const path_t routes_data   = "system/data/routes.data";
					static inline const path_t trains_data   = "system/data/trains.data";

					static inline const path_t segments_order_txt = "segments_order.txt";
				};

			private:

				using path_t = File::path_t;

			private:

				struct Key
				{
					struct Segment
					{
						static inline const std::string id				  = "id";
						static inline const std::string type			  = "type";
						static inline const std::string name			  = "name";
						static inline const std::string length			  = "length";
						static inline const std::string adjacent_segments = "adjacent_segments";
					};

					struct Route
					{
						static inline const std::string id		  = "id";
						static inline const std::string records	  = "records";
						static inline const std::string station	  = "station";
						static inline const std::string arrival	  = "arrival";
						static inline const std::string departure = "departure";
					};

					struct Train
					{
						static inline const std::string id					= "id";
						static inline const std::string name				= "name";
						static inline const std::string code				= "code";
						static inline const std::string type				= "type";
						static inline const std::string weight_k			= "weight_k";
						static inline const std::string route_id			= "route_id";
						static inline const std::string speed				= "speed";
						static inline const std::string length				= "length";
						static inline const std::string current_segment_id	= "current_segment_id";
						static inline const std::string previous_segment_id	= "previous_segment_id";
					};
				};

			public:

				static void load(segments_container_t & segments);

				static void load(trains_container_t & trains);

				static void load(routes_container_t & routes);

				static void save_segments_order(const segments_container_t & segments);

			private:

				static void load(const path_t & path, json_t & object);
			};

		private:

			using v_in_element_t = int;

			using v_in_t = std::vector < v_in_element_t > ;

			using v_out_element = bool;

			using v_out_t = std::vector < v_out_element > ;

		public:

			System()
			{
				initialize();
			}

			~System() noexcept = default;

		private:

			void initialize();

		private:

			void load();

		private:

			void prepare_trains();

			void prepare_segments();

		public:

			void run();

			void stop();

		private:

			bool has_train_on_route() const;

			bool has_ready_train_on_route() const;

			v_in_t make_input_vector() const;

			void print_input_vector(const v_in_t & v_in) const;

			v_out_t make_output_vector() const;

			void print_output_vector(const v_out_t & v_out) const;

		private:

			const std::time_t limit_time = 60 * 24;

		private:

			segments_container_t m_segments;

			trains_container_t m_trains;

			routes_container_t m_routes;
		};

	} // namespace system

} // namespace solution

#endif // #ifndef SOLUTION_SYSTEM_SYSTEM_HPP