#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/dll.hpp>

// =============================================================================

struct Point // verify
{
    std::string name;
    unsigned int dt;
    double lenght;
    double weight;
    int maxspeed;
};

struct Nitka // verify
{
    unsigned int StartTime;
    std::string type; 
    int priority;
    std::vector < Point > points;
};

struct Zapret // verify
{
    std::string name;
    int from, to;  
};

// =============================================================================

extern "C" __declspec(dllexport) bool zsNitkaWorkInit(bool, std::wstring) { return true; } // verify
extern "C" __declspec(dllexport) void zssetGid(std::vector < Nitka > &) {} // verify
extern "C" __declspec(dllexport) bool zsSaveState(std::wstring) { return true; } // verify
extern "C" __declspec(dllexport) bool zsLoadState(std::wstring) { return true; } // verify
extern "C" __declspec(dllexport) void zssetZapret(std::vector < Zapret > &) {} // verify
extern "C" __declspec(dllexport) void zssetZapretZ(int, std::vector < Zapret > &) {} // verify
extern "C" __declspec(dllexport) void zsclearZapret() {} // verify
extern "C" __declspec(dllexport) void zscalcZapretData() {} // verify
extern "C" __declspec(dllexport) void zssetConfig(int, int, std::vector < int >) {} // verify
extern "C" __declspec(dllexport) void zsNitkaWork(std::vector < Nitka > & dummy, std::vector < std::vector < Nitka > > & result); // verify

// =============================================================================