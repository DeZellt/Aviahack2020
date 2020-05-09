#pragma once

#include "json.h"

#include <unordered_map>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <vector>

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
    double width;
    double height;
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

class Hangar {
public:
    void add(Plane plane);
    bool canAdd(Plane plane);
    void clear(int32_t timePoint); // Удаляет из std::vector<Plane> самолеты у которых servicTime <= timePoint
private:
    std::string name;
    int32_t width;
    int32_t height;
    std::vector<Plane> planes;
    //TODO: Написать алгоритм для проверки на добавление самолета и проверки
    //сколько места осталось в ангаре вроде статья на хабре
};

Company ReadCompany(const Json::Node&);
std::map<std::string, Company> ReadCompanies(const Json::Node&);

class HangarManager {
public:
    HangarManager();

};