#Visualization Program Compilation!
CC = g++ -std=c++17
CFLAGS = -O -w
LFLAGS = -I/usr/local/include -L/usr/local/lib -shared -fPIC
LINK = -lX11 -ldl -lXfixes -lXrender -lSDL2 -lSDL2_image -lGL -lGLEW -lboost_system -lboost_filesystem

fidget:
	@$(CC) source/fidget/fidget.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o fidget
	mv fidget ~/.config/splash/exec/fidget

img:
	@$(CC) source/img/img.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o img
	mv img ~/.config/splash/exec/img

gif:
	@$(CC) source/gif/gif.cpp $(CFLAGS) $(LFLAGS) -lgif $(LINK) -o gif
	mv gif ~/.config/splash/exec/gif

text:
	@$(CC) source/text/text.cpp $(CFLAGS) $(LFLAGS) -lSDL2_ttf $(LINK) -o text
	mv text ~/.config/splash/exec/text

all: fidget img gif text
