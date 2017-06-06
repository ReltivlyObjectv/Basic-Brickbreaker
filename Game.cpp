#include "Game.h"
#include <iostream>
#include <fstream>
#include <string>

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
	//Load first level
	getLevel("1.level");
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
Level Game::getLevel(std::string levelName){
	Level newLevel;
	std::ifstream levelFile;
	levelFile.open(levelName.c_str());
	std::string line;
	while(getline(levelFile, line)){
		//# x y width height starting_health
		if(line[0] == '#'){
			std::cout << "Skipping comment: " << line << std::endl;
			continue;
		}
		std::cout << "Reading line: " << line << std::endl;
		std::string x, y, width, height, startingHealth, red, green, blue;
		int variablePosition = 0;
		for(int i = 0; i < line.length(); i++){
			if(line[i] == ' '){
				variablePosition++;
				continue;
			}
			switch(variablePosition){
				case 0:
					//x
					x.push_back(line[i]);
					break;
				case 1:
					//y
					y.push_back(line[i]);
					break;
				case 2:
					//width
					width.push_back(line[i]);
					break;
				case 3:
					//height
					height.push_back(line[i]);
					break;
				case 4:
					//starting_health
					startingHealth.push_back(line[i]);
					break;
				case 5:
					//Red
					red.push_back(line[i]);
					break;
				case 6:
					//Green
					green.push_back(line[i]);
					break;
				case 7:
					//Blue
					blue.push_back(line[i]);
					break;
				default:
					//Out of bounds
					std::cout << "Error reading level: out of bounds" << std::endl;
			}
		}
		//Add information to a block file
		BreakableBlock* newBlock = new BreakableBlock(std::stoi(startingHealth));
		newBlock->width = std::stof(width);
		newBlock->height = std::stof(height);
		newBlock->center.x = std::stof(x);
		newBlock->center.y = std::stof(y);
		newBlock->setColors(std::stoi(red),std::stoi(green),std::stoi(blue));	

		newLevel.blocks.push_back(newBlock);}

	return newLevel;
}

//Breakable Block Member Functions
BreakableBlock::BreakableBlock(int startingHealth){
	health = startingHealth;
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
