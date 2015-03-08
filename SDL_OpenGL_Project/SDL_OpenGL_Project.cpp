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

	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 300, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
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


	GLushort indices[] = { 0, 1, 2,   2, 3, 0,	3, 2, 6,	6, 7, 3,	7, 6, 5,	5, 4, 7,	4, 5, 1,	1, 0, 4,	4, 0, 3,	3, 7, 4,	1, 5, 6,	6, 2, 1 };
	//define an index buffer
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

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
	
	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

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

/*Object loading code*/
//	Mesh *suzanne = new Mesh("suzanne.obj");
//	suzanne->upload();
//	vector<glm::vec4> suzanne_verts;
//	vector<glm::vec3> suzanne_normals;
//	vector<char16_t> suzanne_elements;

#pragma region SHADER_FUNCTIONS

	//=============================================================================================
	//Shader that contains main logic
	ShaderProgram *MainShader = new ShaderProgram();
	MainShader->initFromFiles("MainShader.vert", "MainShader.frag");
	MainShader->addAttribute("position");
	MainShader->addAttribute("color");
	MainShader->addUniform("MVP");

	//another shader to sample from texture and draw on quadVBO
	ShaderProgram *quadProgram = new ShaderProgram();
	quadProgram->initFromFiles("quadProgram.vert","quadProgram.frag");
	quadProgram->addAttribute("quad_vertices");
	quadProgram->addUniform("textureSampler");


#pragma endregion SHADER_FUNCTIONS


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
//		glViewport(0, 0, 1024, 768); //set up viewport
		/*Now specify the layout of the Vertex data */

		// The following line tells the CPU program that "vertexData" stuff goes into "posision"
		//parameter of the vertex shader. It also tells us how data is spread within VBO.
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(MainShader->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(MainShader->attribute("position"));

		glVertexAttribPointer(MainShader->attribute("color"), 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * 3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(MainShader->attribute("color"));
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		MainShader->use();	//Use this shader to write to textures first

#pragma region DATA_UPLOAD
		/*Now add necessary data to the GPU so shader can use it accordingly*/
		//following 2 lines define "intensity" of color, i.e ranging from 0 to highest
		GLfloat time = SDL_GetTicks() ;
//		glUniform1f(uniColor, 1.0f);// (sin(time*0.01f) + 1.0f) / 2.0f);

		model = glm::rotate(glm::mat4(1),time*0.005f, glm::vec3(0, 0, 1));	//calculate on the fly
		MVP = proj*view*model;
		glUniformMatrix4fv(MainShader->uniform("MVP"), 1, FALSE, glm::value_ptr(MVP));

#pragma endregion DATA_UPLOAD

		glDrawElements(GL_TRIANGLES,size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		//By now we have successfully rendered to our texture. We will now draw on screen
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0, 0, 1024, 768);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		//use our quad shader
		quadProgram->use();

		//Bind out texture in texture unit #0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTexture);

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
	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}

