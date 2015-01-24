#pragma once

//GL shit
#include <gl/glew.h>
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
#include <glm.hpp>
#include <sstream>
#include <string>


class Mesh{
private:
	GLuint vbo_vertices, vbo_normals, ibo_elements;

	vector<glm::vec4> vertices;
	vector<glm::vec3> normals;
	vector<GLushort> elements;
	glm::mat4 object2world;

	GLint attribute_v_coord = -1;
	GLint attribute_v_normal = -1;
	GLint uniform_m = -1, uniform_v = -1, uniform_p = -1;
	GLint uniform_m_3x3_inv_transp = -1, uniform_v_inv = -1;
public:
	Mesh();
	~Mesh();
	void upload();
	void draw();
	void draw_bbox();
};
