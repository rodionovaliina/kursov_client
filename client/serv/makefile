.PHONY: all clean

CXX=g++
 
OFLAGS=-O3 -Wall -DNDEBUG -std=c++17

LDFLAGS= -lcrypto++  

EXEC=server
SOURCE=$(EXEC).cpp

all: $(EXEC)

$(EXEC): $(SOURCE)
	$(CXX)  $(SOURCE)  -o $(EXEC) $(OFLAGS) $(LDFLAGS)

clean:
	rm -f $(EXEC)

