#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <SFML/Graphics.hpp>

class Calendar{
public:
    Calendar(){};
    void init(int newDay, int newMonth, int newYear);

    std::string getDate(int id);

private:
    int startDay, startMonth, startYear;
    int day, month, year;

    std::pair<std::string, int> getMonth(int x);
};