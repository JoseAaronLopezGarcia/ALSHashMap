CC = gcc
TARGET = main
SRC = main.c alsmap.c bst.c linkedlist.c
FLAGS = -Wall
LIBS = -lrt

CPP = g++
CPP_TARGET = cppmain
CPP_SRC = main.cpp
CPP_FLAGS = -Wall -std=c++11
CPP_LIBS = -lrt -lstdc++

all:
	$(CC) $(FLAGS) -o $(TARGET) $(SRC) $(LIBS)

bst:
	$(CC) $(FLAGS) -DUSE_TREE -o $(TARGET) $(SRC) $(LIBS)

rbt:
	$(CC) $(FLAGS) -DUSE_TREE -DUSE_RED_BLACK -o $(TARGET) $(SRC) $(LIBS)

cpp:
	$(CPP) $(CPP_FLAGS) -o $(CPP_TARGET) $(CPP_SRC) $(CPP_LIBS)

eprobe:
	$(CC) $(FLAGS) -DEXTENDED_PROBING -o $(TARGET) $(SRC) $(LIBS)

ebst:
	$(CC) $(FLAGS) -DUSE_TREE -DEXTENDED_PROBING -o $(TARGET) $(SRC) $(LIBS)

erbt:
	$(CC) $(FLAGS) -DUSE_TREE -DUSE_RED_BLACK -DEXTENDED_PROBING -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm $(TARGET) $(CPP_TARGET)

