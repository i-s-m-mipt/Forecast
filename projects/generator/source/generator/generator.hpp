#ifndef SOLUTION_GENERATOR_GENERATOR_HPP
#define SOLUTION_GENERATOR_GENERATOR_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/extended/serialization/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace generator
	{
		class generator_exception : public std::exception
		{
		public:

			explicit generator_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit generator_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~generator_exception() noexcept = default;
		};

		class Generator
		{
		private:

			using stations_container_t = std::vector < std::string > ;

		private:

			struct Segment
			{
				using id_t = boost::uuids::uuid;

				using segments_container_t = std::vector < id_t > ;

				enum class Type
				{
					station,
					railway
				};

				id_t id;
				Type type;
				std::string name;
				std::size_t length;

				segments_container_t adjacent_segments;
			};

		private:

			using segments_container_t = std::vector < Segment > ;

		private:

			struct Route
			{
				using id_t = boost::uuids::uuid;

				struct Record
				{
					std::string station;

					std::time_t arrival;
					std::time_t departure;
				};

				using records_container_t = std::vector < Record > ;

				id_t id;

				records_container_t records;
			};

		private:

			using routes_container_t = std::unordered_map < std::string, Route > ;

		private:

			struct Train
			{
				using id_t = boost::uuids::uuid;

				id_t id;

				std::string name;
				std::string code;
				std::string type;

				double weight_k;

				id_t route_id;

				std::size_t speed; // (m/min)

				std::size_t length; // (m)

				id_t current_segment_id;
				id_t previous_segment_id;
			};

		private:

			using trains_container_t = std::vector < Train > ;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct Key
				{
					struct Segment
					{
						static inline const std::string id                = "id";
						static inline const std::string type              = "type";
						static inline const std::string name              = "name";
						static inline const std::string length            = "length";
						static inline const std::string adjacent_segments = "adjacent_segments";
					};

					struct Route
					{
						static inline const std::string id		  = "id";
						static inline const std::string records   = "records";
						static inline const std::string station   = "station";
						static inline const std::string arrival   = "arrival";
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
						static inline const std::string length              = "length";
						static inline const std::string current_segment_id  = "current_segment_id";
						static inline const std::string previous_segment_id = "previous_segment_id";
					};
				};

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t segments_data = "generator/data/segments.data";
					static inline const path_t routes_data   = "generator/data/routes.data";
					static inline const path_t trains_data   = "generator/data/trains.data";
				};

			private:

				using path_t = File::path_t;

			public:

				static void save(const segments_container_t & segments);

				static void save(const routes_container_t & routes);

				static void save(const trains_container_t & trains);

			private:

				static void save(const path_t & path, const json_t & object);
			};

		public:

			Generator() noexcept {};

			~Generator() noexcept = default;

		public:

			void run();

		private:

			void generate_stations();

			void generate_segments();

			void connect_segments();

			void generate_routes();

			void generate_trains();

		private:

			stations_container_t m_stations;

			segments_container_t m_segments;

			routes_container_t m_routes;

			trains_container_t m_trains;
		};

	} // namespace generator

} // namespace solution

#endif // #ifndef SOLUTION_GENERATOR_GENERATOR_HPP