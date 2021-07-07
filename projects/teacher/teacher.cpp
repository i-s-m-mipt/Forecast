#include "teacher.hpp"

namespace solution
{
	namespace teacher
	{
		void Teacher::Help::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto text_height = 20;
				const auto text_width  = 380;
				const auto text_delta  = 10;

				const auto button_height = 30;
				const auto button_width  = 100;

				m_label = new QLabel(this);

				m_label->setFont(QFont("Consolas", outer_font_size));
				m_label->setAlignment(Qt::AlignLeft);
				m_label->setText(QString::fromLocal8Bit(
					"Руководство по настройке процесса обучения"));

				m_label->setGeometry(
					QRect(text_delta, text_delta, text_width, text_height));

				m_text = new QTextBrowser(this);

				m_text->setFont(QFont("Arial", outer_font_size));
				m_text->setAlignment(Qt::AlignLeft);
				m_text->setText(QString::fromLocal8Bit(
					"1. Выберите версию  интеллектуального модуля\n2. Выберите директорию со входными данными\n3. Выберите директорию для обученной модели\n4. Запустите процесс обучения  кнопкой обучить\n\nПримечание: процесс  обучения  в  зависимости от версии  интеллектуального  модуля,  размера входных   данных  и  доступной  вычислительной мощности   может   потребовать   от  нескольких десятков минут  до  нескольких  десятков часов."));

				m_text->setGeometry(QRect(
					text_delta, 2 * text_delta + text_height, 
					text_width, 6 * text_height + 5));
			
				m_button = new QPushButton(this);

				m_button->setFont(QFont("Arial", inner_font_size));
				m_button->setText(QString::fromLocal8Bit("Закрыть"));
				m_button->setGeometry(QRect(
					text_delta + text_width - button_width, 3 * text_delta + 
					7 * text_height + 5, button_width, button_height));
				
				connect(m_button, &QPushButton::clicked, this, &Help::close);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto text_height = 20;
				const auto text_width  = 60;
				const auto text_delta  = 10;

				const auto combo_height = 20;
				const auto combo_width  = 125;
				const auto field_width  = 290;

				const auto button_height = 30;
				const auto button_width  = 90;

				m_label_modules = new QLabel(this);
				m_label_modules->setFont(QFont("Consolas", outer_font_size));
				m_label_modules->setAlignment(Qt::AlignLeft);
				m_label_modules->setText(QString::fromLocal8Bit("Версия:"));
				m_label_modules->setGeometry(QRect(
					text_delta, text_delta, text_width, text_height));

				QStringList modules = { 
					QString::fromLocal8Bit("Синтез расписания на основе ГИД") };

				m_combo_modules = new QComboBox(this);
				m_combo_modules->addItems(modules);
				m_combo_modules->setFont(QFont("Consolas", inner_font_size));
				m_combo_modules->setGeometry(QRect(
					2 * text_delta + text_width, text_delta, field_width, combo_height));
				m_combo_modules->setToolTip(QString::fromLocal8Bit(
					"Выберите версию интеллектуального модуля"));

				m_button_help = new QPushButton(this);
				m_button_help->setFont(QFont("Arial", inner_font_size));
				m_button_help->setText(QString::fromLocal8Bit("?"));
				m_button_help->setGeometry(QRect(
					3 * text_delta + text_width + field_width, text_delta, 
					combo_height, combo_height));
				m_button_help->setToolTip(QString::fromLocal8Bit("Справка"));

				connect(m_button_help, &QPushButton::clicked, this, &Teacher::show_help);

				m_label_input = new QLabel(this);
				m_label_input->setFont(QFont("Consolas", outer_font_size));
				m_label_input->setAlignment(Qt::AlignLeft);
				m_label_input->setText(QString::fromLocal8Bit("Данные:"));
				m_label_input->setGeometry(QRect(
					text_delta, 2 * text_delta + text_height, text_width, text_height));

				m_line_input = new QLineEdit(this);
				m_line_input->setFont(QFont("Consolas", inner_font_size));
				m_line_input->setGeometry(QRect(
					2 * text_delta + text_width, 2 * text_delta + text_height, 
					field_width, combo_height));
				m_line_input->setToolTip(QString::fromLocal8Bit(
					"Выберите директорию со входными данными"));

				m_button_input = new QPushButton(this);
				m_button_input->setFont(QFont("Arial", inner_font_size));
				m_button_input->setText(QString::fromLocal8Bit("..."));
				m_button_input->setGeometry(QRect(
					3 * text_delta + text_width + field_width, 2 * text_delta + text_height, 
					combo_height, combo_height));
				m_button_input->setToolTip(QString::fromLocal8Bit("Поиск"));

