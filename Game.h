#ifndef GAME_H
#define GAME_H

#include <list>

#define MAX_PARTICLES 3

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 10
#define PADDLE_BUFFER 10

//Structures
struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

class Game {
	private: 
		static Shape paddle;
		static std::list<Particle*> balls;
		static std::list<Shape*> allBoxes;
	public:
		static void initialize();
		static int getBallCount();
		static Shape* getPaddle();
		static std::list<Particle*>::iterator getBallIterator();
		static std::list<Particle*>::iterator getLastBallIterator();
		static std::list<Shape*>::iterator getShapesIterator();
		static std::list<Shape*>::iterator getLastShapesIterator();
		static void makeParticle();
		static void removeParticle(Particle* p);
};
#endif