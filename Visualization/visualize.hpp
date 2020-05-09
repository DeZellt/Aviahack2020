#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

typedef size_t timePoint;

class Theme{
public:
    Theme();
    Theme(int theme);
    void applyTheme(int theme);

    sf::Color windowBGColor, angarOutlineColor, angarInsideColor;
    int curTheme;
    enum {White, Dark};
};


//global properties
namespace gProp{
    Theme theme;
    sf::Font font;
    const static std::string fontPath("./Fonts/lucida.ttf");
}


//window properties
namespace wProp{
    const static std::string windowName("Case: S7 Technics");
    const int windowWidth = 640;
    const int windowHeight = 480;
    const int windowFPS = 1;

    //const static sf::Color windowBGColor(sf::Color(49, 54, 55, 255));
}


//angars & planes colors
namespace pColor{
    const static sf::Color S7(sf::Color(80,  158, 47, 255));
    const static sf::Color Aeroflot(sf::Color(81, 45, 109, 255));
    const static sf::Color UralAirlines(sf::Color(155, 34, 66, 255));
    const static sf::Color Pobeda(sf::Color(210, 38, 48, 255));
    const static sf::Color Alrosa(sf::Color(45, 204, 211, 255));
    const static sf::Color Utair(sf::Color(0, 127, 163, 255));
    const static sf::Color Rossiya(sf::Color(245, 182, 205, 255));
    const static sf::Color Belavia(sf::Color(240, 233, 145, 255));
}


//angar properties
namespace aProp{
    const int minDistanceBetweenAngars = 2 * wProp::windowWidth / 100; // 2 % of window width
    const int angarOutlineThickness = 2;
    const int angarTextPadding = 2;
}


class Plane{
public:
    Plane(){};
    Plane(const std::string &newName, int newWidth, int newHeight, int newX, int newY);
    Plane(const Plane &p);

    std::string name;
    int width, height;
    int x, y;
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
void drawAll(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t);

//загрузка шрифта из файла
bool loadFont(sf::Font &font, const std::string &fontPath);

//коэффициенты сжатия по горизонтали и вертикали
double calculateWidthCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t);
double calculateHeightCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t);

//вычисление размера шрифта для названия ангара
int calculateAngarFontSize(const sf::RectangleShape &rec, const Angar &a, int distanceToUpperBound);