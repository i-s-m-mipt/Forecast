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

				m_module_h(make_initialization_data().c_str(), 0);

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
				static double best_result = 0.0;

				static std::size_t counter = 0U;

				std::vector < system_descriptor_t > systems(m_systems.begin(), m_systems.end());

				std::sort(systems.begin(), systems.end(),
					[](const auto & lhs, const auto & rhs) { return lhs.second < rhs.second; });

				const auto group_size = 8U;

				std::vector top_systems(systems.begin(), std::next(systems.begin(), 2U * group_size));

				if (best_result != top_systems.front().second)
				{
					best_result = top_systems.front().second;

					counter = 0U;
				}
				else
				{
					++counter;
				}

				systems.erase(systems.begin(), std::next(systems.begin(), 2U * group_size));

				if (counter > 16)
				{
					const auto seed = static_cast < unsigned int > (
						std::chrono::system_clock::now().time_since_epoch().count());

					std::shuffle(systems.begin(), systems.end(), std::default_random_engine(seed));
				}

				top_systems.insert(top_systems.end(), systems.begin(), std::next(systems.begin(), 2U * group_size));

				systems.erase(systems.begin(), std::next(systems.begin(), 2U * group_size));

				std::sort(top_systems.begin(), top_systems.end(),
					[](const auto & lhs, const auto & rhs) { return lhs.second < rhs.second; });

				for (const auto & system : top_systems)
				{
					std::cout << std::setw(9) << std::right << system.second << " " << 
						boost::uuids::to_string(system.first) << std::endl;
				}

				std::cout << std::endl;

				if (counter > 16)
				{
					const auto seed = static_cast <unsigned int> (
						std::chrono::system_clock::now().time_since_epoch().count());

					std::shuffle(top_systems.begin(), top_systems.end(), std::default_random_engine(seed));
				}

				std::vector < system_descriptor_t > random_systems;

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

				processes.reserve(m_systems.size());

				std::vector < system_descriptor_t > systems_1(top_systems.begin() + 0, top_systems.begin() + 4);
				std::vector < system_descriptor_t > systems_2(top_systems.begin() + 4, top_systems.begin() + 8);
				std::vector < system_descriptor_t > systems_3(top_systems.begin() + 8, top_systems.begin() + 12);
				std::vector < system_descriptor_t > systems_4(top_systems.begin() + 12, top_systems.begin() + 16);
				std::vector < system_descriptor_t > systems_5(top_systems.begin() + 16, top_systems.begin() + 20);
				std::vector < system_descriptor_t > systems_6(top_systems.begin() + 20, top_systems.begin() + 24);
				std::vector < system_descriptor_t > systems_7(top_systems.begin() + 24, top_systems.begin() + 28);
				std::vector < system_descriptor_t > systems_8(top_systems.begin() + 28, top_systems.begin() + 32);

				const auto seed = static_cast <unsigned int> (
					std::chrono::system_clock::now().time_since_epoch().count());

				std::shuffle(systems_1.begin(), systems_1.end(), std::default_random_engine(seed));
				std::shuffle(systems_2.begin(), systems_2.end(), std::default_random_engine(seed));
				std::shuffle(systems_3.begin(), systems_3.end(), std::default_random_engine(seed));
				std::shuffle(systems_4.begin(), systems_4.end(), std::default_random_engine(seed));

				std::vector < std::string > files_1(files.begin() + 0, files.begin() + 4);
				std::vector < std::string > files_2(files.begin() + 4, files.begin() + 8);
				std::vector < std::string > files_3(files.begin() + 8, files.begin() + 12);
				std::vector < std::string > files_4(files.begin() + 12, files.begin() + 16);
				std::vector < std::string > files_5(files.begin() + 16, files.begin() + 20);
				std::vector < std::string > files_6(files.begin() + 20, files.begin() + 24);
				std::vector < std::string > files_7(files.begin() + 24, files.begin() + 28);
				std::vector < std::string > files_8(files.begin() + 28, files.begin() + 32);

				run_crossover(systems_1, files_1, startup_information, processes, 1);
				run_crossover(systems_2, files_2, startup_information, processes, 2);
				run_crossover(systems_3, files_3, startup_information, processes, 3);
				run_crossover(systems_4, files_4, startup_information, processes, 4);
				run_crossover(systems_5, files_5, startup_information, processes, 5);
				run_crossover(systems_6, files_6, startup_information, processes, 6);
				run_crossover(systems_7, files_7, startup_information, processes, 7);
				run_crossover(systems_8, files_8, startup_information, processes, 8);

				// run_copy_mutation(top_systems,    files, startup_information, processes);
				// run_copy_mutation(random_systems, files, startup_information, processes);

				//random_systems.insert(random_systems.end(), top_systems.begin(), top_systems.end());

				//run_crossover_mutation(random_systems, files, startup_information, processes);

				for (auto & process : processes)
				{
					WaitForSingleObject(process.hProcess, INFINITE);

					CloseHandle(process.hProcess);
					CloseHandle(process.hThread);
				}

				make_genetic_variety(std::move(top_systems));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_copy_mutation(const system_descriptor_t & system, const std::vector < std::string > & files,
			STARTUPINFOA & startup_information, std::vector < PROCESS_INFORMATION > & processes) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto i = 0U; i < files.size(); ++i)
				{
					PROCESS_INFORMATION process_information;

					ZeroMemory(&process_information, sizeof(process_information));

					processes.push_back(process_information);

					auto command_line = (process_name + " -function copy_mutation " +
						boost::uuids::to_string(system.first) + " " + files[i]);

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
			catch (const std::exception& exception)
			{
				shared::catch_handler < teacher_exception >(logger, exception);
			}
		}

		void Teacher::run_crossover(std::vector < system_descriptor_t > systems, std::vector < std::string > & files,
			STARTUPINFOA & startup_information, std::vector < PROCESS_INFORMATION > & processes, int i) const
		{
			RUN_LOGGER(logger);

			try
			{
				//if (systems.size() % 2 != 0)
				//{
				//	throw std::logic_error("invalid systems quantity for crossover");
				//}

				for (const auto & system : systems)
				{
					std::cout << std::setw(9) << std::right << system.second << std::endl;
				}

				std::cout << std::endl;

				json_t array_models = json_t::array();
				json_t array_files  = json_t::array();

				for (const auto & system : systems)
				{
					array_models.push_back(boost::uuids::to_string(system.first));
				}

				for (const auto & file : files)
				{
					array_files.push_back(file);
				}

				std::string m = "models" + std::to_string(i);
				std::string f = "files" + std::to_string(i);

				std::fstream fout_models(m, std::ios::out);
				std::fstream fout_files (f,  std::ios::out);

				fout_models << std::setw(4) << array_models;
				fout_files  << std::setw(4) << array_files;

				PROCESS_INFORMATION process_information;

				ZeroMemory(&process_information, sizeof(process_information));

				processes.push_back(process_information);

				auto command_line = process_name + " -function crossover " + std::to_string(i);

				if (!CreateProcessA(NULL, (LPSTR)(command_line.c_str()),
					NULL, NULL, FALSE, 0, NULL, NULL, &startup_information, &processes.back()))
				{
					throw teacher_exception("CreateProcessA error");
				}

				/*
				const auto seed = static_cast < unsigned int > (
					std::chrono::system_clock::now().time_since_epoch().count());

				std::shuffle(systems.begin(), systems.end(), std::default_random_engine(seed));

				for (auto i = 0U; i < systems.size(); i += 2U)
				{
					PROCESS_INFORMATION process_information;

					ZeroMemory(&process_information, sizeof(process_information));

					processes.push_back(process_information);

					std::cout <<
						std::setw(9) << std::right << systems[i + 0].second << " & " <<
						std::setw(9) << std::right << systems[i + 1].second << std::endl;

					auto command_line = (process_name + (std::abs(systems[i + 0].second - systems[i + 1].second) > 5.0 ? 
						" -function crossover " : " -function crossover_mutation ") +
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

				std::cout << std::endl;
				*/
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::run_crossover_mutation(std::vector < system_descriptor_t > systems, std::vector < std::string > & files,
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

					auto command_line = (process_name + " -function crossover_mutation " +
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

		void Teacher::make_genetic_variety(std::vector < system_descriptor_t > && systems) const
		{
			RUN_LOGGER(logger);

			try
			{
				if (systems.size() <= 1)
				{
					return;
				}

				std::sort(systems.begin(), systems.end(),
					[](const auto & lhs, const auto & rhs) { return lhs.second < rhs.second; });

				std::vector < system_descriptor_t > bad_systems;

				bad_systems.reserve(systems.size() - 1);

				std::vector < std::string > files;

				files.reserve(systems.size() - 1);

				for (auto i = 1U; i < systems.size(); ++i)
				{
					if (systems[i].second == systems[i - 1].second)
					{
						bad_systems.push_back(systems[i]);
						files.push_back(boost::uuids::to_string(systems[i].first));
					}
				}

				std::reverse(bad_systems.begin(), bad_systems.end());

				std::cout << "Mutation: " << bad_systems.size() << std::endl << std::endl;

				//if (bad_systems.size() >= 10)
				//{
				//	std::cout << "\a\a\a\a\a" << std::endl;
				//}

				STARTUPINFOA startup_information;

				ZeroMemory(&startup_information, sizeof(startup_information));

				startup_information.cb = sizeof(startup_information);

				std::vector < PROCESS_INFORMATION > processes;

				processes.reserve(bad_systems.size());

				run_copy_mutation(bad_systems, files, startup_information, processes);

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

				if (file_1.empty())
				{
					using json_t = boost::extended::serialization::json;

					json_t array_models = json_t::array();
					json_t array_files = json_t::array();

					std::string m = "models" + model_id_1;
					std::string f = "files"  + model_id_1;

					std::fstream fin_models(m, std::ios::in);
					std::fstream fin_files(f, std::ios::in);

					fin_models >> array_models;
					fin_files >> array_files;

					std::stringstream models, files;

					models << array_models;
					files << array_files;

					function(models.str().c_str(), files.str().c_str());
				}
				else
				{
					function(model_id_1.c_str(), file_1.c_str());
				}

				//if (model_id_2.empty())
				//{
				//	function(model_id_1.c_str(), file_1.c_str());
				//}
				//else
				//{
				//	function(model_id_1.c_str(), model_id_2.c_str(), file_1.c_str(), file_2.c_str());
				//}
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