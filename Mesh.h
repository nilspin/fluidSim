//GL shit
#include <Windows.h>
#include <GL/glew.h>
#include <gl/GLU.h>
#include <gl/GL.h>

//GLM shit
#include <glm.hpp>
#include<gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

//std shit
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

#ifndef MESH_H
#define MESH_H

class Mesh{
public://private:
	GLuint vbo_vertices, vbo_normals, ibo_elements;

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLushort> elements;
	glm::mat4 object2world;

	GLushort *ptr = elements.data();
	GLint attribute_v_coord = -1;
	GLint attribute_v_normal = -1;
	GLint uniform_m = -1, uniform_v = -1, uniform_p = -1;
	GLint uniform_m_3x3_inv_transp = -1, uniform_v_inv = -1;
//public:
	Mesh();
	~Mesh();
	Mesh(const char* filename);
	void upload();
	void draw();
	void draw_bbox();
	void load_obj(const char* filename, std::vector<glm::vec4> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements);
};

#endif