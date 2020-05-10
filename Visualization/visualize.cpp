#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include "visualize.hpp"
#include "calendar.hpp"
#include "json.hpp"

//#define debugMode
#define musicPlay

Plane::Plane(const std::string &newName, const std::string &newCompanyName, 
        int newWidth, int newHeight, int newX, int newY){
    name = newName;
    companyName = newCompanyName;
    width = newWidth;
    height = newHeight;
    x = newX;
    y = newY;
}

Plane::Plane(const Plane &p){
    name = p.name;
    companyName = p.companyName;
    width = p.width;
    height = p.height;
    x = p.x;
    y = p.y;
}

Angar::Angar(const std::string &newName, int newWidth, int newHeight, const std::vector<Plane> &newPlanes){
    name = newName;
    width = newWidth;
    height = newHeight;
    planes = newPlanes;
}

Button::Button(int x, int y, int width, int height, const std::string &s): x(x), 
    y(y), width(width), height(height), title(s), active(false){};

void Button::init(int x, int y, int width, int height, const std::string &s){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->title = s;
    active = false;
}

void Button::switchMode(){
    active = !active;
}

bool Button::isActive() const{
    return this->active;
}

Theme::Theme(){
    curTheme = Theme::Light;
}

Theme::Theme(int theme){
    Theme::applyTheme(theme);
}

