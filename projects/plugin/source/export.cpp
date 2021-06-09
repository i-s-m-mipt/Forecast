#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/dll.hpp>

#include "common/serializer.h"
#include "common/zsheader.hpp"
#include "system/system.hpp"

using Logger = solution::shared::Logger;
using System = solution::plugin::System;

struct Source
{
	std::string file;
	Config config;
	Graph segments;
	std::vector < NitkaID > routes;
	std::vector < Zapret > locks;
	int current_time = 0;
	int interval = 0;
	bool is_forecast = false;
};

namespace
{
	std::shared_ptr < aim::CSerializer > serializer;
	std::shared_ptr < Source > source;
	std::shared_ptr < std::thread > worker;
	std::shared_ptr < System > solver;
}

const char * aimInit(const char * data) 
{
	serializer = std::make_shared < aim::CSerializer > ();

	source = std::make_shared < Source > ();
	
	serializer->deserializeInit(data, source->file, source->config, source->segments);

	return serializer->serializeInitResult(std::string());
}

const char * aimWork(const char * data)
{
	return serializer->serializeInitResult(std::string());
}

const char * aimStartWork(const char * data, void(*callback)(void)) 
{
	if (worker) 
	{
		if (solver)
		{
			worker->join();
			worker.reset();
		}
		else 
		{
			return serializer->serializeStartWorkResult("worker still running");
		}
	}

	serializer->deserializeStartWork(data, source->routes, source->locks, 
		source->current_time, source->interval, source->is_forecast);

	solver = std::make_shared < System > (source->segments, source->routes, source->locks);

	worker = std::make_shared < std::thread > ([](
		const std::vector < NitkaID > & routes, const std::vector < Zapret > & locks, 
		int current_time, int interval, bool is_forecast, void(*callback)(void))
			{
				solver->run();
				callback();
			},
		source->routes, source->locks, source->current_time, source->interval, source->is_forecast, callback);

	return serializer->serializeStartWorkResult(std::string());
}

const char * aimGetResult(const char * reserved) 
{
	if (!solver->is_done())
	{
		return serializer->serializeGetResult(std::string(), 0.0, {});
	}
	else
	{
		const unsigned int second_in_minute = 60U;

		std::vector < NitkaID > nitki;

		for (const auto & [id, chart] : solver->charts())
		{
			NitkaID nitka;

			nitka.StartTime = static_cast < unsigned int > (chart.start) * second_in_minute;
			nitka.type      = chart.type;
			nitka.priority  = static_cast < int > (chart.priority);

			for (const auto & point : chart.points)
			{
				nitka.points.push_back(Point{ point.segment, 
					static_cast < unsigned int > (point.time) * second_in_minute, 0.0, 0.0, 0, false });
			}

			nitki.push_back(nitka);
		}

		std::vector< std::vector < NitkaID > > result;

		result.push_back(nitki);

		if (worker) 
		{
			worker->join();
			worker.reset();
		}

		solver.reset();

		return serializer->serializeGetResult(std::string(), 1.0, result);
	}
}

void aimClose() {}