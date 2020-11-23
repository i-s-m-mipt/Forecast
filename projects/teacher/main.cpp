#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <QtWidgets/QApplication>

#include "../shared/source/logger/logger.hpp"

#include "teacher.hpp"

using Logger  = solution::shared::Logger;
using Teacher = solution::teacher::Teacher;

int main(int argc, char ** argv)
{
	RUN_LOGGER(logger);

	try
	{
		QCoreApplication::addLibraryPath("plugins");

		QApplication application(argc, argv);

		Teacher teacher;

		teacher.resize(410, 170);
		teacher.setFixedWidth(410);
		teacher.setFixedHeight(170);
		teacher.setWindowTitle(QString::fromLocal8Bit("IMT (version 20.11.09)"));
		teacher.setWindowIcon(QIcon("main.jpg"));
		teacher.show();
		
		return application.exec();
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