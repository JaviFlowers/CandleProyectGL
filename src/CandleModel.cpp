#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "CandleModel.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

// This version creates a candle with:
// - Higher resolution cylindrical sides
// - A slightly indented top surface
// - A simple wick

CandleModel::CandleModel() : VAO(0), VBO(0), EBO(0), indexCount(0) {
    int segments = 64;        // Increase for smoother cylinder
    float radius = 0.2f;
    float height = 1.0f;

    // We'll create the candle in parts:
    // 1. The cylindrical side.
    // 2. The top indentation: 
    //    - Outer ring at top (same radius),
    //    - Inner ring slightly smaller radius and slightly lower y to create a dip.
    // 3. A small wick at the center.

    // Vertices format: position (x,y,z), normal (nx,ny,nz)
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Helper lambda to add a vertex
    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(nx);
        vertices.push_back(ny);
        vertices.push_back(nz);
    };

    // Generate side vertices
    // We'll produce a top ring and bottom ring of vertices for the cylinder's side.
    std::vector<glm::vec3> topRing;
    std::vector<glm::vec3> bottomRing;

    for (int i = 0; i <= segments; i++) {
        float theta = (float)i / segments * 2.0f * M_PI;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        topRing.push_back(glm::vec3(x, height/2.0f, z));
        bottomRing.push_back(glm::vec3(x, -height/2.0f, z));
    }

    // Normals for the side can be approximated as pointing outward:
    // normal = (x,0,z)/radius for each side vertex
    for (int i = 0; i <= segments; i++) {
        float theta = (float)i / segments * 2.0f * M_PI;
        float nx = cosf(theta);
        float nz = sinf(theta);
        // top vertex for side
        addVertex(topRing[i].x, topRing[i].y, topRing[i].z, nx, 0.0f, nz);
        // bottom vertex for side
        addVertex(bottomRing[i].x, bottomRing[i].y, bottomRing[i].z, nx, 0.0f, nz);
    }

    // Create side indices
    for (int i = 0; i < segments; i++) {
        int top1 = i*2;
        int bot1 = i*2 + 1;
        int top2 = (i+1)*2;
        int bot2 = (i+1)*2 + 1;

        indices.push_back(top1);
        indices.push_back(bot1);
        indices.push_back(top2);

        indices.push_back(top2);
        indices.push_back(bot1);
        indices.push_back(bot2);
    }

    // Now the top indentation:
    // We'll create two concentric rings at the top:
    // Outer top ring: same radius as topRing (already have that)
    // Inner top ring: slightly smaller radius and slightly lower y
    float innerRadius = radius * 0.7f;
    float topY = height/2.0f; 
    float innerY = topY - 0.05f; // a small dip

    std::vector<glm::vec3> outerTopRing = topRing; // same as candle top ring
    std::vector<glm::vec3> innerTopRing;
    for (int i = 0; i <= segments; i++) {
        float theta = (float)i / segments * 2.0f * M_PI;
        float x = innerRadius * cosf(theta);
        float z = innerRadius * sinf(theta);
        innerTopRing.push_back(glm::vec3(x, innerY, z));
    }

    // Add vertices for these rings
    int startIndex = (int)(vertices.size()/6); // current vertex count
    for (int i = 0; i <= segments; i++) {
        // Outer ring top normal: mainly up
        addVertex(outerTopRing[i].x, outerTopRing[i].y, outerTopRing[i].z, 0.0f,1.0f,0.0f);
    }
    int outerStart = startIndex;
    int outerCount = segments+1;

    startIndex = (int)(vertices.size()/6);
    for (int i = 0; i <= segments; i++) {
        // Inner ring top normal: also up (simplified)
        addVertex(innerTopRing[i].x, innerTopRing[i].y, innerTopRing[i].z, 0.0f,1.0f,0.0f);
    }
    int innerStart = startIndex;
    int innerCount = segments+1;

    // Triangulate the top indentation
    // Outer ring (o) and inner ring (in)
    for (int i = 0; i < segments; i++) {
        int o1 = outerStart + i;
        int o2 = outerStart + (i+1);
        int in1 = innerStart + i;
        int in2 = innerStart + (i+1);

        // Make quads between outer and inner ring
        indices.push_back(o1);
        indices.push_back(in1);
        indices.push_back(o2);

        indices.push_back(o2);
        indices.push_back(in1);
        indices.push_back(in2);
    }

    // Now cap the inner ring with a single center vertex (for the wick base)
    startIndex = (int)(vertices.size()/6);
    // Center vertex slightly lower than inner ring to simulate melted area
    float centerY = innerY - 0.01f;
    addVertex(0.0f, centerY, 0.0f, 0.0f,1.0f,0.0f);
    int centerIndex = startIndex;

    for (int i = 0; i < segments; i++) {
        int in1 = innerStart + i;
        int in2 = innerStart + (i+1);
        // Fan triangles from center vertex
        indices.push_back(centerIndex);
        indices.push_back(in1);
        indices.push_back(in2);
    }

    // Add a wick: a simple small line made of two vertices
    // We'll treat it as a very thin vertical segment at the center.
    // For simplicity, just two vertices forming a line (though lines are not shaded as triangles).
    // Alternatively, create a tiny quad or very thin cylinder for the wick.
    // Here let's do a small vertical cylinder: 
    // We'll just add a few segments to form a small circle of very small radius:
    float wickRadius = 0.002f;
    float wickHeight = 0.05f;
    float wickBaseY = centerY;
    float wickTopY = wickBaseY + wickHeight;

    startIndex = (int)(vertices.size()/6);
    std::vector<glm::vec3> wickBaseRing;
    std::vector<glm::vec3> wickTopRing;
    for (int i = 0; i <= segments; i++) {
        float theta = (float)i / segments * 2.0f * M_PI;
        float x = wickRadius * cosf(theta);
        float z = wickRadius * sinf(theta);
        wickBaseRing.push_back(glm::vec3(x, wickBaseY, z));
        wickTopRing.push_back(glm::vec3(x, wickTopY, z));
    }

    // Wick normal approximations: just outward
    for (int i = 0; i <= segments; i++) {
        float nx = cosf((float)i / segments * 2.0f * M_PI);
        float nz = sinf((float)i / segments * 2.0f * M_PI);
        addVertex(wickBaseRing[i].x, wickBaseRing[i].y, wickBaseRing[i].z, nx,0.0f,nz);
        addVertex(wickTopRing[i].x, wickTopRing[i].y, wickTopRing[i].z, nx,0.0f,nz);
    }

    int wickBaseStart = (int)(startIndex);
    for (int i = 0; i < segments; i++) {
        int wb1 = wickBaseStart + i*2;
        int wb2 = wb1+1;
        int wb3 = wickBaseStart + (i+1)*2;
        int wb4 = wb3+1;

        indices.push_back(wb1);
        indices.push_back(wb2);
        indices.push_back(wb3);

        indices.push_back(wb3);
        indices.push_back(wb2);
        indices.push_back(wb4);
    }

    indexCount = (int)indices.size();

    // Setup buffers
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

CandleModel::~CandleModel() {
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
}

void CandleModel::draw() {
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
