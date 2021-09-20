#ifndef SOLUTION_PLUGIN_SYSTEM_HPP
#define SOLUTION_PLUGIN_SYSTEM_HPP

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#  pragma once
#endif // #ifdef BOOST_HAS_PRAGMA_ONCE

#include <algorithm>
#include <atomic>
#include <deque>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "agents/train/train.hpp"
#include "config/config.hpp"
#include "module/segment/segment.hpp"

#include "../common/zsheader.hpp"

#include "../../../shared/source/logger/logger.hpp"

namespace solution 
{
	namespace plugin
	{
		class system_exception : public std::exception
		{
		public:

			explicit system_exception(const std::string & message) noexcept : 
				std::exception(message.c_str()) 
			{}

			explicit system_exception(const char * const message) noexcept :
				std::exception(message)
			{}

			~system_exception() noexcept = default;
		};

		class System
		{
		private:

			using input_segments_t = Graph;

			using input_routes_t = std::vector < NitkaID > ;

			using input_locks_t = std::vector < Zapret > ;

			using Segment = module::Segment;

			using segments_container_t = std::unordered_map < std::string, Segment > ;

			using Train = agents::Train;

			using trains_container_t = std::vector < Train > ;

			using routes_container_t = std::vector < std::pair < std::time_t, Train > > ;

		private:

			struct Lock // ������ ��������
			{
				std::string segment; // �������� ��������

				std::time_t begin; // ����� ������
				std::time_t end; // ����� ���������
			};

		private:

			using locks_container_t = std::vector < Lock > ;

		private:

			// ���� ����������� ������ (��������) ��������� ��������. ���� ���������� �
			// ���� ���� ������������, ��� �������� ����� ���� �������� � ����������
			// ������������ ����� �.�. ����������� ������. � ���� ����������� ������
			// ���������� ������� ������ ���� �������� ����� ��������, ������� �����������
			// � ���������� ���������� ���� �������� ������� �� ����������� � �����������
			// �� �������� ������������ ��������, ����� ���� ����� �������� ���������������
			// ������� � ������� ����������. ��� ����� �������� ������� ����������� ��������
			// ����������� �������� � ������������ � ��������������� ������������. ����������
			// ��� ��� ��������� ����� � ��� ����������� �������� ���� �� ��������, �����
			// ����������� ���� ��������� ���������. ��� ��������� �������� ���� ������ ���
			// ������ �� ������� ��������� �������� �������������� ������������, ������
			// ����� ������ ��� ��� � �� ���������. ������ ������������� ����� ���� �� ���
			// �/��� � �����, ������ ��� �������� ������ ���������� ������������� ��������,
			// � �� ����������� ���������� �� ��������� ����������, ������� ����������
			// �����������. ����� � ���� ����� ��������, ��� ������� ���� ����� ��������.
			struct Node
			{
				void update_deviation();

				segments_container_t segments; // �������� � ����������� � ���������� � ���������

				trains_container_t trains; // ������ � ���������� �� ������ ������ ������� ���������

				bool has_event = false; // ������� ��������� ��������� � ��������

				double deviation = 0.0; // ��������� ���������� �� ���� �������

				Node * parent = nullptr; // ������������ ����

				std::vector < std::unique_ptr < Node > > children; // ���� ��������

				std::size_t completed_routes_counter = 0U; // ������� ����� ��� ����������� �� ��������
			};

		private:

			struct Point // ��������� �����
			{
				std::string segment;

				std::time_t time;
			};

		private:

			struct Chart // �����
			{
				std::time_t start = 0LL;

				std::string type;

				double priority = 0.0;

				std::vector < Point > points;
			};

		private:

			using charts_container_t = std::vector < Chart > ;

		public:

			explicit System(
				const input_segments_t & input_segments, 
				const input_routes_t   & input_routes,
				const input_locks_t    & input_locks, std::time_t interval) : m_time_begin(0LL), m_head(nullptr), 
				m_interval(interval), m_done_flag(false), m_interrupt_flag(false)
			{
				initialize(input_segments, input_routes, input_locks);
			}

			~System() noexcept
			{
				try
				{
					uninitialize();
				}
				catch (...)
				{
					// std::abort();
				}
			}

		private:

			void initialize(
				const input_segments_t & input_segments,
				const input_routes_t   & input_routes,
				const input_locks_t    & input_locks);

			void uninitialize();

		private:

			void make_segments(const input_segments_t & input_segments);

			void make_routes(const input_routes_t & input_routes);

			void make_locks(const input_locks_t & input_locks);

		private:

			Direction get_direction(const std::string & begin, const std::string & end) const;

		public:

			const auto & charts() const noexcept
			{
				return m_charts;
			}

		public:

			void run();

		private:

			void make_tree();

			void make_charts();

		private:
			
			void make_trains(std::time_t time, Node * node);

			void update_segments(std::time_t time, Node * node) const;

			void execute_commands(std::time_t time, Node * node) const;

			bool has_deadlock(Node * node, std::string segment, Direction direction) const;

			bool has_deadend(Node * node, std::string next_segment, 
				std::string last_segment, Direction direction) const;

			void make_new_nodes(Node * node);

		public:

			bool is_done() const noexcept
			{
				return m_done_flag.load();
			}

			void interrupt() const noexcept
			{
				m_interrupt_flag.store(true);
			}

		private:

			static const std::time_t seconds_in_minute = 60LL;

			static const std::size_t strategies_limit = 1U; // ����������� ������ ��� ��������/���������� � ��.

			static const std::time_t time_limit = 7200U; // �� ������� ������ �������������� ��������

		private:

			segments_container_t m_segments; // ��������� ����

			routes_container_t m_routes; // �������� �����

			locks_container_t m_locks; // �������� �������

			std::time_t m_time_begin; // � ������ ������� ������� �������������� ��������

			std::unique_ptr < Node > m_head; // �������� ���� �������� ���������� � m_time_begin

			std::deque < Node * > m_leafs; // ������ (���� ���� � ����������� ������)

			charts_container_t m_charts; // �������� ����� �����������������

			std::time_t m_interval; // �������� ��� ���������

		private:

			mutable std::atomic < bool > m_done_flag; // ��� dll

			mutable std::atomic < bool > m_interrupt_flag; // ��� dll
		};

	} // namespace plugin

} // namespace solution

#endif // #ifndef SOLUTION_PLUGIN_SYSTEM_HPP