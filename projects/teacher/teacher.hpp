#ifndef SOLUTION_TEACHER_TEACHER_HPP
#define SOLUTION_TEACHER_TEACHER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <atomic>
#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <Windows.h>

#include <QtWidgets>

#include "../shared/source/logger/logger.hpp"

namespace solution
{
	namespace teacher
	{
		class teacher_exception : public std::exception
		{
		public:

			explicit teacher_exception(const std::string& message) noexcept :
				std::exception(message.c_str())
			{}

			explicit teacher_exception(const char* const message) noexcept :
				std::exception(message)
			{}

			~teacher_exception() noexcept = default;
		};

		class Help : public QDialog
		{
			Q_OBJECT

		public:

			Help(QWidget * parent = nullptr) : QDialog(parent)
			{
				initialize();
			}

			~Help() noexcept = default;

		private:

			void initialize();
		};

		class Teacher : public QWidget
		{
			Q_OBJECT

		public:

			Teacher(QWidget * parent = nullptr) : QWidget(parent)
			{
				initialize();
			}

			~Teacher() noexcept = default;

		private:

			void initialize();

		private:

			void show_help();

			void browse_filesystem_for_data();

			void browse_filesystem_for_model();

			void run();

			void cancel();

		private:

			QLabel      * m_label_modules;
			QComboBox   * m_combo_modules;
			QLabel      * m_label_data;
			QLineEdit   * m_line_data;
			QPushButton * m_button_data;
			QCheckBox   * m_check_data;
			QLabel      * m_label_model;
			QLineEdit   * m_line_model;
			QPushButton * m_button_model;
			QLabel      * m_label_status;
			QLabel      * m_label_error;
			QPushButton * m_button_run;
			QPushButton * m_button_verify;
			QPushButton * m_button_remove;
			QPushButton * m_button_help;

			QProgressDialog * m_progress;

			STARTUPINFOA m_startup_information;
			PROCESS_INFORMATION m_process_information;

		private:

			mutable std::atomic < int > progress_value;
		};

	} // namespace teacher

} // namespace solution

#endif // #ifndef SOLUTION_TEACHER_TEACHER_HPP