# cs335 lab1
# to compile your project, type make and press enter

all: main

main: main.cpp
	g++ main.cpp -Wall -olab1 -lX11 -lGL -lGLU -lm
clean:
	rm -f BrickBreaker
	rm -f *.o

