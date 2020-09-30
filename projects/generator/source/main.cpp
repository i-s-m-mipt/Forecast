#include <iostream>

#include "generator/generator.hpp"

#include "../../shared/source/logger/logger.hpp"

using Logger = solution::shared::Logger;

int main(int argc, char** argv)
{
	RUN_LOGGER(logger);

	try
	{
		solution::generator::Generator generator;

		generator.run();

		system("pause");

		return EXIT_SUCCESS;
	}
	catch (const std::exception & exception)
	{
		logger.write(Logger::Severity::fatal, exception.what());

		system("pause");

		return EXIT_FAILURE;
	}
	catch (...)
	{
		logger.write(Logger::Severity::fatal, "unknown exception");

		system("pause");

		return EXIT_FAILURE;
	}
}