#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "visualize.hpp"

#define debugMode
//#define musicPlay

Plane::Plane(const std::string &newName, int newWidth, int newHeight){
    name = newName;
    width = newWidth;
    height = newHeight;
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

void drawAngars(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
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

    sf::RectangleShape angarRec;
    sf::RectangleShape planeRec;

    sf::Text text;
    /*
    text.setFont(wProp::font);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Red);
    text.setPosition(20, 30);
    */

    //количество ангаров
    int numberOfAngars = timeGrid[t].size();

    //минимальное расстояние между анагарми в пикселях в процентах от длины окна
    int minDistanceBetweenAngars = 5 * wProp::windowWidth / 100; //5%

    //подсчёт длины и ширины
    int totalAngarWidth = 0;
    int maxHeight = timeGrid[t][0].height;
    for(int i = 0; i < timeGrid[t].size(); i++){
        totalAngarWidth += timeGrid[t][i].width;

        if(timeGrid[t][i].height > maxHeight){
            maxHeight = timeGrid[t][i].height;
        }
    }

    //какая длинна и высота нужна в сумме для отрисовки ангаров с учетом расстояния между ними и до стен
    int totalActualWidth = (numberOfAngars + 1) * minDistanceBetweenAngars + totalAngarWidth;

    //ангары располагаются в ряд, поэтому учитывается максимальная высота ангара
    int totalActualHeight = (minDistanceBetweenAngars * 2) + maxHeight;

    //во сколько раз нужно всё сжать, если получилась слишком большая длина
    double widthCompressionRatio = static_cast<double>(wProp::windowWidth) / totalActualWidth;
    double heightCompressionRatio = static_cast<double>(wProp::windowHeight) / totalActualHeight;

    /*
    if(heightCompressionRatio > 1){
        heightCompressionRatio = 1;
    }
    */

    #ifdef debugMode
    std::cout << "widthCompressionRatio: " << widthCompressionRatio << std::endl;
    std::cout << "heightCompressionRatio: " << heightCompressionRatio << std::endl;
    #endif


    int lastRectanglePos = 0;
    //отрисовка ангаров в масштабе
    for(int i = 0; i < timeGrid[t].size(); i++){
        std::string angarName = timeGrid[t][i].name;
        int angarWidth = timeGrid[t][i].width;
        int angarHeight = timeGrid[t][i].height;

        sf::RectangleShape rec;
        rec.setSize(sf::Vector2f(angarWidth * widthCompressionRatio, angarHeight * heightCompressionRatio));
        
        int x, y;
        x = minDistanceBetweenAngars * widthCompressionRatio + lastRectanglePos;
        y = minDistanceBetweenAngars * heightCompressionRatio;

        lastRectanglePos = x + angarWidth * widthCompressionRatio;

        rec.setPosition(x, y);
        rec.setFillColor(sf::Color::Black);
        rec.setOutlineColor(sf::Color::Red);
        rec.setOutlineThickness(2);

        window.draw(rec);
    }
    
    //window.draw(text);
}


int main(){
    if(!loadFont(wProp::font, wProp::fontPath)){
        std::cerr << "------------------------------\n";
        std::cerr << "Error: can not load font from path \'" << wProp::fontPath << "\'\n";
        return 1;
    }

    std::vector<std::vector<Angar>> angarTimeGrid;
    timePoint t = 0;

    Plane p1("plane1", 10, 20);
    Plane p2("plane2", 30, 40);
    Plane p3("plane3", 10, 10);

    std::vector<Plane> pv;
    pv.push_back(p1);
    pv.push_back(p2);
    pv.push_back(p3);

    Angar a("sve", 80, 300, pv);
    Angar b("vso", 90, 200, pv);
    Angar c("xxx", 70, 150, pv);
    //Angar a("sve", 300, 80, pv);
    //Angar b("vso", 200, 90, pv);
    //Angar c("xxx", 150, 70, pv);

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


    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        window.clear();
        drawAngars(window, angarTimeGrid, t);
        window.display();
    }

    return 0;
}