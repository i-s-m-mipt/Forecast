#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../shared/source/logger/logger.hpp"
#include "../../shared/source/python/python.hpp"

#include "system/system.hpp"
#include "teacher/teacher.hpp"

using Logger  = solution::shared::Logger;
using Python  = solution::shared::Python;
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

			if (argument == "-plugin")
			{
				System system(System::random_generator()); // model

				system.run();
				system.save();
			}
			else
			{
				if (argument == "-function")
				{
					if (argc - 3 == 2)
					{
						solution::system::apply_genetic_algorithm(argv[2], argv[3], argv[4]);
					}
					else
					{
						solution::system::apply_genetic_algorithm(argv[2], argv[3], argv[5], argv[4], argv[6]);
					}
				}
				else
				{
					System system(System::string_generator(argument));

					system.run();

					Teacher::save_deviation(system);
				}
			}
		}
		else
		{
			Teacher(1024U).run();

			system("pause");
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