				connect(m_button_input, &QPushButton::clicked, this, &Teacher::browse_filesystem_for_input);

				m_label_model = new QLabel(this);
				m_label_model->setFont(QFont("Consolas", outer_font_size));
				m_label_model->setAlignment(Qt::AlignLeft);
				m_label_model->setText(QString::fromLocal8Bit("Модель:"));
				m_label_model->setGeometry(QRect(
					text_delta, 3 * text_delta + 2 * text_height, text_width, text_height));

				m_line_model = new QLineEdit(this);
				m_line_model->setFont(QFont("Consolas", inner_font_size));
				m_line_model->setGeometry(QRect(
					2 * text_delta + text_width, 3 * text_delta + 2 * text_height, 
					field_width, combo_height));
				m_line_model->setToolTip(QString::fromLocal8Bit(
					"Выберите директорию для обученной модели"));

				m_button_model = new QPushButton(this);
				m_button_model->setFont(QFont("Arial", inner_font_size));
				m_button_model->setText(QString::fromLocal8Bit("..."));
				m_button_model->setGeometry(QRect(
					3 * text_delta + text_width + field_width, 3 * text_delta + 
					2 * text_height, combo_height, combo_height));
				m_button_model->setToolTip(QString::fromLocal8Bit("Поиск"));

				connect(m_button_model, &QPushButton::clicked, this, &Teacher::browse_filesystem_for_model);
			
				m_label_status = new QLabel(this);
				m_label_status->setFont(QFont("Consolas", outer_font_size));
				m_label_status->setAlignment(Qt::AlignLeft);
				m_label_status->setText(QString::fromLocal8Bit("Статус:"));
				m_label_status->setGeometry(QRect(
					text_delta, 4 * text_delta + 3 * text_height, text_width, text_height));

				m_label_error = new QLabel(this);
				m_label_error->setFont(QFont("Arial", outer_font_size));
				m_label_error->setAlignment(Qt::AlignLeft);
				m_label_error->setText(QString::fromLocal8Bit(""));
				m_label_error->setGeometry(QRect(
					2 * text_delta + text_width, 4 * text_delta + 3 * text_height, 
					field_width, text_height));

				m_button_run = new QPushButton(this);
				m_button_run->setFont(QFont("Arial", inner_font_size));
				m_button_run->setText(QString::fromLocal8Bit("Обучить"));
				m_button_run->setGeometry(QRect(
					2 * text_delta + text_width, 5 * text_delta + 4 * text_height, 
					button_width, button_height));

				connect(m_button_run, &QPushButton::clicked, this, &Teacher::run);

				m_button_verify = new QPushButton(this);
				m_button_verify->setFont(QFont("Arial", inner_font_size));
				m_button_verify->setText(QString::fromLocal8Bit(""));
				m_button_verify->setGeometry(QRect(
					3 * text_delta + text_width + button_width, 5 * text_delta + 
					4 * text_height, button_width, button_height));
			
