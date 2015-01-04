// SDL_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

GLuint VAO;
GLuint VBO;
GLuint positionAttribute, colAttrib, uniColor;

void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	
}

void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements) {
	ifstream in(filename, ios::in);
	if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }

	string line;
	while (getline(in, line)) {
		if (line.substr(0, 2) == "v ") {
			istringstream s(line.substr(2));
			glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
			vertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ") {
			istringstream s(line.substr(2));
			GLushort a, b, c;
			s >> a; s >> b; s >> c;
			a--; b--; c--;
			elements.push_back(a); elements.push_back(b); elements.push_back(c);
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}

	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for (int i = 0; i < elements.size(); i += 3) {
		GLushort ia = elements[i];
		GLushort ib = elements[i + 1];
		GLushort ic = elements[i + 2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
			glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;
	}
}


int main(int argc, char *argv[])
{

	Uint32 start = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);
	/*
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	*/
	SDL_Window *window = SDL_CreateWindow("SDL_project", 200, 300, 1024,768, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE );
	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << "Sorry, but GLEW failed to load.";
		return 1;
	}

	
	glGenBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	/*
	vector<glm::vec4> suzane_verts;
	vector<glm::vec3> suzane_normals;
	vector<GLushort>  suzane_elements;

	load_obj("suzanne.obj",suzane_verts,suzane_normals,suzane_elements);
	*/
	
	

	//Create Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Now create a vertex buffer object
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//define vertices of our triangle
	GLfloat vertices[] = {
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};
	GLfloat colors[] = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,0,3*3*sizeof(GLfloat),vertices);
	glBufferSubData(GL_ARRAY_BUFFER,3*3*sizeof(GLfloat),3*4*sizeof(GLfloat),colors);
	//Note that we use GL_STATIC_DRAW because we want to send data to GPU only once thoughout the
	//program
	

	glm::mat4 trans; //define a transformation matrix
	
#pragma region SHADER_FUNCTIONS

	///Create and compile the Vertex shader
	const char* vertexSource = GLSL(
		uniform float alpha;
		in vec2 position;
		in vec3 color;//uniform vec3 color;//
		out vec3 Color;
		out vec2 texCoord;

		uniform mat4 transform;

	void main() {
		Color = color*alpha;
		gl_Position = transform*vec4(position, 0.0, 1.0);
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

#pragma endregion SHADER_FUNCTIONS

	/*Now specify the layout of the Vertex data */

	// The following line tells the CPU program that "vertexData" stuff goes into "posision"
	//parameter of the vertex shader.
	positionAttribute = glGetAttribLocation(ShaderProgram,"position");
	glVertexAttribPointer(positionAttribute,3,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(positionAttribute);

	colAttrib = glGetAttribLocation(ShaderProgram, "color");
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE,0,BUFFER_OFFSET(3*3*sizeof(GLfloat)));
	glEnableVertexAttribArray(colAttrib);

	GLuint transformLoc = glGetUniformLocation(ShaderProgram, "transform");
	

	//Changing value of 'uniform' in the fragment shader
	GLint uniColor = glGetUniformLocation(ShaderProgram, "alpha");


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
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//following 2 lines define "intensity" of color, i.e ranging from 0 to highest
		GLfloat time = SDL_GetTicks() ;
		glUniform1f(uniColor,(sin(time*0.4f) + 1.0f )/2.0f);

		trans = glm::rotate(glm::mat4(1),time*0.01f, glm::vec3(0, 0, 1));
		glUniformMatrix4fv(transformLoc, 1, FALSE, glm::value_ptr(trans));
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

