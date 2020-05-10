#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "calendar.hpp"

typedef unsigned timePoint;

class Theme{
public:
    Theme();
    Theme(int theme);
    void applyTheme(int theme);

    sf::Color windowBGColor, angarOutlineColor, angarInsideColor;
    int curTheme;
    enum {Light, Dark};
};

class Button{
public:
    Button(){};
    Button(int x, int y, int width, int height, const std::string &s);
    
    void init(int x, int y, int width, int height, const std::string &s);

    void switchMode();

    bool isActive() const;

    int x, y;
    int width, height;
    std::string title;
    bool active;
};


//global properties
namespace gProp{
    Theme theme;
    sf::Font font;
    Calendar calendar;
    Button showCompaniesButton;
    const static std::string fontPath("./Fonts/courierNew.ttf");
}

//window properties
namespace wProp{
    const static std::string windowName("Case: S7 Technics");
    const int windowWidth = 1000;
    const int windowHeight = 500;
    const int windowFPS = 15;

    const int topBarSize = 38;
}

//planes colors
namespace pColor{
    const static sf::Color S7(sf::Color(80, 158, 47, 255));
    const static sf::Color Aeroflot(sf::Color(153, 93, 199, 255));
    const static sf::Color UralAirlines(sf::Color(155, 34, 66, 255));
    const static sf::Color Pobeda(sf::Color(210, 38, 48, 255));
    const static sf::Color Alrosa(sf::Color(45, 204, 211, 255));
    const static sf::Color Utair(sf::Color(0, 127, 163, 255));
    const static sf::Color Rossiya(sf::Color(245, 182, 205, 255));
    const static sf::Color Belavia(sf::Color(240, 233, 145, 255));
}

//angars properties
namespace aProp{
    const int minDistanceBetweenAngars = 1 * wProp::windowWidth / 100; // 1 % of window width
    const int angarOutlineThickness = 2;
    const int angarTextPadding = 2;
}

//planes properties
namespace pProp{
    const int planeOutlineThickness = 1;
    const int planeTextPadding = 1;
}

//date properties
namespace dProp{
    const int dateTextPadding = 10;
}

//button properies
namespace bProp{
    const int buttonOutlineThickness = 1;
    const int buttonTextPadding = 3;
}


class Plane{
public:
    Plane(){};
    Plane(const std::string &newName, const std::string &newCompanyName, 
        int newWidth, int newHeight, int newX, int newY);
    Plane(const Plane &p);

    std::string name;
    std::string companyName;
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


//загрузка шрифта из файла
bool loadFont(sf::Font &font, const std::string &fontPath);

//коэффициенты сжатия по горизонтали и вертикали
double calculateWidthCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t);
double calculateHeightCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t);

//вычисление размера шрифта для названий ангара и самолёта 
int calculateAngarFontSize(const sf::RectangleShape &rec, const Angar &a, int distanceToUpperBound);
int calculatePlaneFontSize(const sf::RectangleShape &rec, const Plane &p, const std::string &str);

//отрисовка ангаров и самолётов
void drawAll(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t);
void drawAngarsAndPlanes(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid,
    timePoint t, double widthCompressionRatio, double heightCompressionRatio);
void drawPlanes(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid,
    timePoint t, int i, double widthCompressionRatio, double heightCompressionRatio,
    int angarX, int angarY);

//отрисовка даты
void drawDate(sf::RenderWindow &window, timePoint t);

//отрисовка кнопки
void drawButton(sf::RenderWindow &window, const Button &button);
//попали ли внутрь кнопки
bool clickInsideButton(sf::Vector2i &windowPos, Button &button, sf::Vector2i &click);

//определение цвета самолёта
sf::Color determinePlaneColor(const std::string &planeCompanyName);
