#include <exception>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/dll.hpp>

#include "controller/controller.hpp"

using Controller = solution::plugin::Controller;

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

extern "C" __declspec(dllexport) void zsNitkaWork(
    std::vector < Nitka > & dummy, std::vector < std::vector < Nitka > > & result) // verify
{
    std::unordered_map < std::string, std::string > converter;

    std::fstream fin("converter.txt", std::ios::in);

    for (int i = 0; i < 28; ++i)
    {
        std::string key, value;

        fin >> key >> value;

        converter[key] = value;
    }

    Controller controller;

    controller.run();

    std::vector < Nitka > gid;
    gid.reserve(controller.gid().size());
    
    for (const auto & thread : controller.gid())
    {
        Nitka nitka = { 1602495664, thread.first, 1, {} };

        for (const auto & record : thread.second)
        {
            Point point = { converter[record.first], static_cast < unsigned int > (record.second) * 60U, 0.0, 0.0, 0 };

            nitka.points.emplace_back(std::move(point));
        }

        gid.push_back(std::move(nitka));
    }

    result.push_back(std::move(gid));
}

// =============================================================================