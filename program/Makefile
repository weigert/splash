#================================
# Splash Execution Mode Makefile
#================================

#Compiler Settings
CC = g++ -std=c++17
CFLAGS = -O -w
LFLAGS = -I/usr/local/include -L/usr/local/lib -shared -fPIC
LINK = -lX11 -ldl -lXfixes -lXrender -lSDL2 -lSDL2_image -lSDL2_ttf -lgif -lGL -lGLEW -lboost_system -lboost_filesystem

#Programs that are installable...
#Note: Remove programs you do not wish to install!
PROGRAMS = fidget img gif text hairy spiky

build:
	# Make build directory
	@if [ ! -d "./tmp" ]; then mkdir tmp; fi;
	@$(foreach i,$(PROGRAMS), echo Building $(i); $(CC) source/$(i)/$(i).cpp $(CFLAGS) $(LFLAGS) $(LINK) -o tmp/$(i);)

install:
	@$(foreach i,$(PROGRAMS), mv ./tmp/$(i) ~/.config/splash/exec/$(i);)
	@rmdir tmp

all: build install
