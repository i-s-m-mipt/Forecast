#include "serializer.h"
#include "string.h"

/*
int aimProtocolVersion()
{
	return aim::CSerializer::version();
}
*/

struct membuf : std::streambuf
{
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
	}
};

namespace aim {

	const std::string CSerializer::s_init_magic = "VerI";
	const std::string CSerializer::s_work_magic = "VerW";
	const std::string CSerializer::s_start_work_magic = "VerS";
	const std::string CSerializer::s_init_result_magic = "VerT";
	const std::string CSerializer::s_work_result_magic = "VerR";
	const std::string CSerializer::s_start_work_result_magic = "VerK";
	const std::string CSerializer::s_get_result_magic = "VerG";

	const char* CSerializer::writeHeader(const std::string & data, const CVersion& v)
	{
		uint64_t buffer_size = data.size();

		std::stringstream ss;
		bool res = aim::serialize(ss, v);
		res &= aim::serialize(ss, buffer_size);
		ss.write(data.c_str(), buffer_size);
		res &= !ss.fail();

		if (!res)
			m_serialized_data = std::string();
		else
			m_serialized_data = ss.str();

		return m_serialized_data.c_str();
	}

	const char* CSerializer::readHeader(const char* data, const CVersion& current, uint64_t& datasize)
	{
		datasize = 0;
		if (data == nullptr) {
			return nullptr;
		}

		int32_t header_size = CVersion::s_version_size + sizeof(datasize);

		CVersion v;

		char* start = const_cast<char*>(data);
		membuf buf(start, start + header_size);
		std::istream ss(&buf);

		try {
			if (!aim::deserialize(ss, v)) {
				if (!current.empty() && (v.magic() != current.magic() || v.version() != current.version())) {
					m_last_error = std::string("Wrong serializer version ") + v.data() + " != " + current.data();
					return nullptr;
				}
			}
		}
		catch (std::exception& e) {
			m_last_error = e.what();
			return nullptr;
		}

		ss.read(reinterpret_cast<char*>(&datasize), sizeof(datasize));

		if (ss.fail() || datasize == 0) {
			return nullptr;
		}

		return start + header_size;
	}

