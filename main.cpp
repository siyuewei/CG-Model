#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "Model.h"
#include "Ball.h"
#include "utils.h"

#include <iostream>

bool start = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader outShader("model_out.vs", "model_out.fs");
    Shader inShader("model_in.vs", "model_in.fs");
    Shader ballShader("ball.vs", "ball.fs");
    Shader stripShader("strip.vs", "strip.fs");
    Shader fragmentShader("strip.vs", "strip.fs");


    // load models
    // -----------
    Model ourModel("models/bunny_simplify.obj");
    //Model testModel("nanosuit/nanosuit.obj");
    Ball ball(glm::vec3(1.5f, 0.3f, 0.0f), 0.01f);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        if (start) {
            // render
 // ------
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // create transformations
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            view = camera.GetViewMatrix();
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            if (check_collision(ourModel, ball)) {
                ourModel.explosion(deltaTime);
                ball.explosion(deltaTime);
            }

            // get matrix's uniform location and set matrix
            outShader.use();
            outShader.setMatrix4fv("model", 1, model);
            outShader.setMatrix4fv("view", 1, view);
            outShader.setMatrix4fv("projection", 1, projection);
            outShader.setFloatVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            outShader.setFloatVec3("lightPos", lightPos);
            outShader.setFloatVec3("viewPos", camera.getPosition());
            ourModel.DrawOut(outShader);
            ourModel.DrawBox(outShader);

            if (ourModel.is_coll) {
                ourModel.updateFragment(deltaTime);
                fragmentShader.use();
                fragmentShader.setMatrix4fv("model", 1, model);
                fragmentShader.setMatrix4fv("view", 1, view);
                fragmentShader.setMatrix4fv("projection", 1, projection);
                fragmentShader.setFloatVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
                fragmentShader.setFloatVec3("lightPos", lightPos);
                fragmentShader.setFloatVec3("viewPos", camera.getPosition());
                ourModel.DrawFragment(fragmentShader);
            }

            inShader.use();
            inShader.setMatrix4fv("model", 1, model);
            inShader.setMatrix4fv("view", 1, view);
            inShader.setMatrix4fv("projection", 1, projection);
            inShader.setFloatVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
            inShader.setFloatVec3("lightPos", lightPos);
            inShader.setFloatVec3("viewPos", camera.getPosition());
            ourModel.DrawIn(inShader);

            ballShader.use();
            ballShader.setMatrix4fv("model", 1, model);
            ballShader.setMatrix4fv("view", 1, view);
            ballShader.setMatrix4fv("projection", 1, projection);
            ball.update(deltaTime);
            ball.Draw(ballShader);

            stripShader.use();
            stripShader.setMatrix4fv("model", 1, model);
            stripShader.setMatrix4fv("view", 1, view);
            stripShader.setMatrix4fv("projection", 1, projection);
            ourModel.DrawStrip(stripShader);
        }
 


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
