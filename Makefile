CC = g++
CPPFLAGS = -O2 -Wall -Wextra -pedantic -std=c++11

CPPFILES = src/*.cpp
HPPFILES = src/headers/*.hpp

app: $(HPPFILES) $(CPPFILES) 
	mkdir -p bin
	$(CC) -o bin/TKOM $(HPPFILES) $(CPPFILES) $(CPPFLAGS)

all: app

clean: 
	rm bin/TKOM