	const char* CSerializer::serializeInit(const std::string & statefname, const Config& cfg, const Graph & graph_input)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, statefname);
		res &= aim::serialize_named(ss, cfg);
		res &= aim::serialize_named(ss, graph_input);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}
		
		return writeHeader(ss.str(), CVersion(s_init_magic, s_version));
	}

	const char* CSerializer::serializeWork(const std::vector <Nitka> & zakaz, const std::vector<Zapret> & zapr, int currentTime, bool isPrognoz)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, zakaz);
		res &= aim::serialize_named(ss, zapr);
		res &= aim::serialize_named(ss, currentTime);
		res &= aim::serialize_named(ss, isPrognoz);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_work_magic, s_version));
	}

	const char* CSerializer::serializeStartWork(const std::vector <NitkaID> & zakaz, const std::vector<Zapret> & zapr, int currentTime, int interval, bool isPrognoz)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, zakaz);
		res &= aim::serialize_named(ss, zapr);
		res &= aim::serialize_named(ss, currentTime);
		res &= aim::serialize_named(ss, interval);
		res &= aim::serialize_named(ss, isPrognoz);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_start_work_magic, s_version));
	}

	const char* CSerializer::serializeInitResult(const std::string & messages)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, messages);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_init_result_magic, s_version));
	}

	const char* CSerializer::serializeWorkResult(const std::string & messages, const std::vector <std::vector <Nitka> > & result)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, messages);
		res &= aim::serialize_named(ss, result);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_work_result_magic, s_version));
	}

	const char* CSerializer::serializeStartWorkResult(const std::string & messages)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, messages);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_start_work_result_magic, s_version));
	}

	const char* CSerializer::serializeGetResult(const std::string & messages, double progress, const std::vector <std::vector <NitkaID> > & result)
	{
		std::stringstream ss;
		bool res = aim::serialize_named(ss, messages);
		res &= aim::serialize_named(ss, progress);
		res &= aim::serialize_named(ss, result);

		if (!res) {
			m_serialized_data = std::string();
			return m_serialized_data.c_str();
		}

		return writeHeader(ss.str(), CVersion(s_get_result_magic, s_version));
	}

	void CSerializer::deserializeInit(const char* data, std::string& statefname, Config& cfg, Graph& graph_input)
	{
		CVersion current(s_init_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, statefname)) {
			m_last_error = std::string("Error in deserializing statefname");
			return;
		}

		if (!aim::deserialize_named(ss, cfg)) {
			m_last_error = std::string("Error in deserializing config");
			return;
		}

		if (!aim::deserialize_named(ss, graph_input)) {
			m_last_error = std::string("Error in deserializing graph_input");
			return;
		}
	}

	void CSerializer::deserializeWork(const char* data, std::vector <Nitka> & zakaz, std::vector<Zapret> & zapr, int& currentTime, bool& isPrognoz)
	{
		CVersion current(s_work_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}
 
		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, zakaz)) {
			m_last_error = std::string("Error in deserializing zakaz");
			return;
		}

		if (!aim::deserialize_named(ss, zapr)) {
			m_last_error = std::string("Error in deserializing zapr");
			return;
		}

		if (!aim::deserialize_named(ss, currentTime)) {
			m_last_error = std::string("Error in deserializing currentTime");
			return;
		}

		if (!aim::deserialize_named(ss, isPrognoz)) {
			m_last_error = std::string("Error in deserializing isPrognoz");
			return;
		}
	}

	void CSerializer::deserializeStartWork(const char* data, std::vector <NitkaID> & zakaz, std::vector<Zapret> & zapr, int& currentTime, int& interval, bool& isPrognoz)
	{
		CVersion current(s_start_work_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, zakaz)) {
			m_last_error = std::string("Error in deserializing zakaz");
			return;
		}

		if (!aim::deserialize_named(ss, zapr)) {
			m_last_error = std::string("Error in deserializing zapr");
			return;
		}

		if (!aim::deserialize_named(ss, currentTime)) {
			m_last_error = std::string("Error in deserializing currentTime");
			return;
		}

		if (!aim::deserialize_named(ss, interval)) {
			m_last_error = std::string("Error in deserializing interval");
			return;
		}

		if (!aim::deserialize_named(ss, isPrognoz)) {
			m_last_error = std::string("Error in deserializing isPrognoz");
			return;
		}
	}

	void CSerializer::deserializeInitResult(const char* data, std::string & messages)
	{
		CVersion current(s_init_result_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, messages)) {
			m_last_error = std::string("Error in deserializing messages");
			return;
		}
	}

	void CSerializer::deserializeWorkResult(const char* data, std::string & messages, std::vector <std::vector <Nitka> > & result)
	{
		CVersion current(s_work_result_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, messages)) {
			m_last_error = std::string("Error in deserializing messages");
			return;
		}

		if (!aim::deserialize_named(ss, result)) {
			m_last_error = std::string("Error in deserializing result");
			return;
		}
	}

	void CSerializer::deserializeStartWorkResult(const char* data, std::string & messages)
	{
		CVersion current(s_start_work_result_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, messages)) {
			m_last_error = std::string("Error in deserializing messages");
			return;
		}
	}

	void CSerializer::deserializeGetResult(const char* data, std::string & messages, double& progress, std::vector <std::vector <NitkaID> > & result)
	{
		CVersion current(s_get_result_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return;
		}

		char* start = const_cast<char*>(data2);
		membuf buf(start, start + datasize);
		std::istream ss(&buf);

		if (!aim::deserialize_named(ss, messages)) {
			m_last_error = std::string("Error in deserializing messages");
			return;
		}

		if (!aim::deserialize_named(ss, progress)) {
			m_last_error = std::string("Error in deserializing progress");
			return;
		}

		if (!aim::deserialize_named(ss, result)) {
			m_last_error = std::string("Error in deserializing result");
			return;
		}
	}

	bool CSerializer::save(const char* filename, const char* data)
	{
		CVersion current(s_init_result_magic, s_version);
		uint64_t datasize = 0;
		const char* data2 = readHeader(data, current, datasize);

		if (data2 == nullptr) {
			m_last_error = std::string("Empty data found");
			return false;
		}

		uint64_t header_size = CVersion::s_version_size + sizeof(datasize);
		uint64_t full_size = header_size + datasize;
		
		std::ofstream f;
		f.open(filename, std::ios::binary | std::ios::trunc);
		if (!f) {
			m_last_error = std::string("Can't open file: ") + filename;
			return false;
		}
		if (!f.write(data, full_size)) {
			m_last_error = std::string("Can't save ") + std::to_string(full_size) + " bytes to file";
			return false;
		}
		f.close();
		return true;
	}

	bool CSerializer::load(const char* filename, char*& data)
	{
		std::ifstream f;
		f.open(filename, std::ios::binary);

		if (!f) {
			m_last_error = std::string("Can't open file: ") + filename;
			return false;
		}

		uint64_t datasize = 0;
		uint64_t header_size = CVersion::s_version_size + sizeof(datasize);
		char* buf_header = new char[static_cast<size_t>(header_size)];

		if (!f.read(buf_header, header_size)) {
			m_last_error = std::string("Can't load header contains ") + std::to_string(header_size) + " bytes";
			return false;
		}

		const char* data2 = readHeader(buf_header, CVersion(), datasize);
		if (!data2) {
			m_last_error = std::string("Can't read header");
			return false;
		}

		m_serialized_data.clear();
		m_serialized_data.resize(static_cast<size_t>(header_size + datasize));
		memcpy(&(m_serialized_data[0]), buf_header, static_cast<size_t>(header_size));

		char* data_ptr = &(m_serialized_data[0]) + header_size;
		if (!f.read(data_ptr, datasize)) {
			m_last_error = std::string("Can't load data contains ") + std::to_string(datasize) + " bytes";
			return false;
		}
		data = &(m_serialized_data[0]);
		return true;
	}

