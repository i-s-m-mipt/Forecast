#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <locale>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <time.h>

#ifdef _WIN32 
#if defined LIBRARY_EXPORTS || defined _USRDLL
#define LIBRARY_API __declspec(dllexport)
#else
#define LIBRARY_API __declspec(dllimport)
#endif
#else
#define LIBRARY_API
#endif
/*! Станционный путь или перегон, описанный внутри поездной нитки
 */
struct Point {
	std::string name;			// Имена тех объектов, которые поезд должен посетить в том же порядке, в котором он их должен посетить. Обе конечные станции также здесь
								// указаны. Данные строки у меня идут в русской локали в кодировке UTF-8. Мне так удобно, но если неприемлемо - могу переделать на "широкие"
								// строки wstring.
	unsigned int dt;			// Сколько секунд поезд должен находиться на объекте (заказ времени на погрузку-разгрузку и т.д.)
	double lenght;				// Длина поезда [метры] на объекте (если отличается от предыдущего участка, то изменение произошло на этом участке)
	double weight;				// Вес поезда [тонн] (если отличается от предыдущего участка,то изменение произошло на этом участке)
	int maxspeed;				// Максимальная скорость
	bool needOnlyThisWay;		// Если указан станционный путь, то ДА, если нужен именно этот путь или НЕТ если подойдет любой станционный путь
};
/*! нитка, возвращаемая нитка должна быть описана полностью, входная - только ключевые станционные пути и/или перегоны
 */
struct Nitka
{
	unsigned int StartTime;		// Желаемое время подачи поезда на первый объект (то есть объект name[0]), секунды с 00:00 1 января 1970г.
	std::string type;			// Тип 
	int priority;				// Приоритет

	std::vector<Point> points; //массив станционных путей или перегонов, может быть как полным, так и включать только ключевые точки
};
/*! нитка исполненного движения
 */
struct NitkaID : public Nitka
{
	unsigned int idStartTime;		// Время начала исполненного движения
	std::vector<Point> idPoints;	// Исполненное движение по маршруту points
	double idLastPosition;			// Положение на последнем (idPoints.back()) участке [0-1]
	unsigned int pgStartTime;		// Время начала прогнозного движения
	std::vector<Point> pgPoints;	// Прогнозное движение по маршруту points
};
/*! аншлаг, структура может хранить или аншлаг с полным запретом движения, или аншлаг ограничения по скорости
 */
struct Zapret
{
	std::string name;			// имя объекта
	int from, to;				// с какого по какой момент времени включительно запрещено посещение объекта. Если запрет постоянный, то from=-1 to=-1
	int maxspeed;				// максимальная скорость (если <= 0 запрет движения, > 0 ограничения скорости)
};
/*! ребро направленного графа ЖД
 */
struct Line
{
	enum LineDirection : int	// возможные направления ребра графа ЖД
	{
		Unknown = -1,
		Direction0,
		Direction1,
	};
	std::string srcNode;	//название начальной ноды графа
	std::string dstNode;	//название конечной ноды графа
	LineDirection srcDirection; //направление начальной ноды графа
	LineDirection dstDirection; //направление конечной ноды графа
};
/*! стандартное время прохождения ноды конкретным типом поезда
 */
struct StandardTime
{
	std::string type; //тип поезда
	int time; //время прохождения в прямом направлении
	int timeRev;//время прохождения в обратном направлении
};
/*! вершина направленного графа ЖД, станционный путь или перегон.
 */
struct Node
{
	std::string name; // имя объекта
	int dist_size;	//размер в метрах
	std::vector<StandardTime> standard_times; //массив стандарных времен прохождения этой ноды
};
/*! граф ЖД 
 */
struct Graph
{
	std::vector<Node> nodes; // вершины графа
	std::vector<Line> lines; // ребра графа
};
/*! настройки алгоритмов
 */
struct Config {
	int Do1;	//интервал времени времени [сек] освобождения пути перед поездом
	int Posle1; //интервал времени времени [сек] освобождения пути после поезда
	std::vector<int> NumVar1;	// число выдаваемых вариантов по каждому поезду
	int nvar1;	//общее число выдаваемых вариантов
};

extern "C" LIBRARY_API const char* aimName();

extern "C" LIBRARY_API const char* aimVersion();

extern "C" LIBRARY_API int aimProtocolVersion();

extern "C" LIBRARY_API const char* aimInit(const char* data);

extern "C" LIBRARY_API const char* aimWork(const char* data);

extern "C" LIBRARY_API const char* aimStartWork(const char* data, void(*dataready)(void));

extern "C" LIBRARY_API const char* aimGetResult(const char* data);

extern "C" LIBRARY_API void aimClose();
