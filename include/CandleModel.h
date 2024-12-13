#ifndef CANDLE_MODEL_H
#define CANDLE_MODEL_H

#include <GL/glew.h>

class CandleModel {
public:
    CandleModel();
    ~CandleModel();
    void draw();

private:
    GLuint VAO, VBO, EBO;
    int indexCount;
};

#endif
