#include "controller.hpp"

namespace solution
{
	namespace plugin
	{
		void Controller::Data::load(gid_t & gid)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_gid;

				load(File::gid_data, raw_gid);

				for (const auto & raw_train : raw_gid)
				{
					auto name = raw_train[Key::Train::name].get < std::string > ();

					json_t raw_thread = raw_train[Key::Train::thread];

					thread_t thread;

					for (const auto & raw_record : raw_thread)
					{
						thread.emplace_back(
							raw_record[Key::Segment::name].get < std::string > (),
							raw_record[Key::Segment::time].get < std::time_t > ());
					}

					gid.emplace_back(name, thread);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < controller_exception > (logger, exception);
			}
		}

		void Controller::Data::load(const path_t & path, json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw controller_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < controller_exception > (logger, exception);
			}
		}

		void Controller::run()
		{
			RUN_LOGGER(logger);

			try
			{
				run_system();

				Data::load(m_gid);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < controller_exception > (logger, exception);
			}
		}

		void Controller::stop() const
		{
			RUN_LOGGER(logger);

			try
			{
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < controller_exception > (logger, exception);
			}
		}

		void Controller::run_system() const
		{
			RUN_LOGGER(logger);

			try
			{
				STARTUPINFOA startup_information;

				ZeroMemory(&startup_information, sizeof(startup_information));

				startup_information.cb = sizeof(startup_information);

				PROCESS_INFORMATION process_information;

				ZeroMemory(&process_information, sizeof(process_information));

				std::string command_line = "system -plugin fe6656d6-5a73-468b-a34b-be7f3e3fc577";

				if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
					NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &process_information))
				{
					throw controller_exception("CreateProcessA error");
				}

				WaitForSingleObject(process_information.hProcess, INFINITE);

				CloseHandle(process_information.hProcess);
				CloseHandle(process_information.hThread);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < controller_exception > (logger, exception);
			}
		}

	} // namespace plugin

} // namespace solution