#include <iostream>

#include "graph/graph.hpp"

#include "../../shared/source/logger/logger.hpp"

using Logger = solution::shared::Logger;

int main(int argc, char** argv)
{
	RUN_LOGGER(logger);

	try
	{
		solution::generator::Graph graph;

		graph.generate();

		return EXIT_SUCCESS;
	}
	catch (const std::exception & exception)
	{
		logger.write(Logger::Severity::fatal, exception.what());

		return EXIT_FAILURE;
	}
	catch (...)
	{
		logger.write(Logger::Severity::fatal, "unknown exception");

		return EXIT_FAILURE;
	}
}