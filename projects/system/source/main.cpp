#include <iostream>

#include "../../shared/source/logger/logger.hpp"

#include "teacher/teacher.hpp"

using Logger = solution::shared::Logger;

int main(int argc, char** argv)
{
	RUN_LOGGER(logger);

	try
	{
		solution::system::Teacher(10U, 100U).run();

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