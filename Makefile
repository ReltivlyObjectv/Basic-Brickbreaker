# cs335 lab1
# to compile your project, type make and press enter

all: main

main: main.cpp
	g++ main.cpp -Wall -oBrickBreaker -lX11 -lGL -lGLU -lm
mac: main.cpp
	g++ main.cpp -Wall -lX11 -lGL -lGLU -lm -std=c++11 -o BrickBreaker -stdlib=libc++ -I/usr/X11R6/include -L/usr/X11R6/lib -framework GLUT -framework OpenGL -framework Cocoa -Wno-deprecated
clean:
	rm -f BrickBreaker
	rm -f *.o

