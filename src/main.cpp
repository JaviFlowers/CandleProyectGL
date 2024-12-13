#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "CandleModel.h"
#include "RoomModel.h"
#include "ParticleEmitter.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

static glm::vec3 cameraPos(0.0f,0.0f,3.0f);
static glm::vec3 cameraFront(0.0f,0.0f,-4.0f);
static glm::vec3 cameraUp(0.0f,1.0f,0.0f);

float moveSpeed = 2.0f;
const float ROOM_MIN = -5.0f;
const float ROOM_MAX =  5.0f;


int main() {
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"Debug Candle and Particles",NULL,NULL);
    if(!window){
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(glewInit()!=GLEW_OK){
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Make sure we can see inside the room

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE); // Required for particle point sizing


    Shader roomShader("shaders/room.vert","shaders/room.frag");
    Shader candleShader("shaders/candle.vert","shaders/candle.frag");
    Shader particleShader("shaders/particle.vert","shaders/particle.frag");

    RoomModel room;
    CandleModel candle;
    ParticleEmitter coreFlameEmitter(500, EmitterType::CoreFlame);
    ParticleEmitter hazeEmitter(300, EmitterType::HeatHaze);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)800/(float)600,0.1f,100.0f);

    // Increase ambient light in the fragment shaders if too dark (done in shaders).
    // Temporarily, you can hardcode colors in candle.frag and particle.frag to ensure visibility.

    double lastTime = glfwGetTime();
    int frameCount = 0;
    float emissionRate = 300.0f;

    while(!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float dt = (float)(currentTime - lastTime);
        frameCount++;
        //FPS Display Bug
        if (currentTime - lastTime >= 1.0) { // One second has passed
        std::cout << "FPS: " << frameCount << std::endl;
        frameCount = 0;              // Reset the frame count
           // Reset the timer
    }
        lastTime = currentTime;
        


        glm::vec3 proposedPos = cameraPos;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            proposedPos += cameraFront * moveSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            proposedPos -= cameraFront * moveSpeed * dt;
        }

        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            proposedPos -= right * moveSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            proposedPos += right * moveSpeed * dt;
        }

        // Clamp camera inside room
        proposedPos.x = glm::clamp(proposedPos.x, ROOM_MIN+0.5f, ROOM_MAX-0.5f);
        proposedPos.z = glm::clamp(proposedPos.z, ROOM_MIN+0.5f, ROOM_MAX-0.5f);
        cameraPos = proposedPos;

        // Candle in front of camera
        glm::vec3 candlePos = cameraPos + cameraFront * 0.5f + glm::vec3(0.0f,-0.4f,0.0f);
        
        
        glm::vec3 flamePos = candlePos + glm::vec3(0.0f,0.3f,0.0f);
        coreFlameEmitter.setEmissionPosition(flamePos);
        hazeEmitter.setEmissionPosition(flamePos);
        // Update particles

        int coreCount = (int)(300 * dt);   // rate for core flame
        int hazeCount = (int)(100 * dt);   // rate for haze

        coreFlameEmitter.emit(coreCount);
        coreFlameEmitter.update(dt);

        hazeEmitter.emit(hazeCount);
        hazeEmitter.update(dt);


        

        glClearColor(0.05f,0.05f,0.05f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Draw Room
        roomShader.use();
        {
            glm::mat4 model = glm::mat4(1.0f);
            roomShader.setMat4("uProjection", proj);
            roomShader.setMat4("uView", view);
            roomShader.setMat4("uModel", model);

            // Set viewPos and lightPos for room
            roomShader.setVec3("lightPos", flamePos);
            roomShader.setVec3("viewPos", cameraPos);
            roomShader.setVec3("lightColor", glm::vec3(2.0f, 1.0f, 0.5f));; // warm flame color

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, room.albedoTexture);
            roomShader.setInt("uAlbedoMap",0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, room.normalTexture);
            roomShader.setInt("uNormalMap",1);

            room.draw();
        }

        // Draw Candle
        candleShader.use();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), candlePos);
            // Increase scale if candle too small
            model = glm::scale(model, glm::vec3(0.5f,0.5f,0.5f));
            candleShader.setVec3("viewPos", cameraPos);
            candleShader.setMat4("uProjection", proj);
            candleShader.setMat4("uView", view);
            candleShader.setMat4("uModel", model);
            candleShader.setVec3("lightPos", flamePos);
            candleShader.setVec3("lightColor", glm::vec3(2.0f, 1.0f, 0.5f));
            candleShader.setVec3("candleColor", glm::vec3(0.1f, 0.8f, 0.7f));

            glDisable(GL_BLEND);
            candle.draw();
            glEnable(GL_BLEND);
        }

        // Draw Particles
        particleShader.use();
        particleShader.setMat4("uProjection", proj);
        particleShader.setMat4("uView", view);
// The particle shader now uses gradient in frag; no uniform needed for color

        coreFlameEmitter.draw();
        hazeEmitter.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}
