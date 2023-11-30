#pragma once

#include "shader.h"
#include <vector>

class Box
{
private:
	unsigned int VAO, VBO, EBO;
	unsigned int VAO_line, VBO_line, EBO_line;
	std::vector<glm::vec3> vertex_box;
	std::vector<unsigned int> indices_box;

	std::vector<glm::vec3> vertex_plane; //包含位置和法线
	std::vector<unsigned int> indices_plane;

	void setupBox();
	void setupPlane();
public:
	float min_x, max_x, min_y, max_y, min_z, max_z;

	Box();
	Box(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z);
	void Draw(Shader& shader);
	void DrawPlane(Shader& shader);
	bool isInBox(glm::vec3 vertex);
};

