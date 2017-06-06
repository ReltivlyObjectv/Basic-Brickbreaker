//modified by: Christian Russell
//date: June 05
//purpose: Brick Breaker (project)
//
//cs3350 Summer 2017
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <list>
#include "Game.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define GRAVITY 0

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;


//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void movement();
void render();
bool isInShape(Shape *shape, float x, float y, bool inclusive=false);
void checkCollision();

int main(void) {
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//Initialize game
	Game::initialize();
	Game::loadLevel("1.level");
	//start animation
	while (!done) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		movement();
		render();
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void) {
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "Brick Breaker");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		ButtonPress | ButtonReleaseMask | PointerMotionMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
		InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void) {
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
}


void check_mouse(XEvent *e) {
	static int savex = 0;
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			Game::makeParticle();
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x) {
		savex = e->xbutton.x;
		if(savex < (Game::getPaddle()->width)){
			//Too far to the left
			savex = Game::getPaddle()->width;
		}else if(savex > WINDOW_WIDTH - (Game::getPaddle()->width)){
			//Too far to the right
			savex = WINDOW_WIDTH - (Game::getPaddle()->width);
		}
		Game::getPaddle()->center.x = savex;
	}
}

int check_keys(XEvent *e) {
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
		//You may check other keys here.



	}
	return 0;
}

void movement() {
	if (Game::getBallCount() <= 0)
		return;
	
	for(std::list<Particle*>::iterator it = Game::getBallIterator(); it != Game::getLastBallIterator(); ){
		Particle *p = *it;
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;

		//check for collision with shapes...
		for(std::list<Shape*>::iterator its = Game::getShapesIterator(); its != Game::getLastShapesIterator(); its++){
			Shape *s = *its;
			if(isInShape(s, p->s.center.x, p->s.center.y)){
				//Hitting Shape
				if(p->s.center.x < s->center.x){
					//Hitting Bottom
					p->velocity.y *= -1;
				}else if(p->s.center.x > s->center.x){
					//Hitting Top
					p->velocity.y *= -1;
				}else{
					//Hitting Direct Middle
					p->velocity.y *= -1;
				}
			}
		}
		//check for collision with edges...
		if(p->s.center.x <= 0 || p->s.center.x >= WINDOW_WIDTH){
			p->velocity.x *= -1;
		}
		if(p->s.center.y > WINDOW_HEIGHT){
			p->velocity.y *= -1;
		}
		//check for off-screen
		if (p->s.center.y < 0.0) {
			Game::removeParticle(*(it++));
			if(Game::getBallCount() < 1){
				//TODO Lose
			}
		}else{
			it++;
		}
	}
}

void render() {
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...
	for(std::list<Shape*>::iterator its = Game::getShapesIterator(); its != Game::getLastShapesIterator(); its++){
		if(*its != Game::getPaddle()){
			BreakableBlock* b = (BreakableBlock*) (*its);
			ColorBox bColors= b->getColors();
			glPushMatrix();
			glColor3ub(bColors.r,bColors.b,bColors.g);
			glTranslatef(b->center.x, b->center.y, b->center.z);
			w = b->width;
			h = b->height;
			glBegin(GL_QUADS);
			glVertex2i(-w,-h);
			glVertex2i(-w, h);
			glVertex2i( w, h);
			glVertex2i( w,-h);
			glEnd();
			glPopMatrix();
		}
	}
	//draw box
	Shape *s;
	glColor3ub(90,140,90);
	s = Game::getPaddle();
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
		glVertex2i(-w,-h);
		glVertex2i(-w, h);
		glVertex2i( w, h);
		glVertex2i( w,-h);
	glEnd();
	glPopMatrix();

	//draw all particles here
	std::list<Particle*>::iterator it = Game::getBallIterator();
	for(int i = 0; i < Game::getBallCount(); i++, it++){
		glPushMatrix();
		glColor3ub(150,160,220);
		Vec *c = &((*it)->s.center);
		w = 2;
		h = 2;
		glBegin(GL_QUADS);
		glVertex2i(c->x-w, c->y-h);
		glVertex2i(c->x-w, c->y+h);
		glVertex2i(c->x+w, c->y+h);
		glVertex2i(c->x+w, c->y-h);
		glEnd();
		glPopMatrix();
	}
}



bool isInShape(Shape *shape, float x, float y, bool inclusive) {
	float leftBound =  shape->center.x - (shape->width);
	float rightBound = shape->center.x + (shape->width);
	float upperBound = shape->center.y + (shape->height);
	float lowerBound = shape->center.y - (shape->height);
	if(inclusive) {
		//Borders are considered part of the shape
		if(x < leftBound) {
			return false;
		}else if(x > rightBound) {
			return false;
		}else if(y > upperBound) {
			return false;
		}else if(y < lowerBound) {
			return false;
		}else {
			return true;
		}
	}else {
		//Borders are not considered part of the shape
		if(x <= leftBound) {
			return false;
		}else if(x >= rightBound) {
			return false;
		}else if(y >= upperBound) {
			return false;
		}else if(y <= lowerBound) {
			return false;
		}else {
			return true;
		}
	}

}
