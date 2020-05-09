#pragma once
#include <string>
#include <vector>

namespace wProp{
    sf::Font font;

    const std::string windowName;
    const int windowWidth = 640;
    const int windowHeight = 480;
    const int windowFPS = 10;
};

typedef size_t timePoint;

class Plane{
public:
    Plane(){};
    Plane(const std::string &newName, int newWidth, int newHeight);
    Plane(const Plane &p);

    std::string name;
    int width, height;
};

class Angar{
public:
    Angar(){};
    Angar(const std::string &newName, int newWidth, int newHeight, const std::vector<Plane> &newPlanes);

    std::string name;
    int width, height;
    std::vector<Plane> planes;
};

//окно для рендеринга, массив с днями, в каждом из которых находятся ангары в определенном состоянии
void drawAngars(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t);