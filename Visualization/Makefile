all:
	g++ -std=c++17 -c visualize.cpp calendar.cpp json.cpp
	g++ visualize.o calendar.o json.o -o visualize-app -lsfml-graphics -lsfml-system -lsfml-window -lsfml-audio
	rm visualize.o calendar.o json.o