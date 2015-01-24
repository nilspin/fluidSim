#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <glm.hpp>
#include <sstream>
#include <string>
#include <GL\GLU.h>

class ObjLoader
{
public:
	ObjLoader();

	void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements);
	
	~ObjLoader();
};

