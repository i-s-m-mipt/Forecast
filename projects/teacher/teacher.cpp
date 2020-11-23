#include "teacher.hpp"

namespace solution
{
	namespace teacher
	{
		void Help::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto text_height = 20;
				const auto text_width  = 380;
				const auto text_delta  = 10;

				const auto outer_font_size = 12;
				const auto inner_font_size = 10;

				const auto button_width = 100;

				auto label = new QLabel(this);

				label->setFont(QFont("Consolas", outer_font_size));
				label->setAlignment(Qt::AlignLeft);
				label->setText(QString::fromLocal8Bit(
					R"(Руководство по настройке процесса обучения)"));

				label->setGeometry(
					QRect(text_delta, text_delta, text_width, text_height));

				auto text = new QTextBrowser(this);

				text->setFont(QFont("Arial", outer_font_size));
				text->setAlignment(Qt::AlignLeft);
				text->setText(QString::fromLocal8Bit(
					R"(1. Выберите версию  интеллектуального модуля
2. Выберите директорию со входными данными
3. Выберите директорию для обученной модели
4. Запустите процесс обучения  кнопкой обучить
 
Примечание: процесс  обучения  в  зависимости от версии  интеллектуального  модуля,  размера входных   данных  и  доступной  вычислительной мощности   может   потребовать   от  нескольких десятков минут  до  нескольких  десятков часов.
)"));

				text->setGeometry(
					QRect(text_delta, 2 * text_delta + text_height, text_width, 6 * text_height + 5));
			
				//auto note = new QLabel(this);

				//note->setFont(QFont("Consolas", outer_font_size));
				//note->setAlignment(Qt::AlignLeft);
				//note->setText(QString::fromLocal8Bit(
				//	R"(Примечание: процесс обучения длится несколько часов)"));

				//note->setGeometry(
				//	QRect(text_delta, 3 * text_delta + 5 * text_height, text_width, text_height));

				auto button = new QPushButton(this);

				button->setFont(QFont("Arial", inner_font_size));
				button->setText(QString::fromLocal8Bit("Закрыть"));
				button->setGeometry(
					QRect(text_delta + text_width - button_width, 3 * text_delta + 7 * text_height + 5, button_width, 30));
				
				connect(button, &QPushButton::clicked, this, &Help::close);
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
				
				const auto outer_font_size = 12;
				const auto inner_font_size = 10;

				m_label_modules = new QLabel(this);
				m_label_modules->setFont(QFont("Consolas", outer_font_size));
				m_label_modules->setAlignment(Qt::AlignLeft);
				m_label_modules->setText(QString::fromLocal8Bit("Версия:"));
				m_label_modules->setGeometry(
					QRect(text_delta, text_delta, text_width, text_height));

				QStringList modules = { 
					QString::fromLocal8Bit("Синтез расписания на основе ГИД"), 
					/*QString::fromLocal8Bit("Интеллектуальный модуль 2"),
					QString::fromLocal8Bit("Интеллектуальный модуль 3"),*/ };

				m_combo_modules = new QComboBox(this);
				m_combo_modules->addItems(modules);
				m_combo_modules->setFont(QFont("Consolas", inner_font_size));
				m_combo_modules->setGeometry(
					QRect(2 * text_delta + text_width, text_delta, field_width, combo_height));
				m_combo_modules->setToolTip(QString::fromLocal8Bit("Выберите версию интеллектуального модуля"));

				m_button_help = new QPushButton(this);
				m_button_help->setFont(QFont("Arial", inner_font_size));
				m_button_help->setText(QString::fromLocal8Bit("?"));
				m_button_help->setGeometry(
					QRect(3 * text_delta + text_width + field_width, text_delta, combo_height, combo_height));

				connect(m_button_help, &QPushButton::clicked, this, &Teacher::show_help);

				m_label_data = new QLabel(this);
				m_label_data->setFont(QFont("Consolas", outer_font_size));
				m_label_data->setAlignment(Qt::AlignLeft);
				m_label_data->setText(QString::fromLocal8Bit("Данные:"));
				m_label_data->setGeometry(
					QRect(text_delta, 2 * text_delta + text_height, text_width, text_height));

				m_line_data = new QLineEdit(this);
				m_line_data->setFont(QFont("Consolas", inner_font_size));
				m_line_data->setGeometry(
					QRect(2 * text_delta + text_width, 2 * text_delta + text_height, field_width, combo_height));
				m_line_data->setToolTip(QString::fromLocal8Bit("Выберите директорию со входными данными"));

				m_button_data = new QPushButton(this);
				m_button_data->setFont(QFont("Arial", inner_font_size));
				m_button_data->setText(QString::fromLocal8Bit("..."));
				m_button_data->setGeometry(
					QRect(3 * text_delta + text_width + field_width, 2 * text_delta + text_height, combo_height, combo_height));

				connect(m_button_data, &QPushButton::clicked, this, &Teacher::browse_filesystem_for_data);

				//m_check_data = new QCheckBox(this);
				//m_check_data->setFont(QFont("Consolas", inner_font_size));
				//m_check_data->setText(QString::fromLocal8Bit("Конвертировать данные"));
				//m_check_data->setGeometry(
				//	QRect(2 * text_delta + text_width, 3 * text_delta + 2 * text_height, field_width, text_height));

				m_label_model = new QLabel(this);
				m_label_model->setFont(QFont("Consolas", outer_font_size));
				m_label_model->setAlignment(Qt::AlignLeft);
				m_label_model->setText(QString::fromLocal8Bit("Модель:"));
				m_label_model->setGeometry(
					QRect(text_delta, 3 * text_delta + 2 * text_height, text_width, text_height));

				m_line_model = new QLineEdit(this);
				m_line_model->setFont(QFont("Consolas", inner_font_size));
				m_line_model->setGeometry(
					QRect(2 * text_delta + text_width, 3 * text_delta + 2 * text_height, field_width, combo_height));
				m_line_model->setToolTip(QString::fromLocal8Bit("Выберите директорию для обученной модели"));

				m_button_model = new QPushButton(this);
				m_button_model->setFont(QFont("Arial", inner_font_size));
				m_button_model->setText(QString::fromLocal8Bit("..."));
				m_button_model->setGeometry(
					QRect(3 * text_delta + text_width + field_width, 3 * text_delta + 2 * text_height, combo_height, combo_height));

				connect(m_button_model, &QPushButton::clicked, this, &Teacher::browse_filesystem_for_model);
			
				m_label_status = new QLabel(this);
				m_label_status->setFont(QFont("Consolas", outer_font_size));
				m_label_status->setAlignment(Qt::AlignLeft);
				m_label_status->setText(QString::fromLocal8Bit("Статус:"));
				m_label_status->setGeometry(
					QRect(text_delta, 4 * text_delta + 3 * text_height, text_width, text_height));

				m_label_error = new QLabel(this);
				m_label_error->setFont(QFont("Arial", outer_font_size));
				m_label_error->setAlignment(Qt::AlignLeft);
				m_label_error->setText(QString::fromLocal8Bit(""));
				m_label_error->setGeometry(
					QRect(2 * text_delta + text_width, 4 * text_delta + 3 * text_height, field_width, text_height));

				m_button_run = new QPushButton(this);
				m_button_run->setFont(QFont("Arial", inner_font_size));
				m_button_run->setText(QString::fromLocal8Bit("Обучить"));
				m_button_run->setGeometry(
					QRect(2 * text_delta + text_width, 5 * text_delta + 4 * text_height, button_width, button_height));

				connect(m_button_run, &QPushButton::clicked, this, &Teacher::run);

				m_button_verify = new QPushButton(this);
				m_button_verify->setFont(QFont("Arial", inner_font_size));
				m_button_verify->setText(QString::fromLocal8Bit(""));
				m_button_verify->setGeometry(
					QRect(3 * text_delta + text_width + button_width, 5 * text_delta + 4 * text_height, button_width, button_height));
			
				m_button_remove = new QPushButton(this);
				m_button_remove->setFont(QFont("Arial", inner_font_size));
				m_button_remove->setText(QString::fromLocal8Bit(""));
				m_button_remove->setGeometry(
					QRect(4 * text_delta + text_width + 2 * button_width, 5 * text_delta + 4 * text_height, button_width, button_height));
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
				help.setWhatsThis(QString::fromLocal8Bit("Руководство по настройке процесса обучения"));

				help.exec();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::browse_filesystem_for_data()
		{
			RUN_LOGGER(logger);

			try
			{
				QString directory = QFileDialog::getExistingDirectory(this,
					tr("Find Files"), QDir::currentPath());

				if (!directory.isEmpty())
				{
					m_line_data->setText(directory);
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
				QString directory = QFileDialog::getExistingDirectory(this,
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
				const std::filesystem::path input_path = m_line_data->text().toStdString();
				const std::filesystem::path model_path = m_line_model->text().toStdString();

				if (input_path.empty() || !std::filesystem::exists(input_path))
				{
					m_label_error->setStyleSheet("QLabel { color : red; }");
					m_label_error->setText(QString::fromLocal8Bit("некорректная директория для данных"));
					return;
				}

				if (model_path.empty() || !std::filesystem::exists(model_path))
				{
					m_label_error->setStyleSheet("QLabel { color : red; }");
					m_label_error->setText(QString::fromLocal8Bit("некорректная директория для модели"));
					return;
				}

				m_label_error->setStyleSheet("QLabel { color : green; }");
				m_label_error->setText(QString::fromLocal8Bit("процесс обучения запущен ... "));

				QProgressDialog progress(
					QString::fromLocal8Bit("Обучение интеллектуального модуля ..."),
					QString::fromLocal8Bit("Прервать"), 0, 100, this);

				progress.setMinimumWidth(250);
				progress.setFixedWidth(250);
				progress.setMinimumHeight(100);
				progress.setFixedHeight(100);
				progress.setWindowIcon(QIcon("main.jpg"));
				progress.setWindowTitle(QString::fromLocal8Bit("IMT (version 20.11.09)"));
				progress.setWhatsThis(QString::fromLocal8Bit("Процесс обучения интеллектуального модуля"));

				progress.setWindowModality(Qt::WindowModal);

				progress.setValue(0);

				progress.show();

				const std::string process_name = "module";

				STARTUPINFOA startup_information;

				ZeroMemory(&startup_information, sizeof(startup_information));

				startup_information.cb = sizeof(startup_information);

				PROCESS_INFORMATION process_information;

				ZeroMemory(&process_information, sizeof(process_information));

				auto command_line = (process_name + 
					" " + m_line_data->text().toStdString() +
					" " + m_line_model->text().toStdString());

				if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
					NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &process_information))
				{
					throw teacher_exception("CreateProcessA error");
				}

				const std::filesystem::path file = "progress";

				int i = 0;

				while (i < 100)
				{
					if (std::filesystem::exists(file))
					{
						std::fstream fin(file.string(), std::ios::in);
						
						if (!fin)
						{
							throw std::logic_error("cannot open file " + file.string());
						}

						fin >> i;

						std::this_thread::sleep_for(std::chrono::milliseconds(100));

						progress.setValue(i);
					}

					if (progress.wasCanceled())
					{
						TerminateProcess(process_information.hProcess, 0);

						break;
					}
				}

				if (!progress.wasCanceled())
				{
					m_label_error->setStyleSheet("QLabel { color : green; }");
					m_label_error->setText(QString::fromLocal8Bit("процесс обучения завершен"));

					WaitForSingleObject(process_information.hProcess, INFINITE);

					CloseHandle(process_information.hProcess);
					CloseHandle(process_information.hThread);
				}
				else
				{
					m_label_error->setStyleSheet("QLabel { color : red; }");
					m_label_error->setText(QString::fromLocal8Bit("процесс обучения прерван"));
				}

				std::filesystem::remove(file);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

	} // namespace teacher

} // namespace solution