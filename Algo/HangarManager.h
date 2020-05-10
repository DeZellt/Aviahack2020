#pragma once

#include "json.h"

#include <unordered_map>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <vector>

#include "Point.h"

class Date {
public:
    static constexpr int SECONDS_IN_DAY = 86400;
    Date() = default;
    Date(int day, int month, int year);
    bool operator < (const Date& other) const;
    static Date ParseDate(std::string date);
    long long AsTimeStamp() const;


private:

    int day;
    int month;
    int year;
};

struct PlanePosition {
    long long width;
    long long height;
    long long x;
    long long y;
    std::string planeName;
    std::string companyName;
    std::string hangarName;
};

class Plane {
public:
    std::string name;
    int width;
    int height;
    int32_t serviceTime;

    bool operator == (const Plane& other) const;
};



enum TOFormat {
    D_CHECK,
    C_CHECK,
    REDELIVERY,
    PAINTING
};

class Company {
public:
    Date start;
    Date end;
    std::string name;
    double penalty;
};

class Contract {
public:
    int32_t id;
    std::string planeName;
    std::string companyName;
    TOFormat toFormat;

    int32_t planeAmount;
    int32_t minPlaneAmount;

    int32_t days;
    int32_t interval_start;
    int32_t interval_end;
};

struct Level {
    int32_t beginHeight;
    int32_t width;
    std::vector<TPoint> floor;
    std::vector<TPoint> ceiling;

    Level() = default;

    Level(int32_t beginHeight, int32_t width):
            beginHeight(beginHeight),
            width(width),
            floor(0),
            ceiling(0)
    {}

    // Методы для проверки постановки самолета на данный уровень

    bool canPutFloor(const Plane& pln) {
        if (floor.empty()) {
            return true;
        }
        return floor.back().x + pln.width <= width;
    }

    bool canPutCeil(const Plane& pln) {
        for (auto& point : floor) {
            if (width - (ceiling.empty() ? 0 : ceiling.back().x) < point.x
                && getHeight() - (ceiling.empty() ? 0 : ceiling.back().y) < point.y) {
                return false;
            }
        }
        return true;
    }

    // Методы putFloor и putCeil возвращают левый нижний край прямоугольника(самолета)
    // Для того, чтобы после добавления на уровень можно было добавить координаты самолета
    // В общий pull
    TPoint putFloor(const Plane& pln) {
        TPoint res(floor.empty() ? 0 : floor.back().x, beginHeight);
        floor.emplace_back((floor.empty() ? 0 : floor.back().x) + pln.width,
                           beginHeight + pln.height);
        return res;
    }

    TPoint putCeil(const Plane& pln) {
        TPoint res(width - (ceiling.empty() ? 0 : ceiling.back().x) - pln.width,
                   getHeight() - pln.height);
        ceiling.emplace_back(pln.height, width - (ceiling.empty() ? 0 : ceiling.back().x));
        return res;
    }

    int32_t getHeight() { return floor.empty() ? 0 : floor.front().y; }
};

class Hangar {
public:
    Hangar() = default;
    Hangar(std::string name, int width, int height);
    bool add(const Plane& plane);
    void updatePlanes(int32_t timePoint);
    std::vector<PlanePosition> getPositions();

    bool planePutLvl(Level& lvl, const Plane& pln);

    std::string name;
    int32_t width;
    int32_t height;
    std::vector<Plane> planes;
    std::vector<Level> levels;

private:

    class PlaneHashFunc {
    public:
        size_t operator()(const Plane& plane) const {
            return std::hash<std::string>()(plane.name) +
                   std::hash<int32_t >()(plane.height + plane.width);
        }
    };

    std::unordered_map<Plane, TPoint, PlaneHashFunc> points;

};



Company ReadCompany(const Json::Node&);
std::map<std::string, Company> ReadCompanies(const Json::Node&);


Plane ReadPlane(const Json::Node& node);
std::map<std::string, Plane> ReadPlanes(const Json::Node& node);

Company ReadCompany(const Json::Node& node);
std::map<std::string, Company> ReadCompanies(const Json::Node& node);

std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>> ReadPrices(const Json::Node& node);

std::vector<Contract> CreateContracts(const Json::Node& node,
                                      const std::map<std::string, Company>& companies,
                                      const std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>>& prices,
                                      const std::vector<Plane>& planes);

std::vector<Json::Document> ReadTables();

struct AlgorithmResponse{
    std::vector<std::vector<PlanePosition>> positions;
    long long income;
    long long fine;
};


AlgorithmResponse TheAlgorithm();