#include "Box.h"

void Box::setupBox()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_box.size() * sizeof(glm::vec3), &vertex_box[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_box.size() * sizeof(unsigned int), &indices_box[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Box::setupPlane()
{
	glGenVertexArrays(1, &VAO_line);
	glGenBuffers(1, &VBO_line);
	glGenBuffers(1, &EBO_line);

	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, vertex_plane.size() * sizeof(glm::vec3), &vertex_plane[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_plane.size() * sizeof(unsigned int), &indices_plane[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Box::Box()
{
}

Box::Box(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z):
	min_x(min_x),max_x(max_x),min_y(min_y),max_y(max_y),min_z(min_z),max_z(max_z)
{
	glm::vec3 vertex_tmp;
	vertex_tmp = glm::vec3(min_x, max_y, max_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(max_x, max_y, max_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(max_x, max_y, min_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(min_x, max_y, min_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(min_x, min_y, max_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(max_x, min_y, max_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(max_x, min_y, min_z);
	vertex_box.push_back(vertex_tmp);
	vertex_tmp = glm::vec3(min_x, min_y, min_z);
	vertex_box.push_back(vertex_tmp);

	// Define the indices for a unit cube
	unsigned int indices[] = {
		0, 1, 3, 1, 2, 3,
		0, 3, 7, 0, 4, 7,
		0, 1, 5, 0, 4, 5,
		1, 2, 6, 1, 5, 6,
		2, 3, 7, 2, 6, 7,
		4, 5, 6, 4, 6, 7
	};
	unsigned int numIndices = sizeof(indices) / sizeof(indices[0]);
	indices_box.clear();
	for (unsigned int i = 0; i < numIndices; ++i)
	{
		indices_box.push_back(indices[i]);
	}

	setupBox();

	float line_min_x = (max_x - min_x) / 2 - 2 * (max_x - min_x);
	float line_max_x = (max_x - min_x) / 2 + 2 * (max_x - min_x);
	float line_min_z = (max_z - min_z) / 2 - 2 * (max_z - min_z);
	float line_max_z = (max_z - min_z) / 2 + 2 * (max_z - min_z);
	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
	vertex_tmp = glm::vec3(line_max_x, min_y, line_max_z);
	vertex_plane.push_back(vertex_tmp);//Î»ÖÃ
	vertex_plane.push_back(normal);//·¨Ïß
	vertex_tmp = glm::vec3(line_max_x, min_y, line_min_z);
	vertex_plane.push_back(vertex_tmp);
	vertex_plane.push_back(normal);
	vertex_tmp = glm::vec3(line_min_x, min_y, line_min_z);
	vertex_plane.push_back(vertex_tmp);
	vertex_plane.push_back(normal);
	vertex_tmp = glm::vec3(line_min_x, min_y, line_max_z);
	vertex_plane.push_back(vertex_tmp);
	vertex_plane.push_back(normal);

	unsigned int line_indices[] = {
		0,1,2,
		0,2,3
	};
	unsigned int line_numIndices = sizeof(line_indices) / sizeof(line_indices[0]);
	indices_plane.clear();
	for (unsigned int i = 0; i < line_numIndices; ++i)
	{
		indices_plane.push_back(indices[i]);
	}

	setupPlane();
}

void Box::Draw(Shader& shader)
{
	glBindVertexArray(VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indices_box.size(), GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}

void Box::DrawPlane(Shader& shader)
{
	glBindVertexArray(VAO_line);
	glDrawElements(GL_TRIANGLES, indices_plane.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Box::isInBox(glm::vec3 vertex)
{
	if (vertex.x >= min_x && vertex.x <= max_x &&
		vertex.y >= min_y && vertex.y <= max_y &&
		vertex.z >= min_z && vertex.z <= max_z) {
		return true;
	}
	else
	{
		return false;
	}
}
