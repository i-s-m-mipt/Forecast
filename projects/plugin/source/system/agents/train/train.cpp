#include "train.hpp"

namespace solution
{
	namespace plugin
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

			// Проверка, что поезд уже достаточно отстоял на станционном пути
			// или достаточно времени провел на перегоне при движении и готов
			// переместиться на следующий сегмент своего маршрута
			bool Train::is_ready(std::time_t standard_time) const
			{
				RUN_LOGGER(logger);

				try
				{
					// В этой функции можно контролировать скорость движения
					// поездов, например, через коэффициент уменьшить значение
					// standard_time при нахождении на перегоне, это будет означать
					// что поезд едет с увеличенной скоростью и будет ready раньше
					return (m_segment_time >= standard_time);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			// Проверка что поезд доехал до последнего сегмента, но еще
			// возможно должен постоять на нем в соответствии с планом, т.е.
			// ехать он уже не будет, но еще должен постоять, прежде чем исчезнет
			bool Train::has_completed_movement() const
			{
				RUN_LOGGER(logger);

				try
				{
					// Флаг at end выставляется извне и решает проблему с ситуациями,
					// когда поезд финиширует не на своем станционном пути по причине
					// занятости (внезапной), это способ избежать дедлока
					return ((m_segment == end) || at_end);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			// Проверка, что поезд доехал до последнего сегмента и уже простоял на нем
			// нужное количество времени, и теперь может быть исключен из рассмотрения
			bool Train::has_completed_route(std::time_t standard_time) const
			{
				RUN_LOGGER(logger);

				try
				{
					return (has_completed_movement() && is_ready(standard_time));
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);

					return false;
				}
			}

			// Продолжать делать предыдущее действие - двигаться или стоять
			void Train::stay(std::time_t standard_time)
			{
				RUN_LOGGER(logger);

				try
				{
					++m_segment_time; // время пребывания на сегменте увеличивается на 1 (минуту)

					if (m_segment_time > standard_time) // если пребывание больше планового
					{
						m_deviation += priority * (m_segment_time - standard_time); // увеличиваем отклонение
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

			// После перемещения поезда на новый сегмент время пребывания обнуляется
			void Train::move(const std::string & segment)
			{
				RUN_LOGGER(logger);

				try
				{
					m_segment = segment;

					m_segment_time = 0LL;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < train_exception > (logger, exception);
				}
			}

		} // namespace agents

	} // namespace plugin

} // namespace solution