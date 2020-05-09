#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "visualize.hpp"

#define debugMode
//#define musicPlay

Plane::Plane(const std::string &newName, int newWidth, int newHeight, int newX, int newY){
    name = newName;
    width = newWidth;
    height = newHeight;
    x = newX;
    y = newY;
}

Plane::Plane(const Plane &p){
    name = p.name;
    width = p.width;
    height = p.height;
}

Angar::Angar(const std::string &newName, int newWidth, int newHeight, const std::vector<Plane> &newPlanes){
    name = newName;
    width = newWidth;
    height = newHeight;
    planes = newPlanes;
}

bool loadFont(sf::Font &font, const std::string &fontPath){
    if(!font.loadFromFile(fontPath)){
        return false;
    }
    return true;
}

double calculateWidthCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    //количество ангаров
    int numberOfAngars = timeGrid[t].size();

    //подсчёт длины
    int totalAngarWidth = 0;
    for(int i = 0; i < timeGrid[t].size(); i++){
        totalAngarWidth += timeGrid[t][i].width;
    }

    //какая длинна и высота нужна в сумме для отрисовки ангаров с учетом расстояния между ними и до стен
    int totalActualWidth = (numberOfAngars + 1) * aProp::minDistanceBetweenAngars + totalAngarWidth;

    return static_cast<double>(wProp::windowWidth) / totalActualWidth;
}

double calculateHeightCompressionRatio(const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    //подсчёт ширины
    int maxHeight = timeGrid[t][0].height;
    for(int i = 0; i < timeGrid[t].size(); i++){
        if(timeGrid[t][i].height > maxHeight){
            maxHeight = timeGrid[t][i].height;
        }
    }

    //ангары располагаются в ряд, поэтому учитывается максимальная высота ангара
    int totalActualHeight = aProp::minDistanceBetweenAngars * 2 + maxHeight;

    return static_cast<double>(wProp::windowHeight) / totalActualHeight;
}

int calculateAngarFontSize(const sf::RectangleShape &rec, const Angar &angar, int distanceToUpperBound){
    int angarActualWidth = rec.getSize().x;
    int angarNameLength = angar.name.length();

    return std::min(angarActualWidth / angarNameLength, distanceToUpperBound - aProp::angarOutlineThickness - aProp::angarTextPadding);
}

Theme::Theme(){
    curTheme = Theme::White;
}

Theme::Theme(int theme){
    Theme::applyTheme(theme);
}

void Theme::applyTheme(int theme){
    switch(theme){
        case Theme::White:{
            windowBGColor = sf::Color(253, 246, 227, 255);
            angarOutlineColor = sf::Color::Black;
            angarInsideColor = windowBGColor;
            break;
        }
        case Theme::Dark:{
            windowBGColor = sf::Color(49, 54, 55, 255);
            angarOutlineColor = sf::Color::White;
            angarInsideColor = windowBGColor;
            break;
        }
        default:{
            std::cerr << "Error in applying theme: incorrect theme\n";
            break;
        }
    }
    curTheme = theme;
}

void drawTest(sf::RenderWindow &window){
    sf::RectangleShape rec;

    rec.setSize(sf::Vector2f(50, 200));

    rec.setPosition(0,0);
    rec.setFillColor(pColor::S7);
    window.draw(rec);

    rec.setPosition(50,0);
    rec.setFillColor(pColor::Aeroflot);
    window.draw(rec);
    
    rec.setPosition(100,0);
    rec.setFillColor(pColor::UralAirlines);
    window.draw(rec);
    
    rec.setPosition(150,0);
    rec.setFillColor(pColor::Pobeda);
    window.draw(rec);
    
    rec.setPosition(200,0);
    rec.setFillColor(pColor::Alrosa);
    window.draw(rec);
    
    rec.setPosition(250,0);
    rec.setFillColor(pColor::Utair);
    window.draw(rec);
    
    rec.setPosition(300,0);
    rec.setFillColor(pColor::Rossiya);
    window.draw(rec);
    
    rec.setPosition(350,0);
    rec.setFillColor(pColor::Belavia);
    window.draw(rec);
}