void Theme::applyTheme(int theme){
    switch(theme){
        case Theme::Light:{
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

bool clickInsideButton(sf::Vector2i &windowPos, Button &button, sf::Vector2i &click){
    int clickX, clickY;
    clickX = click.x;
    clickY = click.y;

    int winX, winY; 
    winX = windowPos.x;
    winY = windowPos.y;

    int actualX = clickX - winX;
    int actualY = clickY - winY - wProp::topBarSize;
    //std::cout << "Click at: " << clickX << " " << clickY << std::endl;
    //std::cout << "ActualX: " << actualX << " ActualY: " << actualY << std::endl;
    //std::cout << "ButtonX: " << button.x << " ButtonY: " << button.y << std::endl; 
    //std::cout << "BX + BW: " << button.x + button.width << " BY + BW: " << button.y + button.height << std::endl;

    if(actualX >= button.x && actualX <= button.x + button.width &&
        actualY >= button.y && actualY - wProp::topBarSize <= button.y + button.height){
        return true;
    }else{
        return false;
    }
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

    return std::min(angarActualWidth / angarNameLength, distanceToUpperBound - 
        aProp::angarOutlineThickness - aProp::angarTextPadding);
}

int calculatePlaneFontSize(const sf::RectangleShape &rec, const Plane &plane, const std::string &str){
    int planeActualWidth = rec.getSize().x;
    int planeActualHeight = rec.getSize().y;

    int nameLength = str.length();

    return std::min(planeActualWidth / nameLength + 4, (planeActualHeight - 
        pProp::planeTextPadding) / 2);
}

void drawAngarsAndPlanes(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid,
    timePoint t, double widthCompressionRatio, double heightCompressionRatio){
    //отрисовка ангаров в масштабе
    sf::Text text;
    text.setFont(gProp::font);

    int lastRectanglePos = 0;
    for(int i = 0; i < timeGrid[t].size(); i++){
        int angarWidth = timeGrid[t][i].width * widthCompressionRatio;
        int angarHeight = timeGrid[t][i].height * heightCompressionRatio;

        sf::RectangleShape angarRec;
        angarRec.setSize(sf::Vector2f(angarWidth, angarHeight));
        
        int x, y;
        x = aProp::minDistanceBetweenAngars * widthCompressionRatio + lastRectanglePos;
        y = aProp::minDistanceBetweenAngars * heightCompressionRatio;

        lastRectanglePos = x + angarWidth;
        //std::cout << "Angar pos: " << x << " " << y << std::endl;
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

        drawPlanes(window, timeGrid, t, i, widthCompressionRatio, heightCompressionRatio, x, y);
    }
}

void drawPlanes(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid,
    timePoint t, int i, double widthCompressionRatio, double heightCompressionRatio,
    int angarX, int angarY){

    sf::Text text;
    text.setFont(gProp::font);
    
    for(auto &plane: timeGrid[t][i].planes){
        int newPlaneWidth = plane.width * widthCompressionRatio;
        int newPlaneHeight = plane.height * heightCompressionRatio;

        int x = angarX + plane.x;
        int y = angarY + plane.y;

        sf::RectangleShape planeRec;
        planeRec.setSize(sf::Vector2f(newPlaneWidth, newPlaneHeight));
        planeRec.setPosition(x, y);
        planeRec.setFillColor(determinePlaneColor(plane.companyName));
        planeRec.setOutlineColor(gProp::theme.angarOutlineColor);
        
        planeRec.setOutlineThickness(pProp::planeOutlineThickness);
        
        window.draw(planeRec);

        bool drawCompanies = gProp::showCompaniesButton.isActive();

        int textFontSize;

        if(drawCompanies){
            textFontSize = std::min(calculatePlaneFontSize(planeRec, plane, plane.name),
                calculatePlaneFontSize(planeRec, plane, plane.companyName));
        }else{
            textFontSize = calculatePlaneFontSize(planeRec, plane, plane.name);
        }

        int textX = x + pProp::planeOutlineThickness + pProp::planeTextPadding;
        int textY = y;

        text.setString(plane.name);
        text.setCharacterSize(textFontSize);
        text.setFillColor(gProp::theme.angarOutlineColor);
        text.setPosition(textX, textY);
        window.draw(text);
        

        if(drawCompanies){
            textY += textFontSize;
            
            text.setString(plane.companyName);
            text.setPosition(textX, textY);
            window.draw(text);
        }
    }
}

void drawDate(sf::RenderWindow &window, timePoint t){
    std::string date = gProp::calendar.getDate(t);
    
    sf::Text text;
    text.setString(date);
    text.setFont(gProp::font);
    text.setCharacterSize(20);
    text.setFillColor(gProp::theme.angarOutlineColor);

    int x = wProp::windowWidth - date.length() * text.getCharacterSize() + 65;
    int y = wProp::windowHeight - text.getCharacterSize() - dProp::dateTextPadding;
    text.setPosition(x, y);

    window.draw(text);
}

void drawButton(sf::RenderWindow &window, const Button &button){
    sf::RectangleShape buttonRec;
    buttonRec.setPosition(button.x, button.y);
    buttonRec.setSize(sf::Vector2f(button.width, button.height));
    buttonRec.setOutlineThickness(bProp::buttonOutlineThickness);
    buttonRec.setOutlineColor(gProp::theme.angarOutlineColor);

    if(button.isActive()){
        buttonRec.setFillColor(sf::Color::Green);
    }else{
        buttonRec.setFillColor(sf::Color::Red);
    }

    window.draw(buttonRec);

    sf::Text t;
    t.setFont(gProp::font);
    t.setString(button.title);
    t.setCharacterSize(20);
    t.setFillColor(gProp::theme.angarOutlineColor);
    t.setPosition(button.x + button.width - t.getCharacterSize() * t.getString().getSize() + 86,
        button.y - t.getCharacterSize() - bProp::buttonOutlineThickness - bProp::buttonTextPadding);

    window.draw(t);
}

sf::Color determinePlaneColor(const std::string &planeCompanyName){
    if(planeCompanyName == "S7"){
        return pColor::S7;
    }else if(planeCompanyName == "Aeroflot"){
        return pColor::Aeroflot;
    }else if(planeCompanyName == "Ural"){
        return pColor::UralAirlines;
    }else if(planeCompanyName == "Pobeda"){
        return pColor::Pobeda;
    }else if(planeCompanyName == "Alrosa"){
        return pColor::Alrosa;
    }else if(planeCompanyName == "Utair"){
        return pColor::Utair;
    }else if(planeCompanyName == "Rossiya"){
        return pColor::Rossiya;
    }else if(planeCompanyName == "Belavia"){
        return pColor::Belavia;
    }else{
        std::cerr << "Error in determine plane color function: unknown color\n";
        return sf::Color::Red;
    }
}

void drawAll(sf::RenderWindow &window, const std::vector<std::vector<Angar>> &timeGrid, timePoint t){
    try{
        timeGrid.at(t);
    }catch(...){
        std::cerr << "------------------------------\n";
        std::cerr << "Error in draw function: index out of range\n";
        std::cerr << "Vector size: " << timeGrid.size() << " index: " << t << "\n";
        return;
    }

    if(timeGrid[t].empty()){
        return;
    }

    //во сколько раз нужно всё сжать, если получилась слишком большая длина и ширина
    double widthCompressionRatio = calculateWidthCompressionRatio(timeGrid, t);
    double heightCompressionRatio = calculateHeightCompressionRatio(timeGrid, t);

    drawAngarsAndPlanes(window, timeGrid, t, widthCompressionRatio, heightCompressionRatio);
}


int main(){
    if(!loadFont(gProp::font, gProp::fontPath)){
        std::cerr << "------------------------------\n";
        std::cerr << "Error: can not load font from path \'" << gProp::fontPath << "\'\n";
        return 1;
    }

    gProp::calendar.init(15, 8, 2020);
    gProp::showCompaniesButton.init(wProp::windowWidth-35, wProp::windowHeight-80 , 30, 30, 
        "company name");

    gProp::theme.applyTheme(Theme::Light);

    
    std::vector<std::vector<Angar>> angarTimeGrid;

    std::ifstream is("output.json");
    Json::Document doc = Json::Load(is);
    is.close();

    for(const Json::Node& node : doc.GetRoot().AsMap().at("positions").AsArray()){
        std::vector<Angar> va;
        std::vector<Plane> vp;
        Angar svo, dme, vko;

        svo.name = "svo";
        svo.width = 90;
        svo.height = 200;
        dme.name = "dme";
        dme.width = 80;
        dme.height = 300;
        vko.name = "vko";
        vko.width = 70;
        vko.height = 150;
        
        Plane plane;

        for(const Json::Node& inner_node : node.AsArray()){
            Json::Node cur_day = inner_node;
            
            plane.width = cur_day.AsMap().at("width").AsLong() / 1000;
            plane.height = cur_day.AsMap().at("height").AsLong() / 1000;
            plane.x = cur_day.AsMap().at("x").AsLong() / 1000;
            plane.y = cur_day.AsMap().at("y").AsLong() / 1000;
            plane.name = cur_day.AsMap().at("planeName").AsString();
            plane.companyName = cur_day.AsMap().at("companyName").AsString();
            std::string angar = cur_day.AsMap().at("hangarName").AsString();
            
            if(angar == "DME"){
                dme.planes.push_back(plane);
            }else if(angar == "VKO"){
                vko.planes.push_back(plane);
            }else if(angar == "SVO"){
                svo.planes.push_back(plane);
            }

        }

        va.push_back(svo);
        va.push_back(dme);
        va.push_back(vko);

        angarTimeGrid.push_back(va);
    }


    sf::RenderWindow window(sf::VideoMode(wProp::windowWidth, wProp::windowHeight), wProp::windowName);
    window.setFramerateLimit(wProp::windowFPS);

    #ifdef musicPlay
    sf::Music music;
    if(!music.openFromFile("./Music/Nyan.wav")){
        return 0;
    }
    music.play();
    #endif


    timePoint t = 0;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                sf::Vector2i mouseClick = sf::Mouse::getPosition(); 
                sf::Vector2i windowPos = window.getPosition();
                if(clickInsideButton(windowPos, gProp::showCompaniesButton, mouseClick)){
                    gProp::showCompaniesButton.switchMode();
                    //std::cout << "click on button" << std::endl;
                }
                
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
                if(t < angarTimeGrid.size()-1){
                    t++;
                }
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
                if(t > 0){
                    t--;
                }
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
                gProp::theme.applyTheme(Theme::Light);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
                gProp::theme.applyTheme(Theme::Dark);
            }
        }

        window.clear(gProp::theme.windowBGColor);
        drawAll(window, angarTimeGrid, t);
        drawDate(window, t);
        drawButton(window, gProp::showCompaniesButton);
        window.display();
    }
    return 0;
}