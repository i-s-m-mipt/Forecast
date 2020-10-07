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
			const std::string argument(argv[1]);

			if (argument == "plugin")
			{
				System system(System::random_generator());

				system.run();

				system.save();
			}
			else
			{
				System system(System::string_generator(argument));

				system.run();

				Teacher::save_deviation(system);
			}
		}
		else
		{
			Teacher(64U, 1000U).run();
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