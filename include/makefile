OBJS = splash.cpp

TINYLINK = -lX11 -lXfixes -lXrender -lSDL2 -lnoise -lSDL2_image -lGL -lGLEW -lboost_system -lboost_filesystem

CC = g++ -std=c++17
COMPILER_FLAGS = -Wfatal-errors -O
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib
OBJ_NAME = splash

all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(TINYLINK) -o $(OBJ_NAME)
