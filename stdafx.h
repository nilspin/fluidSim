#include<GL/glew.h>
#include<SDL.h>
//#include<SDL_mixer.h>
#include<iostream>
#include<cstring>
#include<string>
#include<SDL_opengl.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<vector>
#include<fstream>
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include<sstream>

const int FPS = 30;
#define GLSL(src) "#version 150 core\n" #src