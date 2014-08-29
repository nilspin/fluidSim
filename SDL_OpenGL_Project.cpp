// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
}




int main(int argc, char *argv[])
{

	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 300, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}


	//Create Vertex Array Object
	GLuint VAO=0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Now create a vertex buffer object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//define vertices of our triangle
	float vertices[] = {
		0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Note that we use GL_STATIC_DRAW because we want to send data to GPU only once thoughout the
	//program



	/*Create and compile the Vertex shader */
	const char* vertexSource = GLSL(
		in vec2 position;
		in vec3 color;
		out vec3 Color;

	void main() {
		Color = color;
		gl_Position = vec4(position, 0.0, 1.0);
	}
	);

	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader,1,&vertexSource,NULL);
	glCompileShader(VertexShader);


	//Create and compile the fragment shader
	const char* fragSource = GLSL(
		in vec3 Color;
		out vec4 outColor;
		void main()
		{
			outColor = vec4(Color,1.0f);
		}
	);

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader,1,&fragSource,NULL);
	glCompileShader(fragShader);

	//Link vertex and fragment shaders into a single shader program
	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram,VertexShader);
	glAttachShader(ShaderProgram,fragShader);
	glBindFragDataLocation(fragShader,0,"outColor");

	glLinkProgram(ShaderProgram);
	glUseProgram(ShaderProgram);

	/*Now specify the layout of the Vertex data */

	// The following line tells the CPU program that "vertexData" stuff goes into "posision"
	//parameter of the vertex shader.
	GLint positionAttribute = glGetAttribLocation(ShaderProgram,"position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute,2,GL_FLOAT,GL_FALSE,5*sizeof(GLfloat),0);

	GLint colAttrib = glGetAttribLocation(ShaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,5*sizeof(GLfloat),(void*)(2 * sizeof(GLfloat)));

	//Changing value of 'uniform' in the fragment shader
	//GLint uniColor = glGetUniformLocation(ShaderProgram, "transparency");
	//glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

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
			if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
		}
		//clear screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

//		GLfloat time = SDL_GetTicks() / 100;
//		glUniform1f(uniColor,(sin(time*0.3f) + 1.0f )/2.0f);
		//draw tringles
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
		if (1000 / FPS > SDL_GetTicks() - start)
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - start));
	}
	

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}

