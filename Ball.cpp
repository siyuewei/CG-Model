#include "ball.h"
float Ball::moveTime;

void Ball::generate()
{
	sphereVertices.clear();
	sphereIndices.clear();
	// 生成球的顶点
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = center.x + radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = center.y + radius * std::cos(ySegment * PI);
			float zPos = center.z + radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			glm::vec3 vertex = glm::vec3(xPos, yPos, zPos);
			sphereVertices.push_back(vertex);
		}
	}

	// 生成球的Indices
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}
}


void Ball::setupBall()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//生成并绑定球体的VAO和VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), &sphereVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//解绑VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Ball::Ball(glm::vec3 center, float radius):center(center),radius(radius)
{
	moveTime = 0.0f;
	direction = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - center);

}

void Ball::Draw(Shader& shader)
{
	generate();
	setupBall();
	glBindVertexArray(VAO);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}



std::vector<glm::vec3> Ball::getVertices()
{
	return sphereVertices;
}

void Ball::explosion()
{

}

glm::vec3 Ball::getCenter()
{
	return center;
}

void Ball::update(float deltatime)
{
	float velocity = speed * deltatime;
	center += direction * velocity;
}

glm::vec3 Ball::getDirection()
{
	return direction;
}

float Ball::getRadius()
{
	return radius;
}
