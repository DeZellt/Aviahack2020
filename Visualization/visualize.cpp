#include <iostream>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "visualize.hpp"
#include "calendar.hpp"

//#define debugMode
//#define musicPlay

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

    /*
    #ifdef debugMode
    std::cout << "calculatePlaneFontSize: " << str << " " << nameLength << " " << planeActualWidth << std::endl;
    std::cout << (planeActualWidth / nameLength) << std::endl;
    std::cout << ((planeActualHeight - pProp::planeTextPadding) / 2) << std::endl;
    #endif
    */

    return std::min(planeActualWidth / nameLength + 4, (planeActualHeight - 
        pProp::planeTextPadding) / 2);
    
    //return planeActualWidth / nameLength + 1;
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

        //std::cout << newPlaneWidth << " " << newPlaneHeight << " " x << " " << y << std::endl; 

        int x = angarX + plane.x;
        int y = angarY + plane.y;

        sf::RectangleShape planeRec;
        planeRec.setSize(sf::Vector2f(newPlaneWidth, newPlaneHeight));
        planeRec.setPosition(x, y);
        planeRec.setFillColor(determinePlaneColor(plane.companyName));
        planeRec.setOutlineColor(gProp::theme.angarOutlineColor);
        //planeRec.setFillColor(gProp::theme.windowBGColor);
        //planeRec.setOutlineColor(determinePlaneColor(plane.companyName));
        
        planeRec.setOutlineThickness(pProp::planeOutlineThickness);
        
        window.draw(planeRec);

        int textFontSize = std::min(calculatePlaneFontSize(planeRec, plane, plane.name),
            calculatePlaneFontSize(planeRec, plane, plane.companyName));
        //int textFontSize = calculatePlaneFontSize(planeRec, plane, plane.name);
        
        int textX = x + pProp::planeOutlineThickness + pProp::planeTextPadding;
        //int middlePosition = (newPlaneWidth - plane.name.length() * textFontSize) / 2;
        //textX += middlePosition > 0 ? middlePosition : -middlePosition;
        int textY = y;

        #ifdef debugMode
        std::cout << "----Plane: " << plane.name << " " << plane.companyName << std::endl;
        std::cout << "newPlaneWidth: " << newPlaneWidth << std::endl;
        std::cout << "textFontSize: " << textFontSize << std::endl;
        std::cout << "text pixels: " << textFontSize * plane.name.length() << std::endl;
        #endif

        text.setString(plane.name);
        text.setCharacterSize(textFontSize);
        text.setFillColor(gProp::theme.angarOutlineColor);
        text.setPosition(textX, textY);
        window.draw(text);
        
        textY += textFontSize;
        
        text.setString(plane.companyName);
        text.setPosition(textX, textY);
        window.draw(text);
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
        std::cerr << "Error in draw function: index out of range\n";
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

    drawAngarsAndPlanes(window, timeGrid, t, widthCompressionRatio, heightCompressionRatio);
}


int main(){
    if(!loadFont(gProp::font, gProp::fontPath)){
        std::cerr << "------------------------------\n";
        std::cerr << "Error: can not load font from path \'" << gProp::fontPath << "\'\n";
        return 1;
    }

    gProp::calendar.init(31, 10, 2000);
    std::cout << gProp::calendar.getDate(0) << std::endl;

    gProp::theme.applyTheme(Theme::Light);

    std::vector<std::vector<Angar>> angarTimeGrid;

    Plane p1("Il-96-300", "S7", 23, 29, 3, 3);
    Plane p2("An-24", "Aeroflot", 20, 40, 90, 3);
    Plane p3("A319", "Ural", 20, 70, 170, 3);
    Plane p4("737-800", "Pobeda", 70, 40, 15, 250);
    Plane p5("737-200", "Alrosa", 23, 60, 3, 120);
    Plane p6("Tu-154M", "Utair", 23, 52, 100, 120);
    Plane p7("A321", "Rossiya", 23, 50, 120, 120);
    Plane p8("A320", "Belavia", 23, 48, 190, 120);

    std::vector<Plane> pv;
    pv.push_back(p1);
    pv.push_back(p2);
    pv.push_back(p3);
    pv.push_back(p5);
    pv.push_back(p6);

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


    //sf::Mouse mouse;
    timePoint t = 0;
    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                //sf::Vector2i mouseClick = sf::Mouse::getPosition();
                //sf::Vector2i windowPos = window.getPosition();
                //std::cout << "Left click at " << mouseClick.x - windowPos.x << " " << mouseClick.y - windowPos.y << std::endl;
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
        }

        window.clear(gProp::theme.windowBGColor);
        drawAll(window, angarTimeGrid, t);
        drawDate(window, t);
        //drawTest(window);
        window.display();
        //std::cout << window.getPosition().x  << " " << window.getPosition().y  << std::endl;
    }

    return 0;
}