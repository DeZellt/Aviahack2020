#include <fstream>
#include "HangarManager.h"

Date::Date(int day, int month, int year)
: day(day), month(month), year(year) {}

bool Date::operator < (const Date &other) const {
    return std::tie(year, month, day) < std::tie(other.year, other.month, other.day);
}

long long Date::AsTimeStamp() const {
    std::tm t;
    t.tm_sec  = 0;
    t.tm_min  = 0;
    t.tm_hour = 0;
    t.tm_mday = day;
    t.tm_mon  = month - 1;
    t.tm_year = year - 1900;
    t.tm_isdst = 0;
    return mktime(&t) / SECONDS_IN_DAY;
}

TOFormat StringToFormat(std::string str) {
    std::map<std::string, TOFormat> map = {
            {"C-Check", TOFormat::C_CHECK},
            {"D-Check", TOFormat::D_CHECK},
            {"Painting", TOFormat::PAINTING},
            {"Redelivery", TOFormat::REDELIVERY}
    };
    return map.at(str);
};

Date Date::ParseDate(std::string date) {
    std::stringstream ss(date);
    std::string cur_part;
    Date result;
    getline(ss, cur_part, '.');
    result.month = std::stoll(cur_part);
    getline(ss, cur_part, '.');
    result.day = std::stoll(cur_part);
    getline(ss, cur_part);
    result.year = std::stoll(cur_part);
    return result;
}

Company ReadCompany(const Json::Node& node) {
    Company result;
    result.start = Date::ParseDate(node.AsMap().at("startTO").AsString());
    result.end = Date::ParseDate(node.AsMap().at("endTO").AsString());
    result.name = node.AsMap().at("company").AsString();
    result.penalty = std::stod(node.AsMap().at("factor").AsString());
    return result;
}

std::map<std::string, Company> ReadCompanies(const Json::Node& node) {
    std::map<std::string, Company> result;
    for (const Json::Node& cur_comp : node.AsArray()) {
        Company temp = ReadCompany(cur_comp);
        result[temp.name] = temp;
    }
    return result;
}

std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>> ReadPrices(const Json::Node& node) {
    std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>> result;
    for (const Json::Node& cur_price : node.AsArray()) {
        TOFormat format;
        std::string plane_name;
        std::unordered_map<std::string, int> hangar_prices;
        for(const auto& kv : cur_price.AsMap()) {
            if (kv.first == "name") {
                plane_name = kv.second.AsString();
            } else if (kv.first == "TO") {
                format = StringToFormat(kv.second.AsString());
            } else if (!kv.second.IsNull()) {
                hangar_prices[kv.first] = kv.second.AsDouble();
            }
        }
        result[{format, plane_name}] = hangar_prices;
    }
    return result;
}

Plane ReadPlane(const Json::Node& node) {
    Plane result;
    result.name = node.AsMap().at("name").AsString();
    result.width = node.AsMap().at("width").AsDouble();
    result.height = node.AsMap().at("length").AsDouble();
    result.serviceTime = 0;
    return result;
}

std::vector<Plane> ReadPlanes(const Json::Node& node) {
    std::vector<Plane> result;
    for (const Json::Node& cur_plane : node.AsArray()) {
        result.push_back(ReadPlane(cur_plane));
    }
    return result;
}

Hangar ReadHangar(const Json::Node& node) {
    Hangar result;
    result.name = node.AsMap().at("name").AsString();
    result.width = node.AsMap().at("width").AsLong() * 1000;
    result.height = node.AsMap().at("depth").AsLong() * 1000;
    return result;
}

std::map<std::string, Hangar> ReadHangars(const Json::Node& node) {
    std::map<std::string, Hangar> result;
    for (const auto& cur_node : node.AsArray()) {
        Hangar cur = ReadHangar(cur_node);
        result[cur.name] = cur;
    }
    return result;
}

std::vector<Contract> CreateContracts(const Json::Node& node,
        const std::map<std::string, Company>& companies,
        const std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>>& prices,
        const std::vector<Plane>& planes) {

    std::vector<Contract> result;
    for (const Json::Node& cur_contract : node.AsArray()) {
        Contract cur;
        cur.id = result.size();
        cur.companyName = cur_contract.AsMap().at("company").AsString();
        cur.planeName = cur_contract.AsMap().at("type").AsString();
        cur.minPlaneAmount = cur_contract.AsMap().at("min").IsNull() ? 0 : cur_contract.AsMap().at("min").AsDouble();
        cur.toFormat = StringToFormat(cur_contract.AsMap().at("TO").AsString());
        cur.planeAmount = cur_contract.AsMap().at("needs").AsDouble();
        cur.days = cur_contract.AsMap().at("length").AsDouble();
        cur.interval_start = companies.at(cur.companyName).start.AsTimeStamp();
        cur.interval_end = companies.at(cur.companyName).end.AsTimeStamp() - cur.days;
        result.push_back(cur);
    }
    return result;
}

std::vector<Json::Document> ReadTables() {
    std::vector<Json::Document> result;
    for (int i = 1; i <= 5; ++i) {
        std::ifstream file("../table" + std::to_string(i) + ".json");
        Json::Document doc = Json::Load(file);
        file.close();
        result.push_back(doc);
    }
    return result;
}

HangarManager::HangarManager() {

    std::vector<Json::Document> tables = ReadTables();


    std::map<std::string, Hangar> hangars = ReadHangars(tables[0].GetRoot());
    std::map<std::string, Company> companies = ReadCompanies(tables[1].GetRoot());
    std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>> prices = ReadPrices(tables[3].GetRoot()); // {TO, Plane} -> {{Hangar1, Price1}, ... {HangarN, PriceN}}
    std::vector<Plane> planes = ReadPlanes(tables[2].GetRoot());
    std::vector<Contract> contracts = CreateContracts(tables[4].GetRoot(), companies, prices, planes);


    std::sort(contracts.begin(), contracts.end(), [&prices] (const Contract& lhs, const Contract& rhs) {
        return *std::max_element(prices.at({lhs.toFormat, lhs.planeName}).begin(), prices.at({lhs.toFormat, lhs.planeName}).end())
               < *std::max_element(prices.at({rhs.toFormat, rhs.planeName}).begin(), prices.at({rhs.toFormat, rhs.planeName}).end());
    });

    std::stable_sort(contracts.begin(), contracts.end(), [&companies] (const Contract& lhs, const Contract& rhs) {
        return lhs.interval_start < rhs.interval_start;
    });

    long long min_time = std::min_element(companies.begin(), companies.end(), [] (const auto& lhs, const auto& rhs) {
        return lhs.second.start < rhs.second.start;
    })->second.start.AsTimeStamp();

    long long max_time = std::max_element(companies.begin(), companies.end(), [] (const auto& lhs, const auto& rhs) {
        return lhs.second.end < rhs.second.end;
    })->second.end.AsTimeStamp();


    std::vector<std::vector<PlanePosition>> result;

    int contractCounter = 0;
    for (long long i = min_time; i <= max_time; ++i) {
        TOFormat format = contracts[contractCounter].toFormat;
        std::string plane_name = contracts[contractCounter].planeName;
        std::vector<std::pair<std::string, int>> hang_name_and_price;
        for (const auto& pair : prices.at({format, plane_name})) {
            hang_name_and_price.push_back(pair);
        }
        std::sort(hang_name_and_price.begin(), hang_name_and_price.end(), [] (const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });
        for (const auto& pair : hang_name_and_price) {
            if ()
        }
    }

}