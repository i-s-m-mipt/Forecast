#include <iostream>

#include "../../shared/source/logger/logger.hpp"

#include "system/system.hpp"

using Logger = solution::shared::Logger;

int main(int argc, char** argv)
{
	RUN_LOGGER(logger);

	try
	{
		/*
		solution::shared::Python python;

		try
		{
			boost::python::object result =
				boost::python::exec("from script import f", python.global(), python.global());

			boost::python::object module = python.global()["f"];

			std::string message = boost::python::extract < std::string > (module("Hello, Python"));

			std::cout << message << std::endl;
		}
		catch (const boost::python::error_already_set & )
		{
			logger.write(Logger::Severity::error, python.exception());
		}
		*/

		solution::system::System().run();

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