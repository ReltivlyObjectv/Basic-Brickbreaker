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

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 3
#define GRAVITY 0

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 10
#define PADDLE_BUFFER 10

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

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
	public:
		static void initialize();
		static int getBallCount();
		static Shape* getPaddle();
		static std::list<Particle*>::iterator getBallIterator();
		static void makeParticle();
		static void removeParticle(Particle* p);
};
Shape Game::paddle;
std::list<Particle*> Game::balls;

void Game::initialize(){
	//declare the paddle
	Game::getPaddle()->width = PADDLE_WIDTH;
	Game::getPaddle()->height = PADDLE_HEIGHT;
	Game::getPaddle()->center.x = 120 + 5*65;
	Game::getPaddle()->center.y = PADDLE_BUFFER + (0.5 * PADDLE_HEIGHT) ;
	
}
Shape* Game::getPaddle(){
	return &paddle;
}
std::list<Particle*>::iterator Game::getBallIterator(){
	return balls.begin();
}
int Game::getBallCount(){
	return balls.size();
}
void Game::makeParticle() {
	if (getBallCount() >= MAX_PARTICLES){
		return;
	}
	//position of particle
	Particle *p = new Particle();
	p->s.center.x = Game::getPaddle()->center.x;
	p->s.center.y = Game::getPaddle()->center.y;
	p->velocity.y = 4.0;
	p->velocity.x = 1.0;
	balls.push_front(p);
}
void Game::removeParticle(Particle* p){
	balls.remove(p);
	std::cout << "Remove ball, there are now " << getBallCount() << "left.";
}

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void movement();
void render();
bool isInShape(Shape &shape, float x, float y, float z=0, bool square=true, bool inclusive=true);
void checkCollision();

int main(void) {
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//Initialize game
	Game::initialize();
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
	XStoreName(dpy, win, "335 Lab1   LMB for particle");
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
	std::list<Particle*>::iterator it = Game::getBallIterator();
	for(int i = 0; i < Game::getBallCount(); i++, it++){
		Particle *p = *it;
		p->s.center.x += p->velocity.x;
		p->s.center.y += p->velocity.y;

		//check for collision with shapes...
		
		//check for collision with edges...
		if(p->s.center.x <= 0 || p->s.center.x >= WINDOW_WIDTH){
			p->velocity.x *= -1;
		}
		if(p->s.center.y > WINDOW_HEIGHT){
			p->velocity.y *= -1;
		}
		//check for off-screen
		if (p->s.center.y < 0.0) {
			Game::removeParticle(*it);
			if(Game::getBallCount() < 1){
				//TODO Lose
			}
		}
	}
}

void render() {
	float w, h;
	glClear(GL_COLOR_BUFFER_BIT);
	//Draw shapes...

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



bool isInShape(Shape &shape, float x, float y, float z, bool square, bool inclusive) {
	if(square) {
		//Square
		float leftBound =  shape.center.x - (shape.width / 2);
		float rightBound = shape.center.x + (shape.width / 2);
		float upperBound = shape.center.y + (shape.height / 2);
		float lowerBound = shape.center.y - (shape.height / 2);
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
	}else {
		//Circle
		//TODO
		return false;
	}
}
void checkCollision(){

}
