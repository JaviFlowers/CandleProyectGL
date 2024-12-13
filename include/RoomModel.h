#ifndef ROOM_MODEL_H
#define ROOM_MODEL_H

#include <GL/glew.h>

class RoomModel {
public:
    RoomModel();
    ~RoomModel();
    void draw();

    GLuint albedoTexture;
    GLuint normalTexture;

private:
    GLuint VAO, VBO;
    int vertexCount;

    GLuint loadTexture(const char* path);
};

#endif
