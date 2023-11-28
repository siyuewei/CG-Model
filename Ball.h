#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shader.h"

const float PI = 3.14159265358979323846f;

//将球横纵划分成50X50的网格
const int Y_SEGMENTS = 20;
const int X_SEGMENTS = 20;


class Ball
{
private:
	glm::vec3 center;
	float radius;
	glm::vec3 direction;
	float speed = 0.2f;
	static float moveTime;
	std::vector<glm::vec3> sphereVertices;
	std::vector<int> sphereIndices;
	unsigned int VBO, EBO;
	void generate();
	void setupBall();
public:
	unsigned int VAO;
	bool stop = false;

	Ball(glm::vec3 center, float radius);
	void Draw(Shader& shader);
	void explosion(float deltatime);

	void update(float deltatime);
	std::vector<glm::vec3> getVertices();
	glm::vec3 getCenter();
	glm::vec3 getDirection();
	float getRadius();

};

