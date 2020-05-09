#pragma once

#include "json.h"

#include <unordered_map>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <MacTypes.h>

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

class Plane {
public:
    std::string name;
    int32_t width;
    int32_t height;
    int32_t serviceTime;
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
    bool add(const Plane& plane);
    void updatePlanes(int32_t timePoint);
private:
    std::string name;
    int32_t width;
    int32_t height;
    std::vector<Plane> planes;

    class PlaneHashFunc {
    	size_t operator()(const Plane& plane) const {
    		return std::hash<std::string>()(plane.name) +
    		        std::hash<int32_t >()(plane.height + plane.width);
    	}
    };

    std::unordered_map<Plane, TPoint, PlaneHashFunc> points;

    bool planePutLvl(Level&, const Plane&);

};

Company ReadCompany(const Json::Node&);
std::map<std::string, Company> ReadCompanies(const Json::Node&);

class HangarManager {
public:
    HangarManager();
};