// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, -0.5);
	glEnd();
}

int main(int argc, char *argv[])
{

	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 300, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	initGL();
	SDL_Event e;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

		}
		render();
		SDL_GL_SwapWindow(window);
		if (1000 / FPS > SDL_GetTicks() - start)
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}

	SDL_Quit();

	return 0;
}

