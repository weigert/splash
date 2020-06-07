#Visualization Program Compilation!
CC = g++ -std=c++17
CFLAGS = -Wfatal-errors -O
LFLAGS = -I/usr/local/include -L/usr/local/lib -shared -fPIC
LINK = -lX11 -ldl -lXfixes -lXrender -lSDL2 -lSDL2_image -lGL -lGLEW -lboost_system -lboost_filesystem

fidget:
	@$(CC) source/fidget/fidget.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o fidget
	mv fidget ~/.config/splash/exec/fidget

img:
	@$(CC) source/img/img.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o img
	mv img ~/.config/splash/exec/img

all: fidget img