#define IMPLEMENT_NAME_TYPE(type) template<> std::string name_type< type >() { return #type; }
#define IMPLEMENT_NAME_TYPE_STRING(type, name) template<> std::string name_type< type >() { return name; }

	IMPLEMENT_NAME_TYPE_STRING(std::string, "string");
	IMPLEMENT_NAME_TYPE(bool);
	IMPLEMENT_NAME_TYPE_STRING(int32_t, "int32");
	IMPLEMENT_NAME_TYPE(double);
	IMPLEMENT_NAME_TYPE_STRING(uint32_t, "uint32");
	IMPLEMENT_NAME_TYPE_STRING(int64_t, "int64");
	IMPLEMENT_NAME_TYPE_STRING(uint64_t, "uint64");
	IMPLEMENT_NAME_TYPE(Nitka);
	IMPLEMENT_NAME_TYPE(NitkaID);
	IMPLEMENT_NAME_TYPE(Point);
	IMPLEMENT_NAME_TYPE(Zapret);
	IMPLEMENT_NAME_TYPE(Line);
	IMPLEMENT_NAME_TYPE(Node);
	IMPLEMENT_NAME_TYPE(Graph);
	IMPLEMENT_NAME_TYPE(StandardTime);
	IMPLEMENT_NAME_TYPE(Config);
	IMPLEMENT_NAME_TYPE_STRING(std::vector<Point>, "vector<Point>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<Nitka>, "vector<Nitka>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<NitkaID>, "vector<NitkaID>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<Zapret>, "vector<Zapret>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<int32_t>, "vector<int>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<Node>, "vector<Node>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<Line>, "vector<Line>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector<StandardTime>, "vector<StandardTime>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector< std::vector<Nitka> >, "vector<vector<Nitka>>");
	IMPLEMENT_NAME_TYPE_STRING(std::vector< std::vector<NitkaID> >, "vector<vector<NitkaID>>");

	bool serialize(std::ostream& stream, const CVersion& s)
	{
		stream.write(s.data(), CVersion::s_version_size);
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const std::string& s)
	{
		size_t len = s.size();
		stream.write(reinterpret_cast<const char*>(&len), sizeof(len));
		RETURN_IF_NOT(!stream.fail());
		stream.write(s.c_str(), len);
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const bool& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const int32_t& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const uint32_t& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const int64_t& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const uint64_t& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const double& s)
	{
		stream.write(reinterpret_cast<const char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool serialize(std::ostream& stream, const Nitka& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.StartTime));
		RETURN_IF_NOT(serialize_named(stream, s.type));
		RETURN_IF_NOT(serialize_named(stream, s.priority));
		RETURN_IF_NOT(serialize_named(stream, s.points));
		return true;
	}

	bool serialize(std::ostream& stream, const NitkaID& s)
	{
		RETURN_IF_NOT(serialize(stream, static_cast<const Nitka&>(s)));
		RETURN_IF_NOT(serialize_named(stream, s.idStartTime));
		RETURN_IF_NOT(serialize_named(stream, s.idPoints));
		RETURN_IF_NOT(serialize_named(stream, s.idLastPosition));
		RETURN_IF_NOT(serialize_named(stream, s.pgStartTime));
		RETURN_IF_NOT(serialize_named(stream, s.pgPoints));
		return true;
	}

	bool serialize(std::ostream& stream, const Point& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.name));
		RETURN_IF_NOT(serialize_named(stream, s.dt));
		RETURN_IF_NOT(serialize_named(stream, s.lenght));
		RETURN_IF_NOT(serialize_named(stream, s.weight));
		RETURN_IF_NOT(serialize_named(stream, s.maxspeed));
		RETURN_IF_NOT(serialize_named(stream, s.needOnlyThisWay));
		return true;
	}

	bool serialize(std::ostream& stream, const Zapret& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.name));
		RETURN_IF_NOT(serialize_named(stream, s.from));
		RETURN_IF_NOT(serialize_named(stream, s.to));
		RETURN_IF_NOT(serialize_named(stream, s.maxspeed));
		return true;
	}

	bool serialize(std::ostream& stream, const Line& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.srcNode));
		RETURN_IF_NOT(serialize_named(stream, s.dstNode));
		RETURN_IF_NOT(serialize_named(stream, (int)s.srcDirection));
		RETURN_IF_NOT(serialize_named(stream, (int)s.dstDirection));
		return true;
	}

	bool serialize(std::ostream& stream, const Node& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.name));
		RETURN_IF_NOT(serialize_named(stream, s.dist_size));
		RETURN_IF_NOT(serialize_named(stream, s.standard_times));
		return true;
	}

	bool serialize(std::ostream& stream, const Graph& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.nodes));
		RETURN_IF_NOT(serialize_named(stream, s.lines));
		return true;
	}

	bool serialize(std::ostream& stream, const StandardTime& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.type));
		RETURN_IF_NOT(serialize_named(stream, s.time));
		RETURN_IF_NOT(serialize_named(stream, s.timeRev));
		return true;
	}

	bool serialize(std::ostream& stream, const Config& s)
	{
		RETURN_IF_NOT(serialize_named(stream, s.Do1));
		RETURN_IF_NOT(serialize_named(stream, s.Posle1));
		RETURN_IF_NOT(serialize_named(stream, s.NumVar1));
		RETURN_IF_NOT(serialize_named(stream, s.nvar1));
		return true;
	}

	bool deserialize(std::istream& stream, CVersion& s)
	{
		std::array<char, CVersion::s_version_size> data;
		stream.read(data.data(), CVersion::s_version_size);
		RETURN_IF_NOT(!stream.fail());
		s.fromData(data);
		return true;
	}

	bool deserialize(std::istream& stream, std::string& s)
	{
		size_t len = 0;
		stream.read(reinterpret_cast<char*>(&len), sizeof(len));
		RETURN_IF_NOT(!stream.fail());
		s.resize(len);
		stream.read(const_cast<char*>(s.data()), len);
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, bool& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, int32_t& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, uint32_t& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, int64_t& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, uint64_t& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, double& s)
	{
		stream.read(reinterpret_cast<char*>(&s), sizeof(s));
		RETURN_IF_NOT(!stream.fail());
		return true;
	}

	bool deserialize(std::istream& stream, Nitka& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.StartTime));
		RETURN_IF_NOT(deserialize_named(stream, s.type));
		RETURN_IF_NOT(deserialize_named(stream, s.priority));
		RETURN_IF_NOT(deserialize_named(stream, s.points));
		return true;
	}

	bool deserialize(std::istream& stream, NitkaID& s)
	{
		RETURN_IF_NOT(deserialize(stream, static_cast<Nitka&>(s)));
		RETURN_IF_NOT(deserialize_named(stream, s.idStartTime));
		RETURN_IF_NOT(deserialize_named(stream, s.idPoints));
		RETURN_IF_NOT(deserialize_named(stream, s.idLastPosition));
		RETURN_IF_NOT(deserialize_named(stream, s.pgStartTime));
		RETURN_IF_NOT(deserialize_named(stream, s.pgPoints));
		return true;
	}

	bool deserialize(std::istream& stream, Point& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.name));
		RETURN_IF_NOT(deserialize_named(stream, s.dt));
		RETURN_IF_NOT(deserialize_named(stream, s.lenght));
		RETURN_IF_NOT(deserialize_named(stream, s.weight));
		RETURN_IF_NOT(deserialize_named(stream, s.maxspeed));
		RETURN_IF_NOT(deserialize_named(stream, s.needOnlyThisWay));
		return true;
	}

	bool deserialize(std::istream& stream, Zapret& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.name));
		RETURN_IF_NOT(deserialize_named(stream, s.from));
		RETURN_IF_NOT(deserialize_named(stream, s.to));
		RETURN_IF_NOT(deserialize_named(stream, s.maxspeed));
		return true;
	}

	bool deserialize(std::istream& stream, Line& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.srcNode));
		RETURN_IF_NOT(deserialize_named(stream, s.dstNode));
		int dir;
		RETURN_IF_NOT(deserialize_named(stream, dir)) s.srcDirection = (Line::LineDirection)dir;
		RETURN_IF_NOT(deserialize_named(stream, dir)) s.dstDirection = (Line::LineDirection)dir;
		return true;
	}

	bool deserialize(std::istream& stream, Node& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.name));
		RETURN_IF_NOT(deserialize_named(stream, s.dist_size));
		RETURN_IF_NOT(deserialize_named(stream, s.standard_times));
		return true;
	}

	bool deserialize(std::istream& stream, Graph& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.nodes));
		RETURN_IF_NOT(deserialize_named(stream, s.lines));
		return true;
	}

	bool deserialize(std::istream& stream, StandardTime& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.type));
		RETURN_IF_NOT(deserialize_named(stream, s.time));
		RETURN_IF_NOT(deserialize_named(stream, s.timeRev));
		return true;
	}

	bool deserialize(std::istream& stream, Config& s)
	{
		RETURN_IF_NOT(deserialize_named(stream, s.Do1));
		RETURN_IF_NOT(deserialize_named(stream, s.Posle1));
		RETURN_IF_NOT(deserialize_named(stream, s.NumVar1));
		RETURN_IF_NOT(deserialize_named(stream, s.nvar1));
		return true;
	}

}
