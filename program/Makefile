TINYLINK = -lX11 -ldl -lXfixes -lXrender -lSDL2 -lnoise -lSDL2_image -lGL -lGLEW -lboost_system -lboost_filesystem

CC = g++ -std=c++17
COMPILER_FLAGS = -Wfatal-errors -O
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib -shared -fPIC
OBJ_NAME = fidget.o

all: source/fidget/fidget.cpp
			$(CC) source/fidget/fidget.cpp $(COMPILER_FLAGS) $(LINKER_FLAGS) $(TINYLINK) -o $(OBJ_NAME)
			mv fidget.o ../splash/exec/fidget
