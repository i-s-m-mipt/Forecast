#include "segment.hpp"

namespace solution
{
	namespace plugin
	{
		namespace module
		{
			// �������� ����������� ����� ��� �������� ��� ������� ���� ������ � �����������
			std::time_t Segment::standard_time(std::string type, Direction direction) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_standard_times.find(type) == std::end(m_standard_times))
					{
						type = "train";
					}

					switch (direction)
					{
					case Direction::north:
					{
						return m_standard_times.at(type).time_to_north;
					}
					case Direction::south:
					{
						return m_standard_times.at(type).time_to_south;
					}
					default:
					{
						throw std::runtime_error("unknown direction");
					}
					}

					return 0LL;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);

					return 0LL;
				}
			}

			// �� ������� ������ �����
			void Segment::train_arrived() const
			{
				RUN_LOGGER(logger);

				try
				{
					m_has_train = true;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// ����� ������� �������
			void Segment::train_departured(std::time_t time) const
			{
				RUN_LOGGER(logger);

				try
				{
					// �����: has_train �� ���������� false, �.�. ��������
					// ����������� ���������. ������ ������ �������� �������
					// ����� ������� ��������� ���������
					m_last_departure = time;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// � ��� ��� ��� ������ ����������� ��������� ����� ��������
			void Segment::update_status(std::time_t time) const
			{
				RUN_LOGGER(logger);

				try
				{
					if (m_last_departure && time - m_last_departure >= m_interval)
					{
						m_has_train = false;

						m_last_departure = 0LL;
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// �� ������� ����� ��������, ���� ��� ������ (���� ����������) � ��� �������
			bool Segment::is_available() const
			{
				RUN_LOGGER(logger);

				try
				{
					// � ���� ����� ���� �������� ��� ��������� ��������, �����
					// ����� �������� �� �������� ������. �������������� �������
					// � ���� �� ��������� ��� ������ ������, � ��� ������������
					// ��� ���������� �����������
					return (!m_has_train && state != State::locked);
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// ��������� �������������
			void Segment::append_northern_adjacent_segment(const std::string & segment)
			{
				RUN_LOGGER(logger);

				try
				{
					const auto & segments = m_northern_adjacent_segments;

					if (std::find(std::begin(segments), std::end(segments), segment) == std::end(segments))
					{
						m_northern_adjacent_segments.push_back(segment);

						m_northern_adjacent_segments.shrink_to_fit();
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// ��������� �������������
			void Segment::append_southern_adjacent_segment(const std::string & segment)
			{
				RUN_LOGGER(logger);

				try
				{
					const auto & segments = m_southern_adjacent_segments;

					if (std::find(std::begin(segments), std::end(segments), segment) == std::end(segments))
					{
						m_southern_adjacent_segments.push_back(segment);

						m_southern_adjacent_segments.shrink_to_fit();
					}
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

			// ��������� �������������
			void Segment::set_standard_time(const std::string & type, Time time)
			{
				RUN_LOGGER(logger);

				try
				{
					m_standard_times[type] = time;
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < segment_exception > (logger, exception);
				}
			}

		} // namespace module

	} // namespace plugin

} // namespace solution