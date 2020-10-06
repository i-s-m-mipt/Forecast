#include "teacher.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		void Teacher::initialize(std::size_t n_systems)
		{
			RUN_LOGGER(logger);

			try
			{
				boost::python::exec("from script import g", m_python.global(), m_python.global());
				boost::python::exec("from script import h", m_python.global(), m_python.global());

				m_module_g = m_python.global()["g"];
				m_module_h = m_python.global()["h"];

				for (auto i = 0U; i < n_systems; ++i)
				{
					m_systems[System::random_generator()] = 0.0;
				}

				make_initialization_data();

				boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());

				m_shared_memory = shared_memory_t(boost::interprocess::create_only,
					shared_memory_name.c_str(), shared_memory_size);

				for (auto & system : m_systems)
				{
					m_shared_memory.construct < double > (
						boost::uuids::to_string(system.first).c_str())(0.0);
				}
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, shared::Python::exception());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				boost::interprocess::shared_memory_object::remove(shared_memory_name.c_str());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::make_initialization_data() const
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				for (const auto & system : m_systems)
				{
					array.push_back(boost::uuids::to_string(system.first));
				}

				std::stringstream sout;

				sout << array;

				send_initialization_data(sout.str());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::send_initialization_data(const std::string & data) const
		{
			RUN_LOGGER(logger);

			try
			{
				m_module_h(/*data.c_str()*/);
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, shared::Python::exception());
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
				std::cout << "Run teacher ? (y/n) ";

				if (getchar() == 'y')
				{
					std::cout << std::endl;

					for (auto i = 0U; i < m_n_generations; ++i)
					{
						shared::Timer timer("GENERATION " + std::to_string(i), std::cout);

						run_systems();

						evaluate_systems(i);
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::stop() const
		{
			RUN_LOGGER(logger);

			try
			{
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_systems()
		{
			RUN_LOGGER(logger);

			try
			{
				STARTUPINFOA startup_information;

				ZeroMemory(&startup_information, sizeof(startup_information));

				startup_information.cb = sizeof(startup_information);

				std::vector < PROCESS_INFORMATION > processes;

				processes.reserve(m_systems.size());

				for (const auto & system : m_systems)
				{
					PROCESS_INFORMATION process_information;

					ZeroMemory(&process_information, sizeof(process_information));

					processes.push_back(process_information);

					auto command_line = (process_name + " " + boost::uuids::to_string(system.first));

					if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()), 
						NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &processes.back()))
					{
						throw teacher_exception("CreateProcessA error");
					}
				}

				for (auto & process : processes)
				{
					WaitForSingleObject(process.hProcess, INFINITE);

					CloseHandle(process.hProcess);
					CloseHandle(process.hThread);
				}

				extract_generation_deviations();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::extract_generation_deviations()
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & system : m_systems)
				{
					system.second = *((m_shared_memory.find < double > (
						boost::uuids::to_string(system.first).c_str())).first);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::evaluate_systems(std::size_t generation_index) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < double > deviations;

				deviations.reserve(m_systems.size());

				auto array = json_t::array();

				for (const auto & system : m_systems)
				{
					json_t element;

					deviations.push_back(system.second);

					element[Key::id]		= boost::uuids::to_string(system.first);
					element[Key::deviation] = deviations.back();

					array.push_back(element);
				}

				print_generation_statistics(generation_index, deviations);

				std::stringstream sout;

				sout << array;

				send_evaluation_data(sout.str());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::print_generation_statistics(std::size_t generation_index, const std::vector < double > & deviations) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto min_max = std::minmax_element(deviations.begin(), deviations.end());

				auto avg = std::accumulate(deviations.begin(), deviations.end(), 0.0) / deviations.size();

				std::cout << "Generation: " << generation_index << " of " << m_n_generations << std::endl << std::endl;

				std::cout << "Minimum deviation: " << std::setprecision(3) << std::fixed << *min_max.first << std::endl;
				std::cout << "Average deviation: " << std::setprecision(3) << std::fixed << avg << std::endl;
				std::cout << "Maximum deviation: " << std::setprecision(3) << std::fixed << *min_max.second << std::endl;

				std::cout << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::send_evaluation_data(const std::string & data) const
		{
			RUN_LOGGER(logger);

			try
			{
				m_module_g(data.c_str());
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, shared::Python::exception());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void save_system_deviation(const System & system)
		{
			RUN_LOGGER(logger);

			try
			{
				Teacher::shared_memory_t shared_memory(
					boost::interprocess::open_only, Teacher::shared_memory_name.c_str());

				*((shared_memory.find < double > (
					boost::uuids::to_string(system.id()).c_str())).first) = system.current_total_deviation();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution