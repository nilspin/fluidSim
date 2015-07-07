// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "camera.h"
#include "ShaderProgram.hpp"
#include "Mesh.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint All_screen, All_screenVBO;
GLuint boundary,boundaryVBO,boundaryIndexVBO;
GLuint inside, insideVBO, insideIndexVBO;
bool quit = false;

int main(int argc, char *argv[])
{
	int width = 640;
	int height = 480;
#pragma region SDL_FUNCTIONS;
	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 100, 100, width,height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//Init GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}
	SDL_Event e;
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);	//clear screen
#pragma endregion SDL_FUNCTIONS;

#pragma region SHADER_FUNCTIONS

	//=============================================================================================
	//Shader that contains main logic
	unique_ptr<ShaderProgram> MainShader(new ShaderProgram());
	MainShader->initFromFiles("MainShader.vert", "MainShader.frag");
	MainShader->addAttribute("position");
	MainShader->addUniform("mousePos");
	//	MainShader->addUniform("MVP");

	//another shader to sample from texture and draw on quadVBO
	unique_ptr<ShaderProgram> quadProgram(new ShaderProgram());
	quadProgram->initFromFiles("quadProgram.vert", "quadProgram.frag");
	quadProgram->addAttribute("quad_vertices");
	quadProgram->addUniform("texturesampler");
//	quadProgram->addUniform("velocity0");
//	quadProgram->addUniform("pressure0");

	//advect velocity--1
	unique_ptr<ShaderProgram> advectVelocity(new ShaderProgram());
	advectVelocity->initFromFiles("MainShader.vert", "advectVelocity.frag");
	advectVelocity->addAttribute("position");
	advectVelocity->addUniform("velocity1");
	advectVelocity->addUniform("Ink");

	//advect velocity boundary--2
	unique_ptr<ShaderProgram> velocityBoundary(new ShaderProgram());
	velocityBoundary->initFromFiles("boundary.vert", "velocityBoundary.frag");
	velocityBoundary->addAttribute("position");
	velocityBoundary->addAttribute("offset");
	velocityBoundary->addUniform("velocity1");

	//add force--3
	unique_ptr<ShaderProgram> addForce(new ShaderProgram());
	addForce->initFromFiles("MainShader.vert","addForce.frag");
	addForce->addAttribute("position");
	addForce->addUniform("mousePos");
	addForce->addUniform("differenceLastPos");
	addForce->addUniform("velocity0");

	//divergence shader--4
	unique_ptr<ShaderProgram> divergenceShader(new ShaderProgram());
	divergenceShader->initFromFiles("MainShader.vert","divergence.frag");
	divergenceShader->addAttribute("position");
	divergenceShader->addUniform("velocity0");

	//jacobi solver shader --5
	unique_ptr<ShaderProgram> jacobiSolver(new ShaderProgram());
	jacobiSolver->initFromFiles("MainShader.vert", "jacobiSolver.frag");
	jacobiSolver->addAttribute("position");
	jacobiSolver->addUniform("pressure0");
	jacobiSolver->addUniform("divergence");

	//pressure boundary shader --6 this is same as #5 but it acts on boundary only
	unique_ptr<ShaderProgram> pressureBoundary(new ShaderProgram());
	pressureBoundary->initFromFiles("boundary.vert", "jacobiSolverBoundary.frag");
	pressureBoundary->addAttribute("position");
	pressureBoundary->addAttribute("offset");
	pressureBoundary->addUniform("pressure0");
//	pressureBoundary->addUniform("divergence");

	//subtract pressure gradient --7
	unique_ptr<ShaderProgram> subtractPressureGradient(new ShaderProgram());
	subtractPressureGradient->initFromFiles("MainShader.vert","subtractPressureGradient.frag");
	subtractPressureGradient->addAttribute("position");
	subtractPressureGradient->addUniform("pressure0");
	subtractPressureGradient->addUniform("velocity0");

	//copy v1 to v0 --8
	unique_ptr<ShaderProgram> texCopyShader(new ShaderProgram());
	texCopyShader->initFromFiles("MainShader.vert", "texCopyShader.frag");
	texCopyShader->addAttribute("position");
	texCopyShader->addUniform("velocity1");


#pragma endregion SHADER_FUNCTIONS

#pragma region MESH_DATA

	//Create Vertex Array Object
	glGenVertexArrays(1, &All_screen);
	glBindVertexArray(All_screen);

	GLfloat canvas[] = {		//DATA
		-1.0f,-1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};	//Don't need index data for this peasant mesh!

	GLuint All_screenVBO;//VBO for fluid wall
	glGenBuffers(1, &All_screenVBO);
	glBindBuffer(GL_ARRAY_BUFFER, All_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvas), &canvas, GL_STATIC_DRAW);
	//Assign attribs
	glVertexAttribPointer(MainShader->attribute("position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(MainShader->attribute("position"));
	glBindVertexArray(0);	//unbind VAO

	//Create Vertex Array Object
	glGenVertexArrays(1, &inside);
	glBindVertexArray(inside);

	float px = 1.0 / width; px = 15 * px;
	float py = 1.0 / height;  py = 15 * py;
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
	GLfloat boundaryWall[] = {

		//left
		-1, 1, 1, 0,
		px - 1, 1, 1, 0,
		-1, -1, 1, 0,
		px - 1, 1, 1, 0,
		px - 1, -1, 1, 0,
		-1, -1, 1, 0,
		//bottom
		-1,py-1, 0, 1,
		-1, -1, 0, 1,
		1, -1, 0, 1,
		-1, py - 1, 0, 1,
		1, py - 1, 0, 1,
		1, -1, 0, 1,
		//right
		1-px,1, -1,0,
		1, 1, -1, 0,
		1 - px, -1, -1, 0,
		1, 1, -1, 0,
		1 - px, -1, -1, 0,
		1, -1, -1, 0,
		//up
		-1,1,0,-1,
		1, 1, 0, -1,
		-1, 1 - py, 0, -1,
		1, 1, 0, -1,
		1, 1 - py, 0, -1,
		-1, 1 - py, 0, -1,
	};
	GLuint boundaryVBO;//VBO for fluid wall
	glGenBuffers(1, &boundaryVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boundaryVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boundaryWall), &boundaryWall, GL_STATIC_DRAW);


	//Assign attribs
	glVertexAttribPointer(velocityBoundary->attribute("position"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(velocityBoundary->attribute("position"));
	glVertexAttribPointer(velocityBoundary->attribute("offset"), 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), BUFFER_OFFSET(2*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(velocityBoundary->attribute("offset"));
	glBindVertexArray(0);	//unbind VAO

#pragma endregion MESH_DATA

#pragma region FBO_FUNCTIONS
	GLuint MainFBO;
	glGenFramebuffers(1,&MainFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, MainFBO);

	//now create a texture
	GLuint renderTexture;
	glGenTextures(1, &renderTexture);
	glBindTexture(GL_TEXTURE_2D, renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, renderTexture, 0);
///*
	// The depth renderbuffer
	GLuint depthbuffer;
	glGenTextures(1, &depthbuffer);
	glBindTexture(GL_TEXTURE_2D, depthbuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthbuffer, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	GLuint Velocity0;
	glGenTextures(1, &Velocity0);
	glBindTexture(GL_TEXTURE_2D, Velocity0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Velocity0, 0);

	GLuint Velocity1;
	glGenTextures(1, &Velocity1);
	glBindTexture(GL_TEXTURE_2D, Velocity1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set renderTexture as our color attachment#1
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, Velocity1, 0);

	GLuint Pressure0;
	glGenTextures(1, &Pressure0);
	glBindTexture(GL_TEXTURE_2D, Pressure0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Pressure0, 0);

	GLuint Pressure1;
	glGenTextures(1, &Pressure1);
	glBindTexture(GL_TEXTURE_2D, Pressure1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, Pressure1, 0);

	GLuint divergence;
	glGenTextures(1, &divergence);
	glBindTexture(GL_TEXTURE_2D, divergence);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	//filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//set renderTexture as our color attachment#0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, divergence, 0);

	//now set the list of draw buffers
	GLenum drawBuffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, drawBuffers);//this is finally where we tell the driver to draw to this paricular framebuffer

	//in case something goes wrong : 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	//Jacobi_iter_FBO_1  : this FBO will be bound for stages 5 and 6
	GLuint Jacobi_iter_FBO_1;
	glGenFramebuffers(1, &Jacobi_iter_FBO_1);
	glBindFramebuffer(GL_FRAMEBUFFER, Jacobi_iter_FBO_1);

	glBindTexture(GL_TEXTURE_2D, divergence);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, divergence, 0);

	glBindTexture(GL_TEXTURE_2D, Pressure0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Pressure0, 0);

	glBindTexture(GL_TEXTURE_2D, Pressure1);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Pressure1, 0);

	GLenum Jacobi_iter_1[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, Jacobi_iter_1);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	//Jacobi_iter_FBO_2  : this FBO will be bound for stages 5 and 6
	GLuint Jacobi_iter_FBO_2;
	glGenFramebuffers(1, &Jacobi_iter_FBO_2);
	glBindFramebuffer(GL_FRAMEBUFFER, Jacobi_iter_FBO_2);

	glBindTexture(GL_TEXTURE_2D, divergence);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, divergence, 0);

	glBindTexture(GL_TEXTURE_2D, Pressure1);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, Pressure1, 0);

	glBindTexture(GL_TEXTURE_2D, Pressure0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, Pressure0, 0);

	GLenum Jacobi_iter_2[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, Jacobi_iter_2);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// The fullscreen quad's VBO
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
	
	while (!quit)
	{
#pragma region EVENT_HANDLING

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
					std::cout << "W pressed \n";
					break;

				case SDLK_s:
					std::cout << "S pressed \n";
					break;

				case SDLK_a:
					std::cout << "A pressed \n";
					break;

				case SDLK_d:
					std::cout << "D pressed \n";
					break;

				case SDLK_q:
					std::cout << "Q pressed \n";
					break;

				case SDLK_e:
					std::cout << "E pressed \n";
					break;

				}
				break;

			case SDL_MOUSEMOTION:
				
				std::cout << "mouse moved by x=" << e.motion.xrel << " y=" << e.motion.yrel << "\n";
				break;

			}
		}
