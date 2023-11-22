#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	
	float Yaw;
	float Pitch;

	float MoveSpeed = 2.5f;
	float MouseSensitivity = 0.1f;
	float ZoomSensitivity = 2.0f;
	float zoom = 45.0f;

public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp);
	Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch);
	glm::mat4 GetViewMatrix();
	void ProgressKeyBoard(Camera_Movement direction,float deltaTime);
	void ProgressMouseMovement(float xoffset, float yoffset, bool constrainPitch = true );
	void ProgressScroll(float yOffset);

	glm::vec3 getPosition();
	float getZoom();
private:
	void updateCameraVectors();

};

