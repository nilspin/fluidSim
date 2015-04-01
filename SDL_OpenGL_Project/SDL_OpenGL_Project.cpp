// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "camera.h"
#include "ShaderProgram.hpp"
#include "Mesh.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint VAO;
GLuint VBO;
GLuint positionAttribute, colAttrib, uniColor;

int main(int argc, char *argv[])
{
	/* Don't use camera for now*/
	Camera cam;
	cam.SetMode(FREE);
	cam.SetPosition(glm::vec3(0, 0, -10));
	cam.SetLookAt(glm::vec3(0, 0, 0));
	cam.SetClipping(.01, 50);
	cam.SetFOV(45);

#pragma region SDL_FUNCTIONS;
	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 100, 100, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Init GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

#pragma endregion SDL_FUNCTIONS;

	//Create Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/*Another VBO (this one is for fluid)*/
	GLfloat fluidwall[] = {		//DATA
	-1.0,-1.0,
	-1.0, 1.0,
	 1.0,-1.0,
	 1.0,-1.0,
	 1.0, 1.0,
	-1.0, 1.0
	};	//Don't need index data for this peasant mesh!
	
	GLuint fluid;//VBO for fluid wall
	glGenBuffers(1, &fluid);
	glBindBuffer(GL_ARRAY_BUFFER, fluid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fluidwall), &fluidwall, GL_STATIC_DRAW);
	
#pragma region FBO_FUNCTIONS
	GLuint FBO;
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//now create a texture
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
///*
	// The depth renderbuffer
	GLuint depthbuffer;
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 768, 0, GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthbuffer, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	//now set the list of draw buffers (here we just need 2- color and depth)
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);//this is finally where we tell the driver to draw to this paricular framebuffer

	//in case something goes wrong : 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	// The fullscreen quad's FBO
	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//this is default : draw to screen

#pragma endregion FBO_FUNCTIONS	

#pragma region SHADER_FUNCTIONS

	//=============================================================================================
	//Shader that contains main logic
	ShaderProgram *MainShader = new ShaderProgram();
	MainShader->initFromFiles("MainShader.vert", "MainShader.frag");
	MainShader->addAttribute("position");
	MainShader->addUniform("mousePos");
//	MainShader->addUniform("MVP");

	//another shader to sample from texture and draw on quadVBO
	ShaderProgram *quadProgram = new ShaderProgram();
	quadProgram->initFromFiles("quadProgram.vert","quadProgram.frag");
	quadProgram->addAttribute("quad_vertices");
	quadProgram->addUniform("textureSampler");

#pragma endregion SHADER_FUNCTIONS


	glEnable(GL_DEPTH_TEST);
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
		//Render to out custom FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		MainShader->use();	//Use this shader to write to textures first

		// The following line tells the CPU program that "vertexData" stuff goes into "posision"
		//parameter of the vertex shader. It also tells us how data is spread within VBO.
		glBindBuffer(GL_ARRAY_BUFFER, fluid);
		glVertexAttribPointer(MainShader->attribute("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(MainShader->attribute("position"));

		glUniform2f(MainShader->uniform("mousePos"), (int)e.motion.x, (int)e.motion.y);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


		//1st draw call
		glDrawArrays(GL_TRIANGLES, 0, 6);
//--------------------------------------------------------------------------------------
		//By now we have successfully rendered to our texture. We will now draw on screen
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0, 0, 1024, 768);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//use our quad shader
		quadProgram->use();

		//Bind out texture in texture unit #0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTexture);//depthbuffer

		//set our 'textureSampler' sampler to use texture unit 0
		glUniform1i(quadProgram->uniform("textureSampler"),0);

		//1st attribute : quad vertices
		glEnableVertexAttribArray(0);	//note that this corresponds to the layout=0 in shader
		glBindBuffer(GL_ARRAY_BUFFER,quad_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			0					// array buffer offset
			);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		//swap buffers
		SDL_GL_SwapWindow(window);
//		if (1000 / FPS > SDL_GetTicks() - start)
//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}
	
	MainShader->disable();
	MainShader->~ShaderProgram();
	quadProgram->disable();
	quadProgram->~ShaderProgram();
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}

