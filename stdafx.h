#include<GL/glew.h>
#include<SDL.h>
//#include<SDL_mixer.h>
#include<SDL_image.h>
#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include<SDL_opengl.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<vector>
#include<algorithm>
#include<fstream>

const int FPS = 30;
#define GLSL(src) "#version 150 core\n" #src