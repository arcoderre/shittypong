#ifndef BALL_H
#define BALL_H

#include "Paddle.h"

class Ball 
{
    private:
        float m_radius;
        float m_hposition;
        float m_vposition;
        float m_hvelocity;
        float m_vvelocity;

        bool collidesWith(float x_coord, float y_coord);
        void bounceOnPoint(float x_coord, float y_coord);
        void bounceVertically();
        void addPaddleVelocity(float paddleVelocity);

    public:
        Ball();

        void tick();
        void reset();

        bool touches(float x_coord);

        void collideWithPaddle(Paddle paddle);

        struct Coords
        {
            float x, y, r;
        };

        Ball::Coords getCoords();

        void assignIndices(int * index, unsigned int * indexArray, unsigned int vertexOffset, int triangleCount);
};

#endif

