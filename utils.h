#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <optional>

#include "Camera.h"
#include "Model.h"
#include "Ball.h"


// settings
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// camera
extern Camera camera;
extern float lastX;
extern float lastY;
extern bool firstMouse;

// timing
extern float deltaTime;
extern float lastFrame;

struct Triangle_indices {
	unsigned int indice1 = 0;
	unsigned int indice2 = 0;
	unsigned int indice3 = 0;

	Triangle_indices() = default;
	Triangle_indices(unsigned int indice1, unsigned int indice2, unsigned int indice3)
		:indice1(indice1), indice2(indice2), indice3(indice3){}

};

bool check_collision(Model &model, Ball &ball);
bool check_collision_box_ball(Box &box, Ball &ball);
Triangle_indices check_collision_mesh_ball(Mesh &mesh, Ball &ball);
float distance_point_line(glm::vec3& point, glm::vec3& start, glm::vec3& end);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


