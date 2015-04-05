// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "camera.h"
#include "ShaderProgram.hpp"
#include "Mesh.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint All_screen;
GLuint boundary,boundaryVBO,boundaryIndexVBO;
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
	int width = 640;
	int height = 480;
#pragma region SDL_FUNCTIONS;
	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 100, 100, width,height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
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
	SDL_Event e;
#pragma endregion SDL_FUNCTIONS;


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
	quadProgram->initFromFiles("quadProgram.vert", "quadProgram.frag");
	quadProgram->addAttribute("quad_vertices");
	quadProgram->addUniform("textureSampler");

#pragma endregion SHADER_FUNCTIONS

	//Create Vertex Array Object
	glGenVertexArrays(1, &All_screen);
	glBindVertexArray(All_screen);

	float px = 1.0 / width; px = 150 * px;
	float py = 1.0 / height;  py = 150 * py;
	float x = 1 - px;
	float y = 1 - py;
	/*Another VBO (this one is for fluid)*/
	GLfloat fluidwall[] = {		//DATA
	-x,-y,
	-x, y,
	 x,-y,
	 x,-y,
	 x, y,
	-x, y
	};	//Don't need index data for this peasant mesh!
	
	GLuint fluid;//VBO for fluid wall
	glGenBuffers(1, &fluid);
	glBindBuffer(GL_ARRAY_BUFFER, fluid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fluidwall), &fluidwall, GL_STATIC_DRAW);	
	//Assign attribs
	glVertexAttribPointer(MainShader->attribute("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(MainShader->attribute("position"));
	glBindVertexArray(0);	//unbind VAO

	//Another VAO for boundary
	glGenVertexArrays(1, &boundary);
	glBindVertexArray(boundary);
	float boundaryWall[] = {
/*		-1,1,
		1,1,
		1,-1,
		-1,-1
		-1,1-py,
		-1,py-1,
		px-1,1,
		1-px,1,
		1,1-py,
		1,py-1,
		1-px,-1,
		px-1,-1
*/
		//left
		-1,1,
		px-1,1,
		-1,-1,
		px-1,1,
		px-1,-1,
		-1,-1,
		//bottom
		-1,py-1,
		-1,-1,
		1,-1,
		-1,py-1,
		1,py-1,
		1,-1,
		//right
		1-px,1,
		1,1,
		1-px,-1,
		1,1,
		1-px,-1,
		1,-1,
		//up
		-1,1,
		1,1,
		-1,1-py,
		1,1,
		1,1-py,
		-1,1-py
	};
	GLuint boundaryVBO;//VBO for fluid wall
	glGenBuffers(1, &boundaryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boundaryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundaryWall), &boundaryWall, GL_STATIC_DRAW);

	/*indexVBO
	GLushort indices[] = {
		//left
		0,6,3,
		6,11,3,
		//bottom
		5,3,2,
		5,9,2,
		//right
		7,1,10,
		1,10,2,
		//up
		0,1,4,
		1,8,4
	};
	glGenBuffers(1, &boundaryIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundaryIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
	*/

	//Assign attribs
	glVertexAttribPointer(MainShader->attribute("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(MainShader->attribute("position"));
	glBindVertexArray(0);	//unbind VAO

#pragma region FBO_FUNCTIONS
	GLuint FBO;
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//now create a texture
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

	GLuint Velocity0;
	glGenTextures(1, &Velocity0);
	glBindTexture(GL_TEXTURE_2D, Velocity0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, Velocity0, 0);

	GLuint Velocity1;
	glGenTextures(1, &Velocity1);
	glBindTexture(GL_TEXTURE_2D, Velocity1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#1
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Velocity1, 0);

	GLuint Pressure0;
	glGenTextures(1, &Pressure0);
	glBindTexture(GL_TEXTURE_2D, Pressure0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, Pressure0, 0);

	GLuint Pressure1;
	glGenTextures(1, &Pressure1);
	glBindTexture(GL_TEXTURE_2D, Pressure1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, Pressure1, 0);

	GLuint divergence;
	glGenTextures(1, &divergence);
	glBindTexture(GL_TEXTURE_2D, divergence);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, 768, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, divergence, 0);

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen

	//here comes event handling part
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
		glBindVertexArray(boundary);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		//we need to do the following because unfortunately uniforms cannot be bound to VAOs
		glUniform2f(MainShader->uniform("mousePos"), (int)e.motion.x, (int)e.motion.y);

		//1st draw call
		glDrawArrays(GL_TRIANGLES,0,24);
		glBindVertexArray(0);//unbind VAO
//--------------------------------------------------------------------------------------
		//By now we have successfully rendered to our texture. We will now draw on screen
		glBindFramebuffer(GL_FRAMEBUFFER,0);
//		glViewport(0, 0, width, 768);

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

