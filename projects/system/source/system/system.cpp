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
				json_t array;
				
				load(File::segments_data, array);

				for (const auto & element : array)
				{
					auto id                    = element[Key::Segment::id               ].get < std::string > ();
					auto type                  = element[Key::Segment::type             ].get < std::size_t > ();
					auto name                  = element[Key::Segment::name             ].get < std::string > ();
					auto length                = element[Key::Segment::length           ].get < std::size_t > ();
					auto adjacent_segments_raw = element[Key::Segment::adjacent_segments].get < std::vector < std::string > > ();

					Segment::segments_container_t adjacent_segments(adjacent_segments_raw.size());

					std::transform(adjacent_segments_raw.begin(), adjacent_segments_raw.end(), adjacent_segments.begin(),
						[](const auto & adjacent_segment_raw)
					{
						return Segment::string_generator(adjacent_segment_raw);
					});

					auto segment = std::make_shared < Segment > (
						std::move(Segment::string_generator(std::move(id))), static_cast < Segment::Type > (type), 
						std::move(name), length, std::move(adjacent_segments));

					segments.push_back(segment);
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
				json_t array;

				load(File::trains_data, array);

				for (const auto & element : array)
				{
					auto id					 = element[Key::Train::id				  ].get < std::string > ();
					auto name				 = element[Key::Train::name				  ].get < std::string > ();
					auto code				 = element[Key::Train::code				  ].get < std::string > ();
					auto type				 = element[Key::Train::type				  ].get < std::string > ();
					auto weight_k			 = element[Key::Train::weight_k			  ].get < double > ();
					auto route_id			 = element[Key::Train::route_id			  ].get < std::string > ();
					auto speed				 = element[Key::Train::speed			  ].get < std::size_t > ();
					auto length				 = element[Key::Train::length			  ].get < std::size_t > ();
					auto current_segment_id  = element[Key::Train::current_segment_id ].get < std::string > ();
					auto previous_segment_id = element[Key::Train::previous_segment_id].get < std::string > ();

					auto train = std::make_shared < Train > (
						std::move(Train::string_generator(std::move(id))), std::move(name), std::move(code), std::move(type), weight_k, 
						std::move(Train::string_generator(std::move(route_id))), speed, length, 
						std::move(Train::string_generator(std::move(current_segment_id))),
						std::move(Train::string_generator(std::move(previous_segment_id))));

					trains.push_back(train);
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
				json_t array;

				load(File::routes_data, array);

				for (const auto & element : array)
				{
					auto id          = element[Key::Route::id].get < std::string > ();
					auto records_raw = element[Key::Route::records];

					Route::records_container_t records(records_raw.size());

					std::transform(records_raw.begin(), records_raw.end(), records.begin(),
						[](const auto & record_raw)
					{
						return Route::Record(
							record_raw[Key::Route::station  ].get < std::string > (),
							record_raw[Key::Route::arrival  ].get < std::time_t > (),
							record_raw[Key::Route::departure].get < std::time_t > ());
					});

					auto route = std::make_shared < Route > (std::move(Route::string_generator(std::move(id))), std::move(records));

					routes.push_back(route);
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

		void System::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				load();
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
				// ...
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::run()
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << "Run system ? (y/n) ";

				if (getchar() == 'y')
				{
					// ...
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

		void System::stop()
		{
			RUN_LOGGER(logger);

			try
			{
				
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < system_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution