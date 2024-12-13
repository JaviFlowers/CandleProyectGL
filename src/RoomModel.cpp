#include "RoomModel.h"
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// We define each face as 4 vertices. For normal mapping, we need tangent & bitangent.
// We'll assume the texture coordinates align such that x increases in U direction and y in V direction for walls.
// For each face, we can deduce a suitable tangent and bitangent.

// Each vertex now: pos(3), normal(3), texCoord(2), tangent(3), bitangent(3) = total 14 floats per vertex
// We'll define a helper to add a face with known orientation and tangents.

static void AddFace(std::vector<float> &vertices,
                    float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float x4, float y4, float z4,
                    float nx, float ny, float nz,
                    // We assume a simple mapping: top-left is (0,0), top-right is (1,0), bottom-right(1,1), bottom-left(0,1)
                    float tx, float ty, float tz,
                    float bx, float by, float bz)
{
    // uv mapping: we define a quad with (0,0) top-left, (1,0) top-right, (1,1) bottom-right, (0,1) bottom-left
    // We'll add vertices in a "triangle fan" order: v1-v2-v3-v4 forming a quad.

    auto addVertex = [&](float x, float y, float z, float u, float v) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); // pos
        vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz); // normal
        vertices.push_back(u); vertices.push_back(v); // texCoord
        vertices.push_back(tx); vertices.push_back(ty); vertices.push_back(tz); // tangent
        vertices.push_back(bx); vertices.push_back(by); vertices.push_back(bz); // bitangent
    };

    // Top-left (0,0)
    addVertex(x1,y1,z1, 0.0f,0.0f);
    // Top-right (1,0)
    addVertex(x2,y2,z2, 1.0f,0.0f);
    // Bottom-right (1,1)
    addVertex(x3,y3,z3, 1.0f,1.0f);
    // Bottom-left (0,1)
    addVertex(x4,y4,z4, 0.0f,1.0f);
}

RoomModel::RoomModel() : VAO(0), VBO(0), vertexCount(0) {
    float size = 5.0f; // half-room size

    // We'll build the room from 6 faces: floor, ceiling, left wall, right wall, front wall, back wall
    // For tangent and bitangent:
    // If we consider that the U axis goes along the X direction, and V along the Z direction (for floor/ceiling),
    // we pick tangent = (1,0,0) and bitangent = (0,0,1).
    // For walls, we must choose accordingly to map U along horizontal and V along vertical for instance.

    std::vector<float> vertices;

    // Floor (y=-size), normal=(0,1,0)
    // Texture plane along x and z: U -> x increasing, V -> z increasing
    AddFace(vertices,
        -size, -size, -size,
         size, -size, -size,
         size, -size,  size,
        -size, -size,  size,
        0,1,0,
        1,0,0, // tangent along X
        0,0,1  // bitangent along Z
    );

    // Ceiling (y=+size), normal=(0,-1,0)
    // U->x, V->-z to keep consistent
    // For simplicity, we still say tangent=(1,0,0) and bitangent=(0,0,-1) since if we look down, z axis inverts.
    AddFace(vertices,
        -size, size,  size,
         size, size,  size,
         size, size, -size,
        -size, size, -size,
        0,-1,0,
        1,0,0,
        0,0,-1
    );

    // Left wall (x=-size), normal=(1,0,0) if we want inside normals pointing inward
    // Let's say U->z, V->y. 
    // If U is along Z increasing and V along Y increasing, tangent=(0,0,1), bitangent=(0,1,0).
    AddFace(vertices,
        -size, -size,  size,
        -size,  size,  size,
        -size,  size, -size,
        -size, -size, -size,
        1,0,0,
        0,0,1,
        0,1,0
    );

    // Right wall (x=+size), normal=(-1,0,0)
    // Similarly U->-z, V->y: tangent=(0,0,-1), bitangent=(0,1,0)
    AddFace(vertices,
         size, -size, -size,
         size,  size, -size,
         size,  size,  size,
         size, -size,  size,
        -1,0,0,
        0,0,-1,
        0,1,0
    );

    // Front wall (z=-size), normal=(0,0,1)
    // If we choose U->x, V->y: tangent=(1,0,0), bitangent=(0,1,0)
    AddFace(vertices,
        -size,  size, -size,
         size,  size, -size,
         size, -size, -size,
        -size, -size, -size,
        0,0,1,
        1,0,0,
        0,1,0
    );

    // Back wall (z=+size), normal=(0,0,-1)
    // U->-x, V->y: tangent=(-1,0,0), bitangent=(0,1,0)
    AddFace(vertices,
         size,  size,  size,
        -size,  size,  size,
        -size, -size,  size,
         size, -size,  size,
        0,0,-1,
        -1,0,0,
        0,1,0
    );

    vertexCount = (int)(vertices.size() / 14); // 14 floats per vertex

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // position (3 floats)
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,14*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    // normal (3 floats)
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,14*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // texcoord (2 floats)
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,14*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    // tangent (3 floats)
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,14*sizeof(float),(void*)(8*sizeof(float)));
    glEnableVertexAttribArray(3);
    // bitangent (3 floats)
    glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,14*sizeof(float),(void*)(11*sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);

    albedoTexture = loadTexture("textures/wood_albedo.jpg");
    normalTexture = loadTexture("textures/wood_normal.jpg");
}

RoomModel::~RoomModel() {
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteTextures(1,&albedoTexture);
    glDeleteTextures(1,&normalTexture);
}

void RoomModel::draw() {
    glBindVertexArray(VAO);

    // We'll bind textures in main.cpp before calling draw(), 
    // so here we just draw the geometry.
    // 6 faces * 4 vertices each, drawn as triangle fan means we can still do:
    for (int i = 0; i < 6; i++) {
        glDrawArrays(GL_TRIANGLE_FAN, i*4, 4);
    }
    glBindVertexArray(0);
}

GLuint RoomModel::loadTexture(const char* path) {
    int w,h,n;
    unsigned char* data = stbi_load(path,&w,&h,&n,3);
    if(!data){
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;
    }
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return tex;
}
