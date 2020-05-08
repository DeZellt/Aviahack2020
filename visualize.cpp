#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "visualize.hpp"

sf::Font font;

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

//окно для рендеринга, массив с днями
void drawAngars(sf::RenderWindow &window, std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    sf::RectangleShape angarRec;
    sf::RectangleShape planeRec;

    sf::Text text;
    
    text.setString("Hello123456");
    text.setFont(font);
    text.setCharacterSize(30);
    
    text.setFillColor(sf::Color::Red);
    text.setPosition(cc::windowWidth/2, cc::windowHeight/2);

    window.draw(text);
}


int main(){
    if(!font.loadFromFile("./Fonts/lucida.ttf")){
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

    sf::RenderWindow window(sf::VideoMode(cc::windowWidth, cc::windowHeight), cc::windowName);

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