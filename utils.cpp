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
    //result:��������ʱ��xOffsetһֱ��0��yOffset��1��-1

    camera.ProgressScroll(yOffset);
}

bool check_collision(Model model, Ball ball)
{
    //�ж��Ƿ�Ͱ�Χ���ཻ
    if (!check_collision_box_ball(model.box, ball)) {
        return false;
    }
    //std::cout << "collision with box" << std::endl;
   
    //�ж��Ƿ��model�ཻ
    for (Mesh mesh : model.meshes_out) {
        Triangle_indices result = check_collision_mesh_ball(mesh, ball);
        if (result.indice1 == 0 && result.indice2 == 0 && result.indice3 == 0) {
            continue;
        }
        else {
            std::cout << "Collision at triangle (" << mesh.vertices[result.indice1].Position.x << ", "
                << mesh.vertices[result.indice1].Position.y << ", "
                << mesh.vertices[result.indice1].Position.z << "), ("
                << mesh.vertices[result.indice2].Position.x << ", "
                << mesh.vertices[result.indice2].Position.y << ", "
                << mesh.vertices[result.indice2].Position.z << "), ("
                << mesh.vertices[result.indice3].Position.x << ", "
                << mesh.vertices[result.indice3].Position.y << ", "
                << mesh.vertices[result.indice3].Position.z << ")" << std::endl;
            return true;
        }
    }
    return false;
}

//�ཻ����true
bool check_collision_box_ball(Box box, Ball ball)
{
    //glm::vec3 tran_ball_center = glm::vec3(ball.getModelMatrix(0) * glm::vec4(ball.getCenter(), 1.0f));
    glm::vec3 tran_ball_center = ball.getCenter();
    float offset_x = max(box.min_x, min(tran_ball_center.x, box.max_x));
    float offset_y = max(box.min_y, min(tran_ball_center.y, box.max_y));
    float offset_z = max(box.min_z, min(tran_ball_center.z, box.max_z));

    float distance = (tran_ball_center.x - offset_x) * (tran_ball_center.x - offset_x) +
        (tran_ball_center.y - offset_y) * (tran_ball_center.y - offset_y) +
        (tran_ball_center.z - offset_z) * (tran_ball_center.z - offset_z);

    return distance < ball.getRadius();
}

float distance_point_line(glm::vec3& point, glm::vec3& start, glm::vec3& end)
{
    glm::vec3 edge = end - start;
    glm::vec3 start_to_point = point - start;

    glm::vec3 projection = glm::dot(edge, start_to_point) * glm::normalize(edge);
    float distance = glm::length(start_to_point - projection);
    return distance;
}

Triangle_indices check_collision_mesh_ball(Mesh mesh, Ball ball)
{
    //��mesh��ÿ�������Σ��ж��Ƿ������ײ
    //1.����������������ƽ��ľ��룬�ж��Ƿ�С�ڰ뾶����С����һ������ײ
    //2.���С�ڣ��ж�������������ƽ���ڵ�ͶӰ�Ƿ��� ��������������������뾶���� �γɵ�����������
    // 2.1 �ж�����ͶӰ�Ƿ���ԭ�������ڲ�������ھ���ײ
    // 2.2 ������ڣ����������������ߵľ��룬�����̾���С�ڰ뾶����ײ

    for (unsigned int i = 0; i < mesh.indices.size(); i += 3)
    {
        glm::vec3 ver_pos1 = mesh.vertices[mesh.indices[i]].Position;
        glm::vec3 ver_pos2 = mesh.vertices[mesh.indices[i + 1]].Position;
        glm::vec3 ver_pos3 = mesh.vertices[mesh.indices[i + 2]].Position;
        //glm::vec3 center = glm::vec3(ball.getModelMatrix(0) * glm::vec4(ball.getCenter(), 1.0f));
        glm::vec3 center = ball.getCenter();

        //���������η���(��������Ҳ��������)
        glm::vec3 normal = glm::normalize(glm::cross(ver_pos2 - ver_pos1, ver_pos3 - ver_pos1));
        float distance_center_triangle = glm::dot(ver_pos1 - center, normal);

        if (abs(distance_center_triangle) <= ball.getRadius()) {
            //std::cout << "The distance between ball and traingle is less than radius" << std::endl;
            //��������ͶӰ
            glm::vec3 projection = center - distance_center_triangle * normal;

            //ͨ������ͶӰ���Ƿ���������������ͬһ�����ж� ���Ƿ����������ڲ�
            float side1 = glm::dot(glm::cross(ver_pos2 - ver_pos3, projection - ver_pos3), normal);
            float side2 = glm::dot(glm::cross(ver_pos1 - ver_pos2, projection - ver_pos2), normal);
            float side3 = glm::dot(glm::cross(ver_pos3 - ver_pos1, projection - ver_pos1), normal);

            if ((side1 >= 0 && side2 >= 0 && side3 >= 0) || (side1 <= 0 && side2 <= 0 && side3 <= 0)) {
                std::cout << "The projection of center is in traingle " << std::endl;
                return Triangle_indices(mesh.indices[i], mesh.indices[i+1],mesh.indices[i+2]);
            }

            //�������������ߵ���̾���
            float min_distance = std::min(
                std::min(
                    distance_point_line(center, ver_pos1, ver_pos2), 
                    distance_point_line(center, ver_pos3, ver_pos2)
                ),
                distance_point_line(center, ver_pos1, ver_pos3)
            );
            if (min_distance <= ball.getRadius()) {
                std::cout << "The distance between center and edge is less than radius" << std::endl;
                return Triangle_indices(mesh.indices[i], mesh.indices[i + 1], mesh.indices[i + 2]);
            }
        }
    }

    return Triangle_indices(0, 0, 0);
}