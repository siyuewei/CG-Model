#pragma once

#include "shader.h"
#include <vector>

class Box
{
private:
	float min_x, max_x, min_y, max_y, min_z, max_z;
	unsigned int VAO, VBO, EBO;
	std::vector<glm::vec3> vertex_box;
	std::vector<unsigned int> indices_box;
	void setupBox();
public:
	Box();
	Box(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z);
	void Draw(Shader& shader);
};

