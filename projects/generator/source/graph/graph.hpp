#ifndef SOLUTION_GENERATOR_GRAPH_HPP
#define SOLUTION_GENERATOR_GRAPH_HPP

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
		class graph_exception : public std::exception
		{
		public:

			explicit graph_exception(const std::string & message) noexcept :
				std::exception(message.c_str())
			{}

			explicit graph_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~graph_exception() noexcept = default;
		};

		class Graph
		{
		private:

			struct Segment
			{
				using id_t = boost::uuids::uuid;

				id_t id;

				std::pair < std::string, std::string > stations;

				std::size_t length; // kilometers

				std::size_t train_limit;
			};

		private:

			struct Station
			{
				std::string name;

				std::vector < Segment::id_t > segments;
			};

		private:

			using stations_container_t = std::vector < Station > ;
			using segments_container_t = std::vector < Segment > ;

		private:

			class Route
			{
			public:

				using id_t = int;

			public:

				enum class Type
				{
					cargo,
					military,
					passenger
				};

			public:

				explicit Route(Type type, const std::vector < std::string > & route,
					const segments_container_t & segments) : 
					m_id(++last_id), m_type(type), m_station_delay(get_station_delay(m_type)),
					m_movement_factor(get_movement_factor(m_type))
				{
					initialize(route, segments);
				}

				~Route() noexcept = default;

			private:

				std::time_t get_station_delay(Type type) const;

				double get_movement_factor(Type type) const;

			private:

				void initialize(const std::vector < std::string > & route, 
					const segments_container_t & segments);

			public:

				const auto id() const noexcept
				{
					return m_id;
				}

				const auto type() const noexcept
				{
					return m_type;
				}

				const auto & timetable() const noexcept
				{
					return m_timetable;
				}

			private:

				static id_t last_id;

				
			private:

				id_t m_id;
				Type m_type;
				std::time_t m_station_delay;
				double m_movement_factor;

				std::vector < std::pair < std::string, std::time_t > > m_timetable;
			};

		private:

			using routes_container_t = std::vector < Route > ;

		private:

			class Data
			{
			private:

				using json_t = boost::extended::serialization::json;

			private:

				struct Key
				{
					struct Station
					{
						static inline const std::string name     = "name";
						static inline const std::string segments = "segments";
					};

					struct Segment
					{
						static inline const std::string id          = "id";
						static inline const std::string station_1   = "station_1";
						static inline const std::string station_2   = "station_2";
						static inline const std::string length      = "length";
						static inline const std::string train_limit = "train_limit";
					};

					struct Route
					{
						static inline const std::string id        = "id";
						static inline const std::string type      = "type";
						static inline const std::string timetable = "timetable";
						static inline const std::string station   = "station";
						static inline const std::string delta     = "delta";
					};
				};

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t stations_data = "graph/data/stations.data";
					static inline const path_t segments_data = "graph/data/segments.data";

					static inline const path_t routes_data = "graph/data/routes.data";
				};

			private:

				using path_t = File::path_t;

			public:

				static void save(const stations_container_t & stations);
				static void save(const segments_container_t & segments);

				static void save(const routes_container_t & routes);

			private:

				static void save(const path_t & path, const json_t & object);
			};

		public:

			Graph() noexcept = default;

			~Graph() noexcept = default;

		public:

			void generate();

		private:

			void verify_segments() const;

			void generate_routes();

		private:

			stations_container_t m_stations;
			segments_container_t m_segments;

			routes_container_t m_routes;
		};

	} // namespace generator

} // namespace solution

#endif // #ifndef SOLUTION_GENERATOR_GRAPH_HPP