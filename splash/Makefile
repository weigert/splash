#================================
#        Splash Makefile
#================================

# Compiler Configuration
CC = g++ -std=c++17
CFLAGS = -Wfatal-errors -O
LFLAGS = -I/usr/local/include -L/usr/local/lib
LINK = -ldl -lX11 -lXfixes -lXrender -lGL -lGLEW -lboost_system -lboost_filesystem

build:
			@if [ ! -d "./tmp" ]; then mkdir tmp; fi;
			@echo "compiling splash in ./tmp"
			@$(CC) splash.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o tmp/splash;
			@echo "done"

install:
			@mv tmp/splash /usr/local/bin/splash
			@echo "moved to /usr/local/bin/splash"
			@echo "test with: splash fidget"
			@rmdir tmp

.PHONY: splash
splash: build install
