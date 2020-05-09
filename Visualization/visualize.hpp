#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

typedef size_t timePoint;

//window properties
namespace wProp{
    sf::Font font;
    const static std::string fontPath("./Fonts/lucida.ttf");

    const std::string windowName;
    const int windowWidth = 640;
    const int windowHeight = 480;
    const int windowFPS = 1;

    const static sf::Color windowBGColor(sf::Color::Black);
}

//plane color
namespace pColor{
    const static sf::Color S7(sf::Color(191, 214, 0, 255)); //салатовый
    const static sf::Color Aeroflot(sf::Color(42, 89, 157, 255)); //синий
    const static sf::Color UralAirlines(sf::Color(220, 26, 44, 255)); //бордовый
    const static sf::Color Pobeda(sf::Color(19, 160, 231, 255)); //голубой
    const static sf::Color Alrosa(sf::Color(69, 151, 215, 255)); //голубой
    const static sf::Color Utair(sf::Color(0, 53, 148, 255)); //глубокий синий
    const static sf::Color Rossiya(sf::Color(227, 19, 44, 255)); //красно-малиновый
    const static sf::Color Belavia(sf::Color(69, 92, 199, 255)); //голубой
}

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

//отрисовка ангаров и самолетов в зависиости от текущего дня
void drawAngars(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t);

//загрузка шрифта из файла
bool loadFont(sf::Font &font, const std::string &fontPath);