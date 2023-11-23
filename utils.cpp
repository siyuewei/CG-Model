#include "utils.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProgressKeyBoard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProgressKeyBoard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProgressKeyBoard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProgressKeyBoard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.ProgressKeyBoard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProgressKeyBoard(DOWN, deltaTime);
    }
}



// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float offsetX = xPos - lastX;
    float offsetY = lastY - yPos;
    //std::cout << "offsetX: " << offsetX << std::endl;
    //std::cout << "offsetY: " << offsetY << std::endl;

    lastX = xPos;
    lastY = yPos;

    camera.ProgressMouseMovement(offsetX, offsetY);

}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    //std::cout << "xoffset:" << xOffset << std::endl;
    //std::cout << "yoffset:" << yOffset << std::endl;
    //result:滚动滚轮时，xOffset一直是0，yOffset是1或-1

    camera.ProgressScroll(yOffset);
}

std::shared_ptr<glm::vec3> check_collision(Model model, Ball ball)
{
    //判断是否和包围盒相交
    std::shared_ptr<glm::vec3> collosion_box_ball = check_collision_box_ball(model.box, ball);
    if (collosion_box_ball == nullptr) {
        return nullptr;
    }
    std::cout << "collision with box" << std::endl;
    //判断是否和model相交
    for (Mesh mesh : model.meshes_out) {
        std::shared_ptr<glm::vec3> collosion_mesh_ball = check_collision_mesh_ball(mesh, ball);
        if (collosion_mesh_ball) {
            return collosion_mesh_ball;
        }
    }
    return nullptr;
}

std::shared_ptr<glm::vec3> check_collision_box_ball(Box box, Ball ball)
{
    for (glm::vec3 vertex : ball.getVertices()) {
        glm::vec3 tran_ball_vertex = glm::vec3(ball.getModelMatrix(0) * glm::vec4(vertex, 1.0f));
        if (box.isInBox(tran_ball_vertex)) {
            return std::make_shared<glm::vec3>(vertex);
        }
    }
    return nullptr;
}

std::shared_ptr<glm::vec3> check_collision_mesh_ball(Mesh mesh, Ball ball)
{
    for (glm::vec3 ball_vertex : ball.getVertices()) {
        //只要选运动方向朝向那半面的点计算就可以，减少一般计算量
        glm::vec3 center_to_ball_vertex = ball_vertex - ball.getCenter();
        if (glm::dot(center_to_ball_vertex, ball.getDirection()) <= 0) {
            continue;
        }
        //判断剩下的点是否和mesh相撞
        glm::vec3 tran_ball_vertex = glm::vec3(ball.getModelMatrix(0) * glm::vec4(ball_vertex, 1.0f));
        for (Vertex mesh_vertex : mesh.vertices) {
            //对球上每个点做一个小包围盒，只有位于这个包围盒内的mesh点才做检测
            //否则直接对所有mesh遍历，下面用法线方向判断就不正确，因为一个model的所有顶点法线必然是各个朝向都有的
            //那无论如何都会判读成相撞
            float aabb_length = 0.01f;
            if (mesh_vertex.Position.x >= tran_ball_vertex.x - aabb_length && mesh_vertex.Position.x <= tran_ball_vertex.x + aabb_length &&
                mesh_vertex.Position.y >= tran_ball_vertex.y - aabb_length && mesh_vertex.Position.y <= tran_ball_vertex.y + aabb_length &&
                mesh_vertex.Position.z >= tran_ball_vertex.z - aabb_length && mesh_vertex.Position.z <= tran_ball_vertex.z + aabb_length) {
                glm::vec3 ball_to_mesh = mesh_vertex.Position - tran_ball_vertex;
                if (glm::dot(ball_to_mesh, mesh_vertex.Normal) > 0) {
                    return std::make_shared<glm::vec3>(mesh_vertex.Position);
                }
            }
            else {
                continue;
            }
        }
    }
    return nullptr;
}