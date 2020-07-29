#ifndef SOLUTION_GENERATOR_GRAPH_HPP
#define SOLUTION_GENERATOR_GRAPH_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
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

				std::string first_station;
				std::string second_station;

				std::size_t length; // miles

				std::size_t train_limit;
			};

			struct Station
			{
				std::string name;

				std::vector < Segment::id_t > segments;
			};

		private:

			using stations_container_t = std::vector < Station > ;
			using segments_container_t = std::vector < Segment > ;

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
						static inline const std::string id             = "id";
						static inline const std::string first_station  = "first_station";
						static inline const std::string second_station = "second_station";
						static inline const std::string length         = "length";
						static inline const std::string train_limit    = "train_limit";
					};
				};

			private:

				struct File
				{
					using path_t = std::filesystem::path;

					static inline const path_t stations_data = "generator/data/stations.data";
					static inline const path_t segments_data = "generator/data/segments.data";
				};

			private:

				using path_t = File::path_t;

			public:

				static void save(const stations_container_t & stations);
				static void save(const segments_container_t & segments);

			private:

				static void save(const path_t & path, const json_t & object);
			};

		public:

			Graph() noexcept = default;

			~Graph() noexcept = default;

		public:

			void generate();

		private:

			void verify() const;

		private:

			stations_container_t m_stations;
			segments_container_t m_segments;
		};

	} // namespace generator

} // namespace solution

#endif // #ifndef SOLUTION_GENERATOR_GRAPH_HPP