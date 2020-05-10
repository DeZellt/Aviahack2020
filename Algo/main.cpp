#include <iostream>
#include <fstream>
#include "json.h"
#include "HangarManager.h"

Json::Node ResponseToJson(const AlgorithmResponse& resp) {
    std::map<std::string, Json::Node> response_blank;
    response_blank["income"] = Json::Node(resp.income);
    response_blank["fine"] = Json::Node(resp.fine);
    std::vector<Json::Node> positions;
    for (const auto& day : resp.positions) {
        std::vector<Json::Node> day_positions;
        for (const auto& pos : day) {
            std::map<std::string, Json::Node> pos_data;
            pos_data["width"] = Json::Node(pos.width);
            pos_data["height"] = Json::Node(pos.height);
            pos_data["x"] = Json::Node(pos.x);
            pos_data["y"] = Json::Node(pos.y);
            pos_data["planeName"] = Json::Node(pos.planeName);
            pos_data["companyName"] = Json::Node(pos.companyName);
            pos_data["hangarName"] = Json::Node(pos.hangarName);
            day_positions.push_back(Json::Node(pos_data));
        }
        positions.push_back(Json::Node(day_positions));
    }
    response_blank["positions"] = Json::Node(positions);
    return Json::Node(response_blank);
}

int main() {
    AlgorithmResponse positions = TheAlgorithm();
    Json::Document result = Json::Document(ResponseToJson(positions));
    std::ofstream os("../output.json");
    Json::Print(result, os);
    os.close();
    return 0;
}
