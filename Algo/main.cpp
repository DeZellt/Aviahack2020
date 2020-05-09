#include <iostream>
#include <fstream>
#include "json.h"
#include "HangarManager.h"

int main() {
    setlocale(LC_ALL, "Russian");

    std::ifstream file("table.json");
    Json::Document doc = Json::Load(file);
    Json::Print(doc, std::cout);
    HangarManager manager(doc);
    return 0;
}
