CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC

LIB_SOURCES := logger.cpp
LIB_HEADERS := logger.h
APP_SOURCE := main.cpp

LIB_NAME := liblogger.so
APP_NAME := logger_app

all: lib app

lib: $(LIB_SOURCES) $(LIB_HEADERS)
	$(CXX) $(CXXFLAGS) -shared $(LIB_SOURCES) -o $(LIB_NAME)

app: $(APP_SOURCE) $(LIB_HEADERS)
	$(CXX) $(CXXFLAGS) $(APP_SOURCE) -L. -llogger -o $(APP_NAME) -Wl,-rpath,.

run: all
	./$(APP_NAME) log.txt 1

clean:
	rm -f $(LIB_NAME) $(APP_NAME) *.o

.PHONY: all lib app run clean