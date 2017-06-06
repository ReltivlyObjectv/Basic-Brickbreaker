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
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};
struct ColorBox {
	int r, g, b;
};

class BreakableBlock : public Shape {
	private:
		int health;
		ColorBox colors;
	public:	
		BreakableBlock(int startingHealth);
		void dealDamage();
		int getHealth();
		ColorBox getColors();
		void setColors(int red, int green, int blue);
};
struct Level {
	std::list<BreakableBlock*> blocks;
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
		static Level getLevel(std::string levelName);
};
#endif
