#ifndef SOLUTION_PLUGIN_AGENTS_TRAIN_HPP
#define SOLUTION_PLUGIN_AGENTS_TRAIN_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <cassert>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "../../config/config.hpp"

#include "../../../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace plugin
	{
		namespace agents
		{
			class train_exception : public std::exception
			{
			public:

				explicit train_exception(const std::string & message) noexcept :
					std::exception(message.c_str())
				{}

				explicit train_exception(const char * const message) noexcept :
					std::exception(message)
				{}

				~train_exception() noexcept = default;
			};

			class Train
			{
			public:

				//  оманда, которую агент (поезд) должен исполн€ть на каждом шаге
				// в процессе моделировани€ движени€. ѕодробно эти команды описаны
				// в статье “»—”-3, которую можно запросить у  атерины.
				enum class Command
				{
					stay, // продолжать делать предыдущее действие (stay или wait)
					skip, // проигнорировать (этот) поезд, который завершил нитку
					move, // переместитьс€ на перегон или станционный путь
					wait, // сто€ть на станционном пути по плану или сверх нормы
				};

			public:

				Train() = default;

				template < typename T, typename P, typename Enable =
					std::enable_if_t <
						std::is_convertible_v < T, std::string > && 
						std::is_convertible_v < P, std::string > > >
				explicit Train(std::size_t index_v, T && type_v, Direction direction_v, P && begin_v, P && end_v, double priority_v) :
					index(index_v), type(std::forward < T > (type_v)), direction(direction_v),
					begin(std::forward < P > (begin_v)), end(std::forward < P > (end_v)), priority(priority_v), 
					at_end(false), command(Command::stay), m_deviation(0.0), m_segment(begin), m_segment_time(0LL)
				{
					initialize();
				}

				~Train() noexcept = default;

			private:

				void initialize();

			public:

				const auto deviation() const noexcept
				{
					return m_deviation;
				}

				const auto & segment() const noexcept
				{
					return m_segment;
				}

			public:

				bool is_ready(std::time_t standard_time) const;

				bool has_completed_movement() const;

				bool has_completed_route(std::time_t standard_time) const;

				void stay(std::time_t standard_time);

				void move(const std::string & segment);

				void set_segment_time(std::time_t segment_time) const noexcept
				{
					m_segment_time = segment_time;
				}

			public: // const

				std::size_t index; // уникальный номер поезда (номер нитки)
				std::string type; // тип поезда
				Direction direction; // направление движени€
				std::string begin; // название сегмента, с которого начинаетс€ движение
				std::string end; // название сегмента, на который в конце надо приехать
				double priority; // приоритет поезда
				bool at_end; // доехал ли поезд до последнего сегмента своего маршрута
				Command command; // текуща€ команда, которую поезд должен исполн€ть

			private:

				double m_deviation; // отклонение от плана, комонент оптимизируемой функции

				std::string m_segment; // название сегмента, на котором поезд сейчас находитс€

			private:

				mutable std::time_t m_segment_time; // врем€, которое поезд находитс€ на текущем сегменте
			};

		} // namespace agents

	} // namespace plugin

} // namespace solution

#endif // #ifndef SOLUTION_PLUGIN_AGENTS_TRAIN_HPP