#ifndef BALL_H
#define BALL_H

#include "Paddle.h"

class Ball 
{
    private:
        float m_r;
        float m_x;
        float m_y;
        float m_vx;
        float m_vy;

        int m_triangleCount;

        bool collidesWith(float x_coord, float y_coord);
        void bounceOnPoint(float x_coord, float y_coord);
        void bounceVertically();
        void addPaddleVelocity(float paddleVelocity);

    public:
        Ball(int triangleCount);

        void tick();
        void reset();

        bool touches(float x_coord);

        void collideWithPaddle(Paddle paddle);

        void assignIndices(int * index, unsigned int * indexArray, unsigned int vertexOffset);

        void assignVertices(int * index, float * vertexArray);
};

#endif