void drawAll(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    try{
        timeGrid.at(t);
    }catch(...){
        std::cerr << "------------------------------\n";
        std::cerr << "Error in angar drawing function: index out of range\n";
        std::cerr << "Vector size: " << timeGrid.size() << " index: " << t << "\n";
        return;
    }

    if(timeGrid[t].empty()){
        return;
    }

    //во сколько раз нужно всё сжать, если получилась слишком большая длина
    double widthCompressionRatio = calculateWidthCompressionRatio(timeGrid, t);
    double heightCompressionRatio = calculateHeightCompressionRatio(timeGrid, t);

    #ifdef debugMode
    std::cout << "widthCompressionRatio: " << widthCompressionRatio << std::endl;
    std::cout << "heightCompressionRatio: " << heightCompressionRatio << std::endl;
    #endif

    sf::Text text;
    text.setFont(gProp::font);

    int lastRectanglePos = 0;
    //отрисовка ангаров в масштабе
    for(int i = 0; i < timeGrid[t].size(); i++){
        int angarWidth = timeGrid[t][i].width * widthCompressionRatio;
        int angarHeight = timeGrid[t][i].height * heightCompressionRatio;

        sf::RectangleShape angarRec;
        angarRec.setSize(sf::Vector2f(angarWidth, angarHeight));
        
        int x, y;
        x = aProp::minDistanceBetweenAngars * widthCompressionRatio + lastRectanglePos;
        y = aProp::minDistanceBetweenAngars * heightCompressionRatio;


        lastRectanglePos = x + angarWidth;

        angarRec.setPosition(x, y);
        angarRec.setFillColor(gProp::theme.windowBGColor);
        angarRec.setOutlineColor(gProp::theme.angarOutlineColor);
        angarRec.setOutlineThickness(aProp::angarOutlineThickness);

        window.draw(angarRec);

        int angarFontSize = calculateAngarFontSize(angarRec, timeGrid[t][i], y);
        text.setString(timeGrid[t][i].name);
        text.setCharacterSize(angarFontSize);
        text.setFillColor(gProp::theme.angarOutlineColor);
        text.setPosition(x, y - angarFontSize - aProp::angarOutlineThickness - aProp::angarTextPadding);
        
        window.draw(text);
    }
    
}


int main(){
    if(!loadFont(gProp::font, gProp::fontPath)){
        std::cerr << "------------------------------\n";
        std::cerr << "Error: can not load font from path \'" << gProp::fontPath << "\'\n";
        return 1;
    }

    gProp::theme.applyTheme(Theme::White);

    std::vector<std::vector<Angar>> angarTimeGrid;
    timePoint t = 0;

    Plane p1("plane1", 10, 20, 0, 0);
    Plane p2("plane2", 30, 40, 15, 0);
    Plane p3("plane3", 10, 10, 0, 50);

    std::vector<Plane> pv;
    pv.push_back(p1);
    pv.push_back(p2);
    pv.push_back(p3);

    Angar a("SVO", 80, 300, pv);
    Angar b("VKO", 90, 200, pv);
    Angar c("DME", 70, 150, pv);

    std::vector<Angar> v;
    v.push_back(a);
    v.push_back(b);
    v.push_back(c);
    
    angarTimeGrid.push_back(v);

    sf::RenderWindow window(sf::VideoMode(wProp::windowWidth, wProp::windowHeight), wProp::windowName);
    window.setFramerateLimit(wProp::windowFPS);

    #ifdef musicPlay
    sf::Music music;
    if(!music.openFromFile("./Music/Nyan.wav")){
        return 0;
    }
    music.play();
    #endif


    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear(gProp::theme.windowBGColor);
        drawAll(window, angarTimeGrid, t);
        //drawTest(window);
        window.display();
    }

    return 0;
}