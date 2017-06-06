#include "Game.h"
#include <iostream>

//Initialize Variables
Shape Game::paddle;
std::list<Particle*> Game::balls;
std::list<Shape*> Game::allBoxes;

//Game Member Functions
void Game::initialize(){
	//declare the paddle
	paddle.width = PADDLE_WIDTH;
	paddle.height = PADDLE_HEIGHT;
	paddle.center.x = 120 + 5*65;
	paddle.center.y = PADDLE_BUFFER + (0.5 * PADDLE_HEIGHT) ;
	allBoxes.push_front(&paddle);
}
Shape* Game::getPaddle(){
	return &paddle;
}
std::list<Particle*>::iterator Game::getBallIterator(){
	return balls.begin();
}
std::list<Particle*>::iterator Game::getLastBallIterator(){
	return balls.end();
}
std::list<Shape*>::iterator Game::getShapesIterator(){
	return allBoxes.begin();
}
std::list<Shape*>::iterator Game::getLastShapesIterator(){
	return allBoxes.end();
}
int Game::getBallCount(){
	return balls.size();
}
void Game::makeParticle() {
	if (getBallCount() >= MAX_PARTICLES){
		std::cout << "Could not create particle" << std::endl;
		return;
	}
	//position of particle
	Particle *p = new Particle();
	std::cout << "Creating particle " << p << std::endl;
	p->s.center.x = Game::getPaddle()->center.x;
	p->s.center.y = Game::getPaddle()->center.y + (Game::getPaddle()->height / 2);
	p->velocity.y = 4.0;
	p->velocity.x = 1.0;
	balls.push_front(p);
}
void Game::removeParticle(Particle* p){
	std::cout << "Out of bounds - removing particle " << p << std::endl;
	balls.remove(p);
}
//Breakable Block Member Functions
BreakableBlock::BreakableBlock(){
	health = 0;
}
void BreakableBlock::dealDamage(){
	if(health > 0){
		health--;
	}
}
int BreakableBlock::getHealth(){
	return health;
}
ColorBox BreakableBlock::getColors(){
	return colors;
}
void BreakableBlock::setColors(int red, int green, int blue){
	colors.r = red;
	colors.g = green;
	colors.b = blue;
}
