#include "Camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp)
{
	this->Position = position;
	this->Front = front;
	this->WorldUp = worldUp;

	this->Right = glm::normalize(glm::cross(Front,WorldUp));
	this->Up = glm::normalize(glm::cross(Right,Front));
	
	this->Yaw = glm::degrees(atan2(Front.z, Front.x));
	this->Pitch = glm::degrees(asin(Front.y));
}

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
{
	this->Position = position;
	this->WorldUp = worldUp;
	this->Yaw = yaw;
	this->Pitch = pitch;
	
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	this->Front = glm::normalize(front);

	this->Right = glm::normalize(glm::cross(Front, WorldUp));
	this->Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProgressKeyBoard(Camera_Movement direction, float deltaTime)
{
	float velocity = MoveSpeed * deltaTime;
	if (direction == FORWARD) {
		Position += Front * velocity;
	}
	if (direction == BACKWARD) {
		Position += -Front * velocity;
	}
	if (direction == RIGHT) {
		Position += Right * velocity;
	}
	if (direction == LEFT) {
		Position += -Right * velocity;
	}
	if (direction == UP) {
		Position.y += velocity;
	}
	if (direction == DOWN) {
		Position.y -= velocity;
	}
	//Position.y = 0.0f;
}

void Camera::ProgressMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//std::cout << "xoffset: " << xoffset << std::endl;
	//std::cout << "yoffset: " << yoffset << std::endl;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch) {
		if (Pitch >= 89.0f) {
			Pitch = 89.0f;
		}
		if (Pitch <= -1.0f) {
			Pitch = -1.0f;
		}
	}

	updateCameraVectors();
}

void Camera::ProgressScroll(float yOffset)
{
	zoom += -(float)(yOffset)*ZoomSensitivity;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

glm::vec3 Camera::getPosition()
{
	return this->Position;
}

float Camera::getZoom()
{
	return this->zoom;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}