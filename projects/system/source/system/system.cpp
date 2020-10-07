#include "system.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		void System::Data::load(segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_segments;
				
				load(File::segments_data, raw_segments);

				for (const auto & raw_segment : raw_segments)
				{
					auto id                    = raw_segment[Key::Segment::id               ].get < std::string > ();
					auto type                  = raw_segment[Key::Segment::type             ].get < std::size_t > ();
					auto name                  = raw_segment[Key::Segment::name             ].get < std::string > ();
					auto station			   = raw_segment[Key::Segment::station			].get < std::string > ();
					auto length                = raw_segment[Key::Segment::length           ].get < std::size_t > ();
					auto raw_adjacent_segments = raw_segment[Key::Segment::adjacent_segments].get < std::vector < std::string > > ();

					Segment::segments_container_t adjacent_segments(raw_adjacent_segments.size());

					std::transform(raw_adjacent_segments.begin(), raw_adjacent_segments.end(), adjacent_segments.begin(),
						[](const auto & raw_adjacent_segment)
					{
						return Segment::string_generator(raw_adjacent_segment);
					});

					auto segment = std::make_shared < Segment > (
						std::move(Segment::string_generator(std::move(id))), static_cast < Segment::Type > (type), 
						std::move(name), std::move(station), length, std::move(adjacent_segments));

					segments[segment->id()] = segment;

					// segments.push_back(segment);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::load(trains_container_t & trains)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_trains;

				load(File::trains_data, raw_trains);

				for (const auto & raw_train : raw_trains)
				{
					auto id					 = raw_train[Key::Train::id				    ].get < std::string > ();
					auto name				 = raw_train[Key::Train::name				].get < std::string > ();
					auto code				 = raw_train[Key::Train::code				].get < std::string > ();
					auto type				 = raw_train[Key::Train::type				].get < std::string > ();
					auto weight_k			 = raw_train[Key::Train::weight_k			].get < double > ();
					auto route_id			 = raw_train[Key::Train::route_id			].get < std::string > ();
					auto speed				 = raw_train[Key::Train::speed			    ].get < std::size_t > ();
					auto length				 = raw_train[Key::Train::length			    ].get < std::size_t > ();
					auto current_segment_id  = raw_train[Key::Train::current_segment_id ].get < std::string > ();
					auto previous_segment_id = raw_train[Key::Train::previous_segment_id].get < std::string > ();

					auto train = std::make_shared < Train > (
						std::move(Train::string_generator(std::move(id))), std::move(name), std::move(code), std::move(type), weight_k, 
						std::move(Train::string_generator(std::move(route_id))), speed, length, 
						std::move(Train::string_generator(std::move(current_segment_id))),
						std::move(Train::string_generator(std::move(previous_segment_id))));

					trains[train->id()] = train;

					// trains.push_back(train);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::load(routes_container_t & routes)
		{
			RUN_LOGGER(logger);

			try
			{
				json_t raw_routes;

				load(File::routes_data, raw_routes);

				for (const auto & raw_route : raw_routes)
				{
					auto id          = raw_route[Key::Route::id].get < std::string > ();
					auto raw_records = raw_route[Key::Route::records];

					Route::records_container_t records(raw_records.size());

					std::transform(raw_records.begin(), raw_records.end(), records.begin(),
						[](const auto & raw_record)
					{
						return Route::Record(
							raw_record[Key::Route::station  ].get < std::string > (),
							raw_record[Key::Route::arrival  ].get < std::time_t > (),
							raw_record[Key::Route::departure].get < std::time_t > ());
					});

					auto route = std::make_shared < Route > (std::move(Route::string_generator(std::move(id))), std::move(records));

					routes[route->id()] = route;

					// routes.push_back(route);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save(const trains_container_t & trains, const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				auto raw_trains = json_t::array();

				for (const auto & train : trains)
				{
					json_t raw_train;

					raw_train[Key::Train::name] = train.second->name();

					json_t raw_thread;

					for (const auto & record : train.second->thread())
					{
						json_t raw_record;

						raw_record[Key::Segment::name] = segments.at(record.first)->name();
						raw_record[Key::Segment::time] = record.second;

						raw_thread.push_back(raw_record);
					}

					raw_train[Key::Train::thread] = raw_thread;

					raw_trains.push_back(raw_train);
				}

				save(File::gid_data, raw_trains);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save_segments_order(const segments_container_t & segments)
		{
			RUN_LOGGER(logger);

			try
			{
				path_t path = File::segments_order_txt;

				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw system_exception("cannot open file " + path.string());
				}

				for (const auto & segment : segments)
				{
					fout << boost::uuids::to_string(segment.second->id()) << std::endl;
				}
			}
			catch (const std::exception& exception)
			{
				shared::catch_handler < system_exception >(logger, exception);
			}
		}

		void System::Data::load(const path_t & path, json_t & object)
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
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::Data::save(const path_t & path, const json_t & object)
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
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				load();

				prepare_trains();
				prepare_segments();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::load()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load(m_segments);
				Data::load(m_trains);
				Data::load(m_routes);

				// Data::save_segments_order(m_segments); // debug
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::prepare_trains()
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & train : m_trains)
				{
					train.second->set_route(*m_routes.at(train.second->route_id()));
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::prepare_segments()
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & train : m_trains)
				{
					m_segments.at(train.second->current_segment_id())->train_arrived(train.second->id());
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::reinitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				clear();

				initialize();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::clear()
		{
			RUN_LOGGER(logger);

			try
			{
				m_segments.clear();
				m_trains.clear();
				m_routes.clear();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run() const
		{
			RUN_LOGGER(logger);

			try
			{
				boost::python::exec("from script import f", m_python.global(), m_python.global());
				boost::python::exec("from script import h", m_python.global(), m_python.global());

				boost::python::object module_f = m_python.global()["f"];
				boost::python::object module_h = m_python.global()["h"];

				module_h(make_initialization_data().c_str(), 0);

				for (std::time_t t = 0; t < limit_time && has_train_on_route(); ++t)
				{
					if (has_ready_train_on_route())
					{
						auto v_in = make_input_vector();

						std::string result = boost::python::extract < std::string > (
							module_f(to_string(v_in).c_str(), boost::uuids::to_string(m_id).c_str()));

						auto v_out = from_string(result);

						apply_output_vector(v_out);
					}

					continue_action();
				}
			}
			catch (const boost::python::error_already_set &)
			{
				logger.write(Severity::error, shared::Python::exception());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::stop() const
		{
			RUN_LOGGER(logger);

			try
			{
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::save() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save(m_trains, m_segments);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		std::string System::make_initialization_data() const
		{
			RUN_LOGGER(logger);

			try
			{
				auto array = json_t::array();

				array.push_back(boost::uuids::to_string(m_id));

				std::stringstream sout;

				sout << array;

				return sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::has_train_on_route() const
		{
			RUN_LOGGER(logger);

			try
			{
				return (std::find_if(m_trains.begin(), m_trains.end(), [](const auto & train)
				{
					return !(train.second->route()->empty());
				}) != m_trains.end());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::has_ready_train_on_route() const
		{
			RUN_LOGGER(logger);

			try
			{
				return (std::find_if(m_trains.begin(), m_trains.end(), [](const auto & train)
				{
					return (
						train.second->state() == Train::State::stop_move || 
						train.second->state() == Train::State::stop_wait);
				}) != m_trains.end());
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception >(logger, exception);
			}
		}

		System::v_in_t System::make_input_vector() const
		{
			RUN_LOGGER(logger);

			try
			{
				v_in_t v_in;

				const auto segment_data_size = 2U + 2U * m_routes.begin()->second->records().size();

				v_in.reserve(m_segments.size() * segment_data_size);

				for (const auto & segment : m_segments)
				{
					auto train_id = segment.second->train_id();

					if (train_id.is_nil())
					{
						for (auto i = 0U; i < segment_data_size; ++i)
						{
							v_in.push_back(0);
						}
					}
					else
					{
						v_in.push_back(static_cast < v_in_element_t > (m_trains.at(train_id)->state()));
						v_in.push_back(static_cast < v_in_element_t > (segment.second->state()));

						for (const auto & record : m_trains.at(train_id)->route()->records())
						{
							v_in.push_back(static_cast < v_in_element_t > (record.arrival));
							v_in.push_back(static_cast < v_in_element_t > (record.departure));
						}
					}
				}

				return v_in;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::print_input_vector(const v_in_t & v_in, bool print_full) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto segment_data_size = 2U + 2U * m_routes.begin()->second->records().size();

				for (auto i = 0U; i < m_segments.size(); ++i)
				{
					if (std::next(m_segments.begin(), i)->second->has_train() || print_full)
					{
						for (auto j = 0U; j < segment_data_size; ++j)
						{
							if (j < 2U)
							{
								std::cout << v_in.at(i * segment_data_size + j) << " | ";
							}
							else
							{
								std::cout << std::setw(4) << std::right << v_in.at(i * segment_data_size + j) << "| ";
							}
						}

						std::cout << std::endl;
					}
				}

				std::cout << std::endl;

				// std::cout << "v_in size: " << v_in.size() << std::endl << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		std::string System::to_string(const v_in_t & v_in) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::stringstream sout;

				for (auto element : v_in)
				{
					sout << element << ' ';
				}

				return sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		System::v_out_t System::from_string(const std::string & string) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::size_t total_adjacent_segments_size = 0U;

				for (const auto & segment : m_segments)
				{
					total_adjacent_segments_size += segment.second->adjacent_segments().size();
				}

				v_out_t v_out(total_adjacent_segments_size + m_segments.size(), 0);

				std::stringstream sin(string);

				for (auto & element : v_out)
				{
					sin >> element;
				}

				return v_out;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		System::v_out_t System::make_random_output_vector() const
		{
			RUN_LOGGER(logger);

			try
			{
				std::size_t total_adjacent_segments_size = 0U;

				for (const auto & segment : m_segments)
				{
					total_adjacent_segments_size += segment.second->adjacent_segments().size();
				}

				v_out_t v_out(total_adjacent_segments_size + m_segments.size(), 0);

				// get v_out from model

				std::uniform_int_distribution < v_out_element_t > distribution_command(0, 1);

				std::size_t index = 0U;

				for (const auto & segment : m_segments)
				{
					const auto group_size = segment.second->adjacent_segments().size();

					v_out[index] = distribution_command(m_generator);

					if (v_out[index] == 1)
					{
						std::uniform_int_distribution < std::size_t > distribution_segment(0U, group_size - 1U);

						v_out[index + 1U + distribution_segment(m_generator)] = 1;
					}

					index += (group_size + 1);
				}

				return v_out;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::print_output_vector(const v_out_t & v_out, bool print_full) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::size_t index = 0U;

				for (const auto & segment : m_segments)
				{
					const auto group_size = segment.second->adjacent_segments().size() + 1;

					if (segment.second->has_train() || print_full)
					{
						for (auto j = 0U; j < group_size; ++j, ++index)
						{
							std::cout << std::setw(1) /*<< std::boolalpha*/ << v_out[index] << ' ';
						}

						std::cout << std::endl;
					}
					else
					{
						index += group_size;
					}
				}

				std::cout << std::endl;

				// std::cout << "v_out size: " << v_out.size() << std::endl << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::apply_output_vector(const v_out_t & v_out) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::size_t index = 0U;

				for (const auto & segment : m_segments)
				{
					const auto group_size = segment.second->adjacent_segments().size() + 1;

					if (can_execute_command(segment.first))
					{
						v_out_t command(group_size, 0);

						for (auto j = 0U; j < group_size; ++j, ++index)
						{
							command[j] = v_out[index];
						}

						execute_command(segment.first, command);
					}
					else
					{
						index += group_size;
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::can_execute_command(const Segment::id_t & id) const
		{
			RUN_LOGGER(logger);

			try
			{
				return (m_segments.at(id)->has_train() &&
					((m_trains.at(m_segments.at(id)->train_id())->state() == Train::State::stop_move) ||
					 (m_trains.at(m_segments.at(id)->train_id())->state() == Train::State::stop_wait)));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::execute_command(const Segment::id_t & id, const v_out_t & command) const
		{
			RUN_LOGGER(logger);

			try
			{
				assert(command.size() >= 2);

				Train::State command_type = static_cast < Train::State > (command.at(0));

				const auto bias = 1U;

				switch (command_type)
				{
				case Train::State::wait:
				{
					m_trains.at(m_segments.at(id)->train_id())->update_state(Train::State::stop_wait);

					break;
				}
				case Train::State::move:
				{
					auto iterator = std::find(std::next(command.begin(), bias), command.end(), 1);

					if (iterator == command.end())
					{
						m_trains.at(m_segments.at(id)->train_id())->update_state(Train::State::stop_wait);
					}
					else
					{
						const auto adjacent_segment_index = static_cast <std::size_t> (
							std::distance(std::next(command.begin(), bias), iterator));

						const auto next_segment_id = m_segments.at(id)->adjacent_segments().at(adjacent_segment_index);

						if (can_goto_segment(id, next_segment_id))
						{
							goto_segment(id, next_segment_id);
						}
						else
						{
							m_trains.at(m_segments.at(id)->train_id())->update_state(Train::State::stop_wait);
						}
					}

					break;
				}
				default:
				{
					throw std::logic_error("unknown command type: " + std::to_string(command.at(0)));

					break;
				}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		bool System::can_goto_segment(const Segment::id_t & current_segment_id, const Segment::id_t & next_segment_id) const
		{
			RUN_LOGGER(logger);

			try
			{
				bool is_next_segment_available_to_move = m_segments.at(next_segment_id)->is_available_to_move();
				bool are_different_previous_and_next_segment_ids = 
					(m_trains.at(m_segments.at(current_segment_id)->train_id())->previous_segment_id() != next_segment_id);
				bool is_train_previous_segment_id_nil = m_trains.at(m_segments.at(current_segment_id)->train_id())->previous_segment_id().is_nil();
				bool is_next_segment_railway = (m_segments.at(next_segment_id)->type() == Segment::Type::railway);

				return (is_next_segment_available_to_move && are_different_previous_and_next_segment_ids && 
					(is_train_previous_segment_id_nil || (is_next_segment_railway || (m_segments.at(next_segment_id)->station() !=
						m_segments.at(m_trains.at(m_segments.at(current_segment_id)->train_id())->previous_segment_id())->station()))));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::goto_segment(const Segment::id_t & current_segment_id, const Segment::id_t & next_segment_id) const
		{
			RUN_LOGGER(logger);

			try
			{
				m_segments.at(next_segment_id)->train_arrived(m_segments.at(current_segment_id)->train_id());

				m_segments.at(current_segment_id)->train_departured();

				m_trains.at(m_segments.at(next_segment_id)->train_id())->update_current_segment_id(next_segment_id);

				switch (m_segments.at(next_segment_id)->type())
				{
				case Segment::Type::railway:
				{
					m_trains.at(m_segments.at(next_segment_id)->train_id())->update_state(Train::State::move);

					break;
				}
				case Segment::Type::station:
				{
					m_trains.at(m_segments.at(next_segment_id)->train_id())->update_state(Train::State::wait);

					m_trains.at(m_segments.at(next_segment_id)->train_id())->update_deviation(m_segments.at(next_segment_id)->station());

					break;
				}
				default:
				{
					throw std::logic_error("unknown segment type: " + std::to_string(static_cast < int > (m_segments.at(next_segment_id)->type())));

					break;
				}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::continue_action() const
		{
			RUN_LOGGER(logger);

			try
			{
				const std::time_t delta = 1;

				for (auto & train : m_trains)
				{
					switch (train.second->state())
					{
					case Train::State::move:
					{
						train.second->continue_movement(m_segments.at(train.second->current_segment_id())->length());

						train.second->reduce_route_time(delta);

						break;
					}
					case Train::State::wait:
					{
						train.second->reduce_route_time(delta, m_segments.at(train.second->current_segment_id())->station());
						
						break;
					}
					case Train::State::stop_move: // impossible here
					case Train::State::stop_wait:
					{
						train.second->reduce_route_time(delta);

						break;
					}
					default:
					{
						break;
					}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::print_current_deviations() const
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & train : m_trains)
				{
					std::cout << 
						std::setw(5) << std::right << train.second->name() << " deviation = " <<
						std::setw(4) << std::right << train.second->deviation() << " (min)\n";
				}

				std::cout << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		double System::current_total_deviation() const
		{
			RUN_LOGGER(logger);

			try
			{
				double deviation = 0.0;

				for (auto & train : m_trains)
				{
					deviation += (train.second->current_total_deviation() * train.second->weight_k());
				}

				return deviation;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::print_result(std::time_t elapsed_time) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << std::endl << "Result (t = " << elapsed_time << ") :" << std::endl << std::endl;

				std::cout << std::boolalpha << "Has train on route: " << has_train_on_route() << std::endl << std::endl; // debug

				auto v_in = make_input_vector();

				print_input_vector(v_in);

				std::cout << "Deviation: " << std::setprecision(3) << std::fixed << current_total_deviation() << std::endl << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		boost::uuids::random_generator System::random_generator;
		boost::uuids::string_generator System::string_generator;

	} // namespace system

} // namespace solution