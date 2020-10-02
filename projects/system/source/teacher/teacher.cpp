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
				boost::python::exec("from script import f", m_python.global(), m_python.global());
				boost::python::exec("from script import g", m_python.global(), m_python.global());

				m_module_f = m_python.global()["f"];
				m_module_g = m_python.global()["g"];

				m_systems.reserve(n_systems);

				for (auto i = 0U; i < n_systems; ++i)
				{
					m_systems.emplace_back(System::random_generator(), m_python, m_module_f);
				}				
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, m_python.exception());
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
						// shared::Timer timer("GENERATION", std::cout);

						run_systems();

						evaluate_systems(i);

						reinitialize_systems();
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

		void Teacher::run_systems() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & system : m_systems)
				{
					system.run();
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

					deviations.push_back(system.current_total_deviation());

					element[Key::id]		= boost::uuids::to_string(system.id());
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
				logger.write(Severity::error, m_python.exception());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

		void Teacher::reinitialize_systems()
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & system : m_systems)
				{
					system.reinitialize();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < teacher_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution