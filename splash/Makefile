CC = g++ -std=c++17
CFLAGS = -Wfatal-errors -O
LFLAGS = -I/usr/local/include -L/usr/local/lib
LINK = -ldl -lX11 -lXfixes -lXrender -lGL -lGLEW -lboost_system -lboost_filesystem

splash: splash.cpp
	@$(CC) splash.cpp $(CFLAGS) $(LFLAGS) $(LINK) -o splash
	@mv splash /usr/local/bin/splash
	@echo "splash compiled. moved to /usr/local/bin"
	@echo "test with: splash fidget"