#pragma endregion EVENT_HANDLING

#pragma region RTT_MAIN
/*/		THIS BLOCK IS FOR TESTING ONLY. Do not remove.
		glBindFramebuffer(GL_FRAMEBUFFER, MainFBO);
		MainShader->use();
		glUniform2f(MainShader->uniform("mousePos"), (int)e.motion.x, (int)e.motion.y);
		glBindVertexArray(All_screen);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
*/

		//Render to out custom MainFBO
		glBindFramebuffer(GL_FRAMEBUFFER, MainFBO);

		//stage 1------------------------------Force addition

		addForce->use();
		//we need to do the following because unfortunately uniforms cannot be bound to VAOs
		glUniform2f(addForce->uniform("mousePos"), (int)e.motion.x, (int)e.motion.y);
		glUniform2f(addForce->uniform("differenceLastPos"), (int)e.motion.xrel, (int)e.motion.yrel);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity0);	//add V0 as input texture
		glUniform1i(addForce->uniform("velocity0"), 0);
		glBindVertexArray(inside);
		//2nd draw call
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//stage 2--------------------------------Advection
		advectVelocity->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity1);
		glUniform1i(advectVelocity->uniform("velocity1"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Velocity1);
		glUniform1i(advectVelocity->uniform("Ink"), 1);
		glBindVertexArray(inside);
		//1st draw call
		glDrawArrays(GL_TRIANGLES,0,6);
		glBindVertexArray(0);//unbind VAO
		glBindTexture(GL_TEXTURE_2D, 0);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//stage 3-----------------------------boundary for velocity
		glBindVertexArray(boundary);
		
		velocityBoundary->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity1);
		glUniform1i(velocityBoundary->uniform("velocity1"),0);
		glBindVertexArray(boundary);

		glDrawArrays(GL_TRIANGLES, 0, 24);
		glBindVertexArray(0);//unbind VAO
		glBindTexture(GL_TEXTURE_2D, 0);
		
		//stage 4--------------------------------divergence calculation


		divergenceShader->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity0);
		glUniform1i(divergenceShader->uniform("velocity0"), 0);
		glBindVertexArray(inside);
		//4th draw call
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);


		//stage 5--------------------------Pressure computation
		auto tempFBO = Jacobi_iter_FBO_1;
		auto tempPressure = Pressure0;
		for (auto i = 0; i < 10; i++)
		{ 
			glBindFramebuffer(GL_FRAMEBUFFER, tempFBO);
			jacobiSolver->use();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, divergence);
			glUniform1i(jacobiSolver->uniform("divergence"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tempPressure);
			glUniform1i(jacobiSolver->uniform("pressure0"), 1);

			glBindVertexArray(inside);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			//Stage 6----------------------------------Pressure boundary
			pressureBoundary->use();
			glBindVertexArray(boundary);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Pressure0);
			glUniform1i(pressureBoundary->uniform("pressure0"), 0);

			glDrawArrays(GL_TRIANGLES, 0, 24);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//swap pressure textures
			tempPressure = Pressure1;
			Pressure1 = Pressure0;
			Pressure0 = tempPressure;

			//swap FBOs
			tempFBO = Jacobi_iter_FBO_2;
			Jacobi_iter_FBO_2 = Jacobi_iter_FBO_1;
			Jacobi_iter_FBO_1 = tempFBO;
		}

		//stage 6----------------Subtraction of pressure gradient to get divergence free velocity
		glBindFramebuffer(GL_FRAMEBUFFER, MainFBO);

		subtractPressureGradient->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Pressure0);
		glUniform1i(subtractPressureGradient->uniform("pressure0"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Velocity0);
		glUniform1i(subtractPressureGradient->uniform("velocity0"), 1);

		glBindVertexArray(inside);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//stage 7---- copy v1 to v0
		texCopyShader->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity1);
		glUniform1i(texCopyShader->uniform("velocity1"), 0);


		glBindVertexArray(inside);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


#pragma endregion RTT_MAIN

#pragma region DRAW_TO_SCREEN
//--------------------------------------------------------------------------------------
		//By now we have successfully rendered to our texture. We will now draw on screen
		glBindFramebuffer(GL_FRAMEBUFFER,0);


		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//use our quad shader
		quadProgram->use();

		//Bind out texture in texture unit #0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Velocity0);
		glUniform1i(quadProgram->uniform("texturesampler"),0);

//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, Pressure0);
//		glUniform1i(quadProgram->uniform("pressure0"), 1);

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

#pragma endregion DRAW_TO_SCREEN
	
		SDL_GL_SwapWindow(window);
//		if (1000 / FPS > SDL_GetTicks() - start)
//			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}
	
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}