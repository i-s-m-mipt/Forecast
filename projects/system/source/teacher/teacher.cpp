#include "teacher.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		void Teacher::Data::load(systems_container_t & systems, std::size_t n_systems)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_systems;

				load(File::systems_data, raw_systems);

				std::size_t index = 0U;

				if (!raw_systems.empty())
				{
					for (; index < raw_systems.size() && index < n_systems; ++index)
					{
						systems[System::string_generator(raw_systems.at(index).get < std::string > ())] = 0.0;
					}
				}

				for (; index < n_systems; ++index)
				{
					systems[System::random_generator()] = 0.0;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::Data::save(const systems_container_t & systems)
		{
			RUN_LOGGER(logger);

			try
			{
				auto raw_systems = json_t::array();

				for (const auto & system : systems)
				{
					raw_systems.push_back(boost::uuids::to_string(system.first));
				}

				save(File::systems_data, raw_systems);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::Data::load(const path_t & path, json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw system_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw system_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
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
				boost::python::exec("from script import h", m_python.global(), m_python.global());

				m_module_h = m_python.global()["h"];

				Data::load(m_systems, n_systems);

				m_module_h(make_initialization_data().c_str(), 1);

				Data::save(m_systems);

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

				shared::catch_handler < teacher_exception > (logger);
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

		std::string Teacher::make_initialization_data() const
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

				return sout.str();
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

						print_generation_statistics(i);

						evaluate_systems();
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

		void Teacher::print_generation_statistics(std::size_t generation_index) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto min_max = std::minmax_element(m_systems.begin(), m_systems.end(),
					[](const auto & lhs, const auto & rhs) { return lhs.second < rhs.second; });

				auto avg = std::transform_reduce(m_systems.begin(), m_systems.end(), 0.0,
					std::plus < double > (), [](const auto & system) { return system.second; }) / m_systems.size();

				std::cout << "Generation: " << generation_index << " of " << m_n_generations << std::endl << std::endl;

				std::cout << "Minimum deviation: " << std::setprecision(3) << std::fixed << min_max.first->second  << std::endl;
				std::cout << "Average deviation: " << std::setprecision(3) << std::fixed << avg					   << std::endl;
				std::cout << "Maximum deviation: " << std::setprecision(3) << std::fixed << min_max.second->second << std::endl;

				std::cout << std::endl;

				std::cout << "Best fitness: " << boost::uuids::to_string(min_max.first->first) << std::endl;

				std::cout << std::endl;
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

		void Teacher::evaluate_systems() const
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < system_descriptor_t > systems(m_systems.begin(), m_systems.end());

				std::sort(systems.begin(), systems.end(),
					[](const auto & lhs, const auto & rhs) { return lhs.second < rhs.second; });

				const auto group_size = 8U;

				std::vector top_systems(systems.begin(), std::next(systems.begin(), group_size));

				systems.erase(systems.begin(), std::next(systems.begin(), group_size));

				const auto seed = static_cast < unsigned int > (
					std::chrono::system_clock::now().time_since_epoch().count());

				std::shuffle(systems.begin(), systems.end(), std::default_random_engine(seed));

				std::vector random_systems(systems.begin(), std::next(systems.begin(), group_size));

				systems.erase(systems.begin(), std::next(systems.begin(), group_size));

				std::vector < std::string > files(systems.size());

				std::transform(systems.begin(), systems.end(), files.begin(),
					[](const auto & system) { return boost::uuids::to_string(system.first); });

				run_genetic_algorithm(std::move(top_systems), std::move(random_systems), std::move(files));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_genetic_algorithm(
			std::vector < system_descriptor_t > && top_systems,
			std::vector < system_descriptor_t > && random_systems, std::vector < std::string > && files) const
		{
			RUN_LOGGER(logger);

			try
			{
				STARTUPINFOA startup_information;

				ZeroMemory(&startup_information, sizeof(startup_information));

				startup_information.cb = sizeof(startup_information);

				std::vector < PROCESS_INFORMATION > processes;

				processes.reserve(top_systems.size() * 4U);

				run_copy_mutation(top_systems,    files, startup_information, processes);
				run_copy_mutation(random_systems, files, startup_information, processes);

				run_crossover(top_systems,    files, startup_information, processes);
				run_crossover(random_systems, files, startup_information, processes);

				top_systems.insert(top_systems.end(), random_systems.begin(), random_systems.end());

				run_crossover(top_systems, files, startup_information, processes);

				for (auto & process : processes)
				{
					WaitForSingleObject(process.hProcess, INFINITE);

					CloseHandle(process.hProcess);
					CloseHandle(process.hThread);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_copy_mutation(const std::vector < system_descriptor_t > & systems, std::vector < std::string > & files,
			STARTUPINFOA & startup_information, std::vector < PROCESS_INFORMATION > & processes) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & system : systems)
				{
					PROCESS_INFORMATION process_information;

					ZeroMemory(&process_information, sizeof(process_information));

					processes.push_back(process_information);

					auto command_line = (process_name + " -function copy_mutation " +
						boost::uuids::to_string(system.first) + " " + files.back());

					files.pop_back();

					if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
						NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &processes.back()))
					{
						throw teacher_exception("CreateProcessA error");
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_crossover(std::vector < system_descriptor_t > systems, std::vector < std::string > & files,
			STARTUPINFOA & startup_information, std::vector < PROCESS_INFORMATION > & processes) const
		{
			RUN_LOGGER(logger);

			try
			{
				if (systems.size() % 2 != 0)
				{
					throw std::logic_error("invalid systems quantity for crossover");
				}

				const auto seed = static_cast < unsigned int > (
					std::chrono::system_clock::now().time_since_epoch().count());

				std::shuffle(systems.begin(), systems.end(), std::default_random_engine(seed));

				for (auto i = 0U; i < systems.size(); i += 2U)
				{
					PROCESS_INFORMATION process_information;

					ZeroMemory(&process_information, sizeof(process_information));

					processes.push_back(process_information);

					auto command_line = (process_name + " -function crossover " +
						boost::uuids::to_string(systems[i + 0].first) + " " +
						boost::uuids::to_string(systems[i + 1].first) + " " +
						files[files.size() - 1] + " " + files[files.size() - 2]);

					files.pop_back();
					files.pop_back();

					if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
						NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &processes.back()))
					{
						throw teacher_exception("CreateProcessA error");
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::save_deviation(const System & system)
		{
			RUN_LOGGER(logger);

			try
			{
				shared_memory_t shared_memory(boost::interprocess::open_only, shared_memory_name.c_str());

				*((shared_memory.find < double > (
					boost::uuids::to_string(system.id()).c_str())).first) = system.current_total_deviation();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void apply_genetic_algorithm(const std::string & function_name, 
			const std::string & model_id_1, const std::string & file_1,
			const std::string & model_id_2, const std::string & file_2)
		{
			RUN_LOGGER(logger);

			try
			{
				shared::Python python;

				boost::python::exec(("from script import " + function_name).c_str(), 
					python.global(), python.global());

				boost::python::object function = python.global()[function_name.c_str()];

				if (model_id_2.empty())
				{
					function(model_id_1.c_str(), file_1.c_str());
				}
				else
				{
					function(model_id_1.c_str(), model_id_2.c_str(), file_1.c_str(), file_2.c_str());
				}
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, shared::Python::exception());

				shared::catch_handler < teacher_exception > (logger);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution