#ifndef SOLUTION_TEACHER_TEACHER_HPP
#define SOLUTION_TEACHER_TEACHER_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <Windows.h>

#include <boost/filesystem.hpp>

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

		class Teacher : public QWidget
		{
			Q_OBJECT

		private:

			class Help : public QDialog
			{
			public:

				Help(QWidget * parent = nullptr) : QDialog(parent)
				{
					initialize();
				}

				~Help() noexcept = default;

			private:

				void initialize();

			private:

				QLabel       * m_label;
				QTextBrowser * m_text;
				QPushButton  * m_button;
			};

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

			void browse_filesystem_for_input();

			void browse_filesystem_for_model();

			void run();

			bool has_correct_directories() const;

			void create_progress_dialog();

			void create_process_module();

			void perform();

			void cancel();

		private:

			static const auto outer_font_size = 12;
			static const auto inner_font_size = 10;

			static const auto max_progress_value = 100;

		private:

			QLabel      * m_label_modules;
			QComboBox   * m_combo_modules;
			QLabel      * m_label_input;
			QLineEdit   * m_line_input;
			QPushButton * m_button_input;
			QCheckBox   * m_check_input;
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
			QTimer * m_timer;

			STARTUPINFOA m_startup_information;
			PROCESS_INFORMATION m_process_information;

		private:

			mutable std::atomic < int > m_progress_value;
		};

	} // namespace teacher

} // namespace solution

#endif // #ifndef SOLUTION_TEACHER_TEACHER_HPP