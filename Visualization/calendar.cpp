#include "calendar.hpp"

void Calendar::init(int newDay, int newMonth, int newYear){
    day = startDay = newDay;
    month = startMonth = newMonth-1;
    year = startYear = newYear;
}

std::string Calendar::getDate(int id){
    day = startDay;
    month = startMonth;
    year = startYear;

    std::pair<std::string, int> monthRes = getMonth(month);
    while(id + day > monthRes.second){
        //std::cout << "id: " << id << " day: " << day << " monthRes.second: " << monthRes.second << std::endl;
        id -= (monthRes.second - day);
        day = 0;
        if(++month > 11){
            month = 0;
            year++;
        }
        monthRes = getMonth(month);
    }

    day += id;

    return std::string(std::to_string(day) + " " + monthRes.first + " " + std::to_string(year));
}

std::pair<std::string, int> Calendar::getMonth(int x){
    switch(x){
        case 0:
            return std::make_pair("Jan", 31);
        case 1:
            return std::make_pair("Feb", 28);
        case 2:
            return std::make_pair("Mar", 31);
        case 3:
            return std::make_pair("Apr", 30);
        case 4:
            return std::make_pair("May", 31);
        case 5:
            return std::make_pair("Jun", 30);
        case 6:
            return std::make_pair("Jul", 31);
        case 7:
            return std::make_pair("Aug", 30);
        case 8:
            return std::make_pair("Sep", 30);
        case 9:
            return std::make_pair("Oct", 31);
        case 10:
            return std::make_pair("Nov", 30);
        case 11:
            return std::make_pair("Dec", 31);
        default:
            return std::make_pair("WrongMonth", 0);
    }
}