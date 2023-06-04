CC=g++
CCFlags = -Wall
LDFLAGS= -lcrypto++  
EXEC=client
SOURCE = main.cpp client.h client.cpp md5.cpp md5.h 
OPT = -Ofast
	
all: int16_t
int16_t:
	$(CC) $(CCFlags) $(OPT) $(SOURCE) -o $(EXEC) $(LDFLAGS)
dbg:
	$(CC) -g $(SOURCE) -o $(EXEC)DBG	