				m_button_remove = new QPushButton(this);
				m_button_remove->setFont(QFont("Arial", inner_font_size));
				m_button_remove->setText(QString::fromLocal8Bit(""));
				m_button_remove->setGeometry(QRect(
					4 * text_delta + text_width + 2 * button_width, 5 * text_delta + 
					4 * text_height, button_width, button_height));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::show_help()
		{
			RUN_LOGGER(logger);

			try
			{
				Help help;

				help.setMinimumWidth(400);
				help.setFixedWidth(400);
				help.setMinimumHeight(215);
				help.setFixedHeight(215);

				help.setWindowTitle(QString::fromLocal8Bit("Помощь"));
				help.setWindowIcon(QIcon("help.jpg"));
				help.setWhatsThis(QString::fromLocal8Bit(
					"Руководство по настройке процесса обучения"));

				help.setWindowModality(Qt::WindowModal);

				help.show();

				help.exec();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::browse_filesystem_for_input()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto directory = QFileDialog::getExistingDirectory(this,
					tr("Find Files"), QDir::currentPath());

				if (!directory.isEmpty())
				{
					m_line_input->setText(directory);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::browse_filesystem_for_model()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto directory = QFileDialog::getExistingDirectory(this,
					tr("Find Files"), QDir::currentPath());

				if (!directory.isEmpty())
				{
					m_line_model->setText(directory);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run()
		{
			RUN_LOGGER(logger);

			try
			{
				if (has_correct_directories())
				{
					create_progress_dialog();
					create_process_module();

					std::thread([this]()
						{
							const boost::filesystem::path file = "progress";

							auto value = 0;

							while (m_progress_value.load() < max_progress_value)
							{
								if (boost::filesystem::exists(file))
								{
									std::fstream fin(file.string(), std::ios::in);

									if (!fin)
									{
										throw std::logic_error("cannot open file " + file.string());
									}

									fin >> value;

									m_progress_value.store(value);
								}

								std::this_thread::sleep_for(std::chrono::milliseconds(100));
							}

							boost::filesystem::remove(file);

							if (value == max_progress_value)
							{
								m_label_error->setStyleSheet("QLabel { color : green; }");
								m_label_error->setText(QString::fromLocal8Bit(
									"процесс обучения завершен"));

								if (m_process_information.hProcess)
								{
									WaitForSingleObject(m_process_information.hProcess, INFINITE);

									CloseHandle(m_process_information.hProcess);
									CloseHandle(m_process_information.hThread);
								}
							}
						}).detach();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		bool Teacher::has_correct_directories() const
		{
			RUN_LOGGER(logger);

			try
			{
				const boost::filesystem::path input_path = m_line_input->text().toStdString();
				const boost::filesystem::path model_path = m_line_model->text().toStdString();

				if (input_path.empty() || !boost::filesystem::exists(input_path))
				{
					m_label_error->setStyleSheet("QLabel { color : red; }");
					m_label_error->setText(QString::fromLocal8Bit(
						"некорректная директория для данных"));

					return false;
				}

				if (model_path.empty() || !boost::filesystem::exists(model_path))
				{
					m_label_error->setStyleSheet("QLabel { color : red; }");
					m_label_error->setText(QString::fromLocal8Bit(
						"некорректная директория для модели"));

					return false;
				}

				m_label_error->setStyleSheet("QLabel { color : green; }");
				m_label_error->setText(QString::fromLocal8Bit(
					"процесс обучения запущен ... "));
				
				return true;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::create_progress_dialog()
		{
			RUN_LOGGER(logger);

			try
			{
				m_progress = new QProgressDialog(
					QString::fromLocal8Bit("Обучение интеллектуального модуля ..."),
					QString::fromLocal8Bit("Прервать"), 0, max_progress_value, this);

				m_progress->setMinimumWidth(250);
				m_progress->setFixedWidth(250);
				m_progress->setMinimumHeight(100);
				m_progress->setFixedHeight(100);

				m_progress->setWindowIcon(QIcon("main.jpg"));
				m_progress->setWindowTitle(QString::fromLocal8Bit(
					"IMT (version 20.11.09)"));
				m_progress->setWhatsThis(QString::fromLocal8Bit(
					"Процесс обучения интеллектуального модуля"));

				m_progress->setWindowModality(Qt::WindowModal);

				m_progress->show();

				connect(m_progress, &QProgressDialog::canceled, this, &Teacher::cancel);

				m_timer = new QTimer(this);

				connect(m_timer, &QTimer::timeout, this, &Teacher::perform);

				m_timer->start(0);

				m_progress_value.store(0);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::create_process_module()
		{
			RUN_LOGGER(logger);

			try
			{
				const std::string process_name = "module";

				ZeroMemory(&m_startup_information, sizeof(m_startup_information));
				ZeroMemory(&m_process_information, sizeof(m_process_information));

				m_startup_information.cb = sizeof(m_startup_information);

				auto command_line = (process_name +
					" \"" + m_line_input->text().toStdString() + "\"" +
					" \"" + m_line_model->text().toStdString()) + "\"";

				if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
					NULL, NULL, FALSE, 0, NULL, NULL, &m_startup_information, &m_process_information))
				{
					throw teacher_exception("CreateProcessA error");
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::perform()
		{
			RUN_LOGGER(logger);

			try
			{
				m_progress->setValue(m_progress_value.load());

				if (m_progress_value.load() == max_progress_value)
				{
					m_timer->stop();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::cancel()
		{
			RUN_LOGGER(logger);

			try
			{
				m_timer->stop();

				m_progress_value.store(100);

				TerminateProcess(m_process_information.hProcess, 0);

				m_label_error->setStyleSheet("QLabel { color : red; }");
				m_label_error->setText(QString::fromLocal8Bit(
					"процесс обучения прерван"));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

	} // namespace teacher

} // namespace solution