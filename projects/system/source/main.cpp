#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../shared/source/logger/logger.hpp"

#include "teacher/teacher.hpp"

using Logger  = solution::shared::Logger;
using System  = solution::system::System;
using Teacher = solution::system::Teacher;

int main(int argc, char ** argv)
{
	RUN_LOGGER(logger);

	try
	{
		if (argc > 1)
		{
			System system(System::string_generator(std::string(argv[1])));

			system.run();

			save_system_deviation(system);
		}
		else
		{
			Teacher(64U, 100U).run();
		}

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