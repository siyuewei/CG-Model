#include "ball.h"
float Ball::moveTime;

void Ball::generate()
{
	// ������Ķ���
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = center.x + radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = center.y + radius * std::cos(ySegment * PI);
			float zPos = center.z + radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			sphereVertices.push_back(xPos);
			sphereVertices.push_back(yPos);
			sphereVertices.push_back(zPos);
		}
	}

	// �������Indices
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
	//���ɲ��������VAO��VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//���������ݰ�����ǰĬ�ϵĻ�����
	glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	//���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//���VAO��VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Ball::Ball(glm::vec3 center, float radius):center(center),radius(radius)
{
	moveTime = 0.0f;
	direction = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - center);
    generate();
	setupBall();
}

void Ball::Draw(Shader& shader)
{
	glBindVertexArray(VAO);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(0);
}

glm::mat4 Ball::getModelMatrix(float deltaTime)
{
	this->moveTime += deltaTime;
	float offset = speed * moveTime;
	glm::vec3 move = offset * direction;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, move);
	return model;
}
