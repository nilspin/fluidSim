// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "camera.h"
#include "ShaderProgram.hpp"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint VAO;
GLuint VBO;
GLuint positionAttribute, colAttrib, uniColor;

void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
}

int main(int argc, char *argv[])
{
	/* Don't use camera for now*/
	Camera cam;
	cam.SetMode(FREE);
	cam.SetPosition(glm::vec3(0, 0, -10));
	cam.SetLookAt(glm::vec3(0, 0, 0));
	cam.SetClipping(.01, 50);
	cam.SetFOV(45);
	
	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 300, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

	//Create Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//create a vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//define vertices of our triangle
	GLfloat vertices[] = {
		// front
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0
	};
	
	GLfloat colors[] = 
	{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f
	};
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * 3 * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(GLfloat), 8 * 4 * sizeof(GLfloat), colors);
	//Note that we use GL_STATIC_DRAW because we want to send data to GPU only once thoughout the
	//program


	GLushort indices[] = { 0, 1, 2,	2, 3, 0,	3, 2, 6,	6, 7, 3,	7, 6, 5,	5, 4, 7,	4, 5, 1,	1, 0, 4,	4, 0, 3,	3, 7, 4,	1, 5, 6,	6, 2, 1 };
	//define an index buffer
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	
#pragma region SHADER_FUNCTIONS

	//=============================================================================================

	ShaderProgram *shaderProgram = new ShaderProgram();
	shaderProgram->initFromFiles("simple.vert", "simple.frag");

	shaderProgram->addAttribute("position");
	shaderProgram->addAttribute("color");

	shaderProgram->addUniform("MVP");

	shaderProgram->use();


#pragma endregion SHADER_FUNCTIONS

	/*Now specify the layout of the Vertex data */

	// The following line tells the CPU program that "vertexData" stuff goes into "posision"
	//parameter of the vertex shader.
	glVertexAttribPointer(shaderProgram->attribute("position"),3,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(shaderProgram->attribute("position"));

	glVertexAttribPointer(shaderProgram->attribute("color"), 4, GL_FLOAT, GL_FALSE,0,BUFFER_OFFSET(8*3*sizeof(GLfloat)));
	glEnableVertexAttribArray(shaderProgram->attribute("color"));

	glm::mat4 model; //define a transformation matrix for model in local coords
	
	glm::mat4 view	= glm::lookAt(
		glm::vec3(4.2f, 4.2f, 4.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);	//view matrix

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);	//projection matrix
	
	glm::mat4 MVP;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glEnable(GL_DEPTH_TEST); //wierd shit happens if you don't do this
	glEnable(GL_BLEND);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen

	//here comes event handling part
	SDL_Event e;
	bool quit = false;
	
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_QUIT:	//if X windowkey is pressed then quit
				quit = true;
			
			case SDL_KEYDOWN :	//if ESC is pressed then quit
				
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;

				case SDLK_w:
					cam.Move(FORWARD);
					std::cout << "W pressed \n";
					break;

				case SDLK_s:
					cam.Move(BACK);
					std::cout << "S pressed \n";
					break;

				case SDLK_a:
					cam.Move(LEFT);
					std::cout << "A pressed \n";
					break;

				case SDLK_d:
					cam.Move(RIGHT);
					std::cout << "D pressed \n";
					break;

				case SDLK_q:
					cam.Move(DOWN);
					std::cout << "Q pressed \n";
					break;

				case SDLK_e:
					cam.Move(UP);
					std::cout << "E pressed \n";
					break;

				}
				break;

			case SDL_MOUSEMOTION:
				
				cam.Move2D(e.motion.x,e.motion.y);
				std::cout << "mouse moved by x=" << e.motion.x << " y=" << e.motion.y << "\n";
				break;

			case SDL_MOUSEBUTTONDOWN:
				cam.SetPos(e.button.button, e.button.state, e.button.x, e.button.y);
				break;

			case SDL_MOUSEBUTTONUP:
				cam.SetPos(e.button.button, e.button.state, e.button.x, e.button.y);
				break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//following 2 lines define "intensity" of color, i.e ranging from 0 to highest
		GLfloat time = SDL_GetTicks() ;
		glUniform1f(uniColor, 1.0f);// (sin(time*0.01f) + 1.0f) / 2.0f);

		model = glm::rotate(glm::mat4(1),time*0.1f, glm::vec3(0, 0, 1));	//calculate on the fly
		MVP = proj*view*model;
		glUniformMatrix4fv(shaderProgram->uniform("MVP"), 1, FALSE, glm::value_ptr(cam.projection*cam.view*model/*MVP*/));
	
		glDrawElements(GL_TRIANGLES,size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		SDL_GL_SwapWindow(window);
//		if (1000 / FPS > SDL_GetTicks() - start)
//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}
	
	shaderProgram->disable();
	shaderProgram->~ShaderProgram();
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}

