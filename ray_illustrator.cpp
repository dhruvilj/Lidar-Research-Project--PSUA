#include "ray_illustrator.h"
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

RayIllustrator::RayIllustrator(int width, int height, std::string title)
	: w_width{ width }, w_height{ height }, w_title{ title } {

}

bool RayIllustrator::Init() {
	if (SDL_VideoInit(NULL) < 0)
	{
		std::cerr << "Cannot initialize SDL_VIDEO. \n";
		return false;
	}

	mainwindow = SDL_CreateWindow(w_title.c_str(), SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, w_width, w_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (!mainwindow) {
		std::cerr << "Cannot create window. \n";
		return false;
	}

	maincontext = SDL_GL_CreateContext(mainwindow);

	SDL_GL_SetSwapInterval(1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-7000.f, 7000.f, -7000.f, 7000.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w_width, w_height);

	return true;
}

void RayIllustrator::Terminate() {
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_VideoQuit();
}

void RayIllustrator::RenderAsTriangles(const std::vector<Ray>& data, int elems_played) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0, 1.0, 0.0);

	//TODO MAX AND MIN RADIANS

	//TODO place them in angle increasing order

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < (data.size() - 1) && data.size() != 0; i++) {
		if (i == elems_played)
			glColor3f(1.0, 0.0, 0.0);
		auto elem = data[i];
		glVertex2d(cos(elem.angle)*elem.dist, sin(elem.angle)*elem.dist);	//	vertex 1
		elem = data[i + 1];
		glVertex2d(cos(elem.angle)*elem.dist, sin(elem.angle)*elem.dist);	//	vertex 2
		glVertex2d(0.f, 0.f);												//	vertex 3 at origin
	}
	glEnd();

	SDL_GL_SwapWindow(mainwindow);
}
