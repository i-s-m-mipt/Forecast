#pragma once

#include "zsheader.hpp"

#include <iostream>
#include <array>

namespace aim {

	class CVersion {
	public:
		static const int s_str_size = 4;
		static const int s_version_size = 7;

		CVersion()
			: m_version(2)
		{
			m_magic.fill('0');
		}

		CVersion(const std::array<char, s_str_size>& magic, int ver)
			: m_version(ver)
		{
			m_magic.fill('0');
			std::copy(magic.begin(), magic.end(), m_magic.begin());
			int v = std::abs(ver) % 1000;
			std::string s = std::to_string((long long) v);
			std::copy(s.rbegin(), s.rend(), m_magic.rbegin());
		}

		CVersion(const std::string& magic, int ver)
			: m_version(ver)
		{
			m_magic.fill('0');
			std::copy(magic.begin(), magic.begin() + s_str_size, m_magic.begin());
			int v = std::abs(ver) % 1000;
			std::string s = std::to_string((long long)v);
			std::copy(s.rbegin(), s.rend(), m_magic.rbegin());
		}

		void fromData(const std::array<char, s_version_size>& magic) {
			m_magic = magic;
			std::string s(s_version_size - s_str_size, '0');
			std::copy(m_magic.begin() + s_str_size, m_magic.end(), s.begin());
			
			m_version = std::stoi(s);
		}

		bool empty() const {
			for (auto& c : m_magic) {
				if (c != '0')
					return false;
			}
			return true;
		}

		const char* data() const { return m_magic.data(); }
		char* data() { return m_magic.data(); }

		const std::array<char, s_version_size>& magic() const { return m_magic; }
		int version() const { return m_version; }
	protected:
		std::array<char, s_version_size> m_magic;
		int m_version;
	};
	/*!	 * сериализатор данных интеллектуального модуля
	 */
	class CSerializer {
	public:
		/*!
		* сериализация данных инициализации алгоритмов
		* @param[in] statefname название файла конфигурации для конкретного алгоритма
		* @param[in] cfg		общие настройки для всех алгоритмов
		* @param[in] Graph		граф ЖД
		* @return указатель на сериализованные данные
		*/
		const char* serializeInit(const std::string & statefname, const Config& cfg, const Graph & graph_input);
		/*!
		* сериализация рабочих данных алгоритмов для использования в однопоточном режиме, используется только для построения расписания
		* @param[in] zakaz			список ниток
		* @param[in] zapr			список аншлагов
		* @param[in] currentTime	текущее время
		* @param[in] isPrognoz		устаревший параметр, не используется
		* @return указатель на сериализованные данные
		*/
		const char* serializeWork(const std::vector <Nitka> & zakaz, const std::vector<Zapret> & zapr, int currentTime, bool isPrognoz);
		/*!
		* сериализация рабочих данных алгоритмов для использования в многопоточном режиме
		* @param[in] zakaz			список ниток и исполненного движения
		* @param[in] zapr			список аншлагов
		* @param[in] currentTime	текущее время
		* @param[in] interval	максимальный интервал прогнозирования, в секундах
		* @param[in] isPrognoz		ключ режима работы алгоритма
		* @return указатель на сериализованные данные
		*/
		const char* serializeStartWork(const std::vector <NitkaID> & zakaz, const std::vector<Zapret> & zapr, int currentTime, int interval, bool isPrognoz);
		/*!
		* сериализация результата инициализации алгоритмов
		* @param[in] messages сообщения
		* @return указатель на сериализованные данные
		*/
		const char* serializeInitResult(const std::string & messages);
		/*!
		* сериализация результата работы алгоритмов для использования в однопоточном режиме, используется только для построения расписания
		* @param[in] messages		сообщения
		* @param[in] result			списки ниток, варианты результата работы алгоритма
		* @return указатель на сериализованные данные
		*/
		const char* serializeWorkResult(const std::string & messages, const std::vector <std::vector <Nitka> > & result);
		/*!
		* сериализация  результата работы алгоритмов для использования в многопоточном режиме
		* @param[in] messages		сообщения
		* @return указатель на сериализованные данные
		*/
		const char* serializeStartWorkResult(const std::string & messages);
		const char* serializeGetResult(const std::string & messages, double progress, const std::vector <std::vector <NitkaID> > & result);

		void deserializeInit(const char* data, std::string& statefname, Config& cfg, Graph& graph_input);
		void deserializeWork(const char* data, std::vector <Nitka> & zakaz, std::vector<Zapret> & zapr, int& currentTime, bool& isPrognoz);
		void deserializeStartWork(const char* data, std::vector <NitkaID> & zakaz, std::vector<Zapret> & zapr, int& currentTime, int& interval, bool& isPrognoz);

		void deserializeInitResult(const char* data, std::string & messages);
		void deserializeWorkResult(const char* data, std::string & messages, std::vector <std::vector <Nitka> > & result);
		void deserializeStartWorkResult(const char* data, std::string & messages);
		void deserializeGetResult(const char* data, std::string & messages, double& progress, std::vector <std::vector <NitkaID> > & result);

		bool save(const char* filename, const char* data);
		bool load(const char* filename, char*& data);

		//! Return last error and clear it
		std::string last_error() { std::string res; std::swap(m_last_error, res); return res; }

		static int version() { return s_version; }

	protected:
		const char* writeHeader(const std::string & data, const CVersion& v);
		const char* readHeader(const char* data, const CVersion& v, uint64_t& datasize);

