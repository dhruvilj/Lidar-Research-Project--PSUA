#Copyright Notice:
#The files within this zip file are copyrighted by Lazy Foo' Productions (2004-2014)
#and may not be redistributed without written permission.

#OBJS specifies which files to compile as part of the project
OBJS = main.cpp

#CC specifies which compiler we're using
CC = g++ -std=c++11

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lGL -pthread 

INC=-Iinclude -Iheaders

#SOURCES := $(shell find '.' -name '*.cpp')
SOURCES = main.cpp audio_player.cpp lidar_control.cpp ray_illustrator.cpp logic.cpp Connection_information.cpp common.cpp server.cpp

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = lidar

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(SOURCES) $(INC) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) -lsfml-audio -lsfml-system -lurg_cpp -lm
