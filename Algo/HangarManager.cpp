#include <fstream>
#include "HangarManager.h"


bool Plane::operator==(const Plane &other) const {
    return name == other.name && width == other.width && height == other.height;
}

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
    result.width = node.AsMap().at("width").AsDouble() * 1000 + 1;
    result.height = node.AsMap().at("length").AsDouble() * 1000 + 1;
    result.serviceTime = 0;
    return result;
}

std::map<std::string, Plane> ReadPlanes(const Json::Node& node) {
    std::map<std::string, Plane> result;
    for (const Json::Node& cur_plane : node.AsArray()) {
        Plane plane = ReadPlane(cur_plane);
        result[plane.name] = plane;
    }
    return result;
}

Hangar ReadHangar(const Json::Node& node) {
    Hangar result( node.AsMap().at("name").AsString(),
                 node.AsMap().at("width").AsLong() * 1000,
                node.AsMap().at("depth").AsLong() * 1000);
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
        const std::map<std::string, Plane>& planes) {

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

std::vector<PlanePosition> GetPositions(const std::map<std::string, Hangar>& hangars) {
    std::vector<PlanePosition> result;
    for (auto [name, hangar] : hangars) {
        auto positions = hangar.getPositions();
        result.insert(result.end(), positions.begin(), positions.end());
    }

    return result;
}


Hangar::Hangar(std::string name, int width, int height)
: name(name), width(width), height(height) {//width и height для уровней меняются местами
    int cur_width = width;
    while (cur_width > 80000) {
        levels.push_back(Level(80000, height));
        cur_width -= 80000;
    }
    if (cur_width != 0) {
        levels.push_back(Level(cur_width, height));
    }
}

void Hangar::Pack() {
    points.clear();
    for (const auto& plane : planes) {
        for (auto& level : levels) {
            if (level.canPutFloor(plane)) {
                points[plane] = level.putFloor(plane);
                break;
            } else if (level.canPutCeil(plane)) {
                points[plane] = level.putCeil(plane);
                break;
            }
        }
    }
}

void Hangar::updatePlanes(int32_t timePoint) {
    planes = std::vector<Plane>(planes.begin(), std::remove_if(planes.begin(), planes.end(), [timePoint] (const Plane& plane) {
        return plane.serviceTime > timePoint;
    }));
    Pack();
}

bool Hangar::add(const Plane &plane) {
    for (auto& level : levels) {
        if (level.canPutFloor(plane)) {
            points[plane] = level.putFloor(plane);
            return true;
        } else if (level.canPutCeil(plane)) {
            points[plane] = level.putCeil(plane);
            return true;
        }
    }
    return false;
}

std::vector<PlanePosition> Hangar::getPositions() {
    std::vector<PlanePosition> result;
    for (auto& pair : points) {
        result.push_back(PlanePosition{.width = pair.first.width, .height = pair.first.height,
                                       .x = pair.second.x, .y = pair.second.y, .planeName = pair.first.name,
                                       .companyName = "", .hangarName = name});
    }
    return result;
}







std::vector<std::vector<PlanePosition>> TheAlgorithm() {

    std::vector<Json::Document> tables = ReadTables();

    std::map<std::string, Hangar> hangars = ReadHangars(tables[0].GetRoot());
    std::map<std::string, Company> companies = ReadCompanies(tables[1].GetRoot());
    std::map<std::pair<TOFormat, std::string>, std::unordered_map<std::string, int>> prices = ReadPrices(tables[3].GetRoot()); // {TO, Plane} -> {{Hangar1, Price1}, ... {HangarN, PriceN}}
    std::map<std::string, Plane> planes = ReadPlanes(tables[2].GetRoot());
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



    long long summaryIncome = 0;
    std::vector<std::vector<PlanePosition>> result;


    for (long long i = min_time; i <= max_time; ++i) {
        for (auto& pair : hangars) {
            pair.second.updatePlanes(i);
        }
        for (auto& contract : contracts) {
            TOFormat format = contract.toFormat;
            std::string plane_name = contract.planeName;

            std::vector<std::pair<std::string, int>> hang_name_and_price;
            for (const auto &pair : prices.at({format, plane_name})) {
                hang_name_and_price.push_back(pair);
            }
            std::sort(hang_name_and_price.begin(), hang_name_and_price.end(), [](const auto &lhs, const auto &rhs) {
                return lhs.second < rhs.second;
            });

            for (const auto &pair : hang_name_and_price) {
                Plane current_plane = planes[plane_name];
                current_plane.serviceTime = contract.days;
                if (contract.planeAmount > 0 && hangars[pair.first].add(current_plane)) {
                    contract.planeAmount--;
                    summaryIncome += prices[{format, plane_name}][pair.first];
                }
            }
        }
        std::vector<PlanePosition> temp_result;
        temp_result = GetPositions(hangars);
        result.push_back(temp_result);
    }

    return result;
}