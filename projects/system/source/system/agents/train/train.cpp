#include "train.hpp"

namespace solution
{
	namespace system
	{
		namespace agents
		{
			void Train::initialize()
			{
				RUN_LOGGER(logger);

				try
				{
					// ...
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			id_t Train::current_segment_id() const
			{
				RUN_LOGGER(logger);

				try
				{
					assert(m_route);

					return m_route->points().at(m_current_point_index).segment_id;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			id_t Train::next_segment_id() const
			{
				RUN_LOGGER(logger);

				try
				{
					assert(m_route);

					if (!has_completed_move())
					{
						return m_route->points().at(m_current_point_index + 1U).segment_id;
					}
					else
					{
						return generate_null_id();
					}					
				}
				catch (const std::exception& exception)
				{
					shared::catch_handler < train_exception >(logger, exception);
				}
			}

			void Train::set_route(std::shared_ptr < Route > route)
			{
				RUN_LOGGER(logger);

				try
				{
					assert(route);

					if (route->id() == m_route_id)
					{
						m_route = route;
					}
					else
					{
						throw std::invalid_argument("invalid route");
					}

					m_gid.push_back(Point(m_route->points().at(m_current_point_index).segment_id, 
						m_route->start_time(), 0LL));
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::is_ready() const
			{
				RUN_LOGGER(logger);

				try
				{
					assert(m_route);

					return (m_current_staying_time >= m_route->points().at(m_current_point_index).staying);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::stay()
			{
				RUN_LOGGER(logger);

				try
				{
					assert(m_route);

					++m_current_staying_time;

					++m_total_movement_time;

					++m_gid.back().staying;

					if (m_current_staying_time > m_route->points().at(m_current_point_index).staying)
					{
						m_deviation += m_route->weight_k() * 1.0 *
							(m_current_staying_time > 60LL ? 4.0 : 1.0); // TODO
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			void Train::move()
			{
				RUN_LOGGER(logger);

				try
				{
					if (!has_completed_move())
					{
						++m_current_point_index;

						m_current_staying_time = 0LL;

						m_gid.push_back(Point(m_route->points().at(m_current_point_index).segment_id, 
							m_gid.front().arrival + m_total_movement_time, 0LL));
					}
					else
					{
						throw std::runtime_error("bad move");
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::has_completed_move() const
			{
				RUN_LOGGER(logger);

				try
				{
					assert(m_route);

					return (m_current_point_index + 1 == std::size(m_route->points()));
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			bool Train::has_completed_route() const
			{
				RUN_LOGGER(logger);

				try
				{
					return (has_completed_move() && is_ready());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

		} // namespace agents

	} // namespace system

} // namespace solution