		std::string m_last_error;
		std::string m_serialized_data;
		static const int s_version = 2;
		static const std::string s_init_magic;
		static const std::string s_work_magic;
		static const std::string s_start_work_magic;
		static const std::string s_init_result_magic;
		static const std::string s_work_result_magic;
		static const std::string s_start_work_result_magic;
		static const std::string s_get_result_magic;
	};



#define RETURN_IF_NOT(func) { bool res = func; if(!res) return res; }
#define DECLARE_NAME_TYPE(type) template<> std::string name_type< type >()

	template<class T>
	std::string name_type()
	{
#if defined(_MSC_VER)
		static_assert(false, "Unknown name type");
#else
		return "";
#endif
	}

	DECLARE_NAME_TYPE(std::string);
	DECLARE_NAME_TYPE(bool);
	DECLARE_NAME_TYPE(int32_t);
	DECLARE_NAME_TYPE(uint32_t);
	DECLARE_NAME_TYPE(int64_t);
	DECLARE_NAME_TYPE(uint64_t);
	DECLARE_NAME_TYPE(double);
	DECLARE_NAME_TYPE(Nitka);
	DECLARE_NAME_TYPE(NitkaID);
	DECLARE_NAME_TYPE(Point);
	DECLARE_NAME_TYPE(Zapret);
	DECLARE_NAME_TYPE(Graph);
	DECLARE_NAME_TYPE(StandardTime);
	DECLARE_NAME_TYPE(Config);
	DECLARE_NAME_TYPE(std::vector<Point>);
	DECLARE_NAME_TYPE(std::vector<Nitka>);
	DECLARE_NAME_TYPE(std::vector<NitkaID>);
	DECLARE_NAME_TYPE(std::vector<Zapret>);
	DECLARE_NAME_TYPE(std::vector<int32_t>);
	DECLARE_NAME_TYPE(std::vector<Node>);
	DECLARE_NAME_TYPE(std::vector<Line>);
	DECLARE_NAME_TYPE(std::vector<StandardTime>);
	DECLARE_NAME_TYPE(std::vector< std::vector<Nitka> >);
	DECLARE_NAME_TYPE(std::vector< std::vector<NitkaID> >);

	bool serialize(std::ostream& stream, const CVersion& s);
	bool serialize(std::ostream& stream, const std::string& s);
	bool serialize(std::ostream& stream, const bool& s);
	bool serialize(std::ostream& stream, const int32_t& s);
	bool serialize(std::ostream& stream, const uint32_t& s);
	bool serialize(std::ostream& stream, const int64_t& s);
	bool serialize(std::ostream& stream, const uint64_t& s);
	bool serialize(std::ostream& stream, const double& s);
	bool serialize(std::ostream& stream, const Nitka& s);
	bool serialize(std::ostream& stream, const NitkaID& s);
	bool serialize(std::ostream& stream, const Point& s);
	bool serialize(std::ostream& stream, const Zapret& s);
	bool serialize(std::ostream& stream, const Line& s);
	bool serialize(std::ostream& stream, const Node& s);
	bool serialize(std::ostream& stream, const Graph& s);
	bool serialize(std::ostream& stream, const StandardTime& s);
	bool serialize(std::ostream& stream, const Config& s);

	template<class T>
	bool serialize(std::ostream& stream, const std::vector<T>& v) {
		RETURN_IF_NOT(serialize(stream, v.size()));
		for (auto& i : v) {
			RETURN_IF_NOT(serialize(stream, i));
		}
		return true;
	}

	template<class T>
	bool serialize(std::ostream& stream, const std::string& t, const T& item) {
		RETURN_IF_NOT(serialize(stream, t));
		RETURN_IF_NOT(serialize(stream, item));
		return true;
	}

	template<class T>
	bool serialize_named(std::ostream& stream, const T& s) {
		return serialize(stream, name_type<T>(), s);
	}

	bool deserialize(std::istream& stream, CVersion& s);
	bool deserialize(std::istream& stream, std::string& s);
	bool deserialize(std::istream& stream, bool& s);
	bool deserialize(std::istream& stream, int32_t& s);
	bool deserialize(std::istream& stream, uint32_t& s);
	bool deserialize(std::istream& stream, int64_t& s);
	bool deserialize(std::istream& stream, uint64_t& s);
	bool deserialize(std::istream& stream, double& s);
	bool deserialize(std::istream& stream, Nitka& s);
	bool deserialize(std::istream& stream, NitkaID& s);
	bool deserialize(std::istream& stream, Point& s);
	bool deserialize(std::istream& stream, Zapret& s);
	bool deserialize(std::istream& stream, Line& s);
	bool deserialize(std::istream& stream, Node& s);
	bool deserialize(std::istream& stream, Graph& s);
	bool deserialize(std::istream& stream, StandardTime& s);
	bool deserialize(std::istream& stream, Config& s);

	template<class T>
	bool deserialize(std::istream& stream, std::vector<T>& v) {
		size_t sz = 0;
		RETURN_IF_NOT(deserialize(stream, sz));
		v.resize(sz);
		for (auto& i : v) {
			RETURN_IF_NOT(deserialize(stream, i));
		}
		return true;
	}

	template<class T>
	bool deserialize(std::istream &stream, const std::string& t, T &item) {
		std::string type;
		RETURN_IF_NOT(deserialize(stream, type));
		if (type.compare(t) == 0) {
			return deserialize(stream, item);
		}
		return false;
	}

	template<class T>
	bool deserialize_named(std::istream& stream, T& s) {
		return deserialize(stream, name_type<T>(), s);
	}
};
