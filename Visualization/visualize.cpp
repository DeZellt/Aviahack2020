#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "visualize.hpp"

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

void drawAngars(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    try{
        timeGrid.at(t);
    }catch(...){
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

    //минимальное расстояние между анагарми в процентах от всего места в окне
    int minDistanceBetweenAngars = 5 * wProp::windowWidth / 100; //5%

    //подсчёт суммарной длины и ширины
    int totalAngarWidth = 0;
    int totalAngarHeight = 0;
    for(int i = 0; i < timeGrid[t].size(); i++){
        totalAngarWidth += timeGrid[t][i].width;
        totalAngarHeight += timeGrid[t][i].height;
    }

    //отрисовка ангаров в масштабе
    for(int i = 0; i < timeGrid[t].size(); i++){
        std::string angarName = timeGrid[t][i].name;
        int angarWidth = timeGrid[t][i].width;
        int angarHeight = timeGrid[t][i].height;

        sf::RectangleShape rec;
        rec.setSize(sf::Vector2f(wProp::windowWidth/4, wProp::windowHeight/4));
        //rec.setPosition();

        window.draw(rec);
    }
    
    window.draw(text);
}


int main(){
    if(!wProp::font.loadFromFile("./Fonts/lucida.ttf")){
        throw int();
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

    Angar a("sve", 500, 500, pv);

    std::vector<Angar> v;
    v.push_back(a);
    

    angarTimeGrid.push_back(v);

    sf::RenderWindow window(sf::VideoMode(wProp::windowWidth, wProp::windowHeight), wProp::windowName);
    window.setFramerateLimit(wProp::windowFPS);

    sf::Music music;
    if(!music.openFromFile("./Music/Nyan.wav")){
        return 0;
    }
    music.play();



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