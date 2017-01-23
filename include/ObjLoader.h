#ifndef _OBJLOADER_H
#define _OBJLOADER_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <GL/glu.h>

class ObjLoader
{
public:
	//public data
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLushort> elements;

	glm::vec3 *ptrToNormal = normals.data();

	//public methods
	ObjLoader();

	ObjLoader(const char*);

	void load_obj(const char* filename, std::vector<glm::vec4> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements);
	
	~ObjLoader();
};
#endif
