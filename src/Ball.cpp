#include <math.h>
#include "Ball.h"
#include "Paddle.h"

const float defaultVVelocity = 0.002;
const float defaultHVelocity = 0.015;
const float defaultRadius = 0.05;

const float paddleAccelerationFactor = 0.3;

const float twoPi = 3.14159*2;

Ball::Ball(int triangleCount)
{
    m_triangleCount = triangleCount;
    reset();
}

void Ball::tick()
{
    m_x += m_vx;
    m_y += m_vy;

    bounceVertically();
}

void Ball::reset()
{
    m_x = 0;
    m_y = 0;
    m_vx = defaultHVelocity;
    m_vy = defaultVVelocity;
    m_r = defaultRadius;
}

void Ball::collideWithPaddle(Paddle paddle)
{
    Paddle::Coords paddleCoords = paddle.getCoords();
    float x = m_vx > 0 ? paddleCoords.x1 : paddleCoords.x2;
    float y_top = paddleCoords.y1;
    float y_bot = paddleCoords.y2;

    // miss: do nothing, avoid calculations below
    if (y_top < m_y - m_r || y_bot > m_y + m_r)
        return;

    // Can only collide if the x-position is within the ball's range
    if (x >= m_x - m_r && x <= m_x + m_r)
    {
        // flat bounce, just reverse horizontal velocity
        if (m_y <= y_top && m_y >=y_bot)
        {
            m_vx = -m_vx;
            addPaddleVelocity(paddle.getVelocity());
        }
        else if (collidesWith(x, y_top))
        {
            bounceOnPoint(x, y_top);
            addPaddleVelocity(paddle.getVelocity());
        }
        else if (collidesWith(x, y_bot))
        {
            bounceOnPoint(x, y_bot);
            addPaddleVelocity(paddle.getVelocity());
        }
    }
}

void Ball::addPaddleVelocity(float paddleVelocity)
{
    m_vy += paddleAccelerationFactor * paddleVelocity;
}

bool Ball::collidesWith(float x, float y)
{
    return pow(x - m_x, 2) + pow(y - m_y, 2) <= pow(m_r, 2);
}

void Ball::bounceOnPoint(float x, float y)
{
    // angle is from contact point to ball
    float angle = atan2(m_y- y, m_x - x);

    // maintain the same total velocity along the new angle:
    // todo: add paddle speed component?
    float current_velocity = sqrt(pow(m_vy, 2) + pow(m_vx, 2));
    m_vx = current_velocity * cos(angle);
    m_vy = current_velocity * sin(angle);
}

bool Ball::touches(float x)
{
    return m_x - m_r <= x &&  m_x + m_r >= x;
}

void Ball::bounceVertically()
{
    if (m_y + m_r >= 1 || m_y- m_r <= -1)
    {
        m_vy = -m_vy;
    }
}

Ball::Coords Ball::getCoords()
{
    return Ball::Coords {
        m_x,
        m_y,
        m_r
    };
}

void Ball::assignIndices(int * index, unsigned int * indexArray, unsigned int vertexOffset)
{
    for (int j = 0; j < m_triangleCount; j++)
    {
        indexArray[(*index)++] = vertexOffset;
        indexArray[(*index)++] = vertexOffset + j;
        indexArray[(*index)++] = vertexOffset + j + 1;
    }
    // the final triangle index should be the first vertex on the circle of the ball
    indexArray[(*index) - 1] = vertexOffset + 1;
}

void Ball::assignVertices(int * index, float * vertexArray)
{
    vertexArray[(*index)++] = m_x;
    vertexArray[(*index)++] = m_y;

    for (int j = 0; j < m_triangleCount; j++)
    {
        vertexArray[(*index)++] = m_x + (m_r * cos(j * twoPi / m_triangleCount));
        vertexArray[(*index)++] = m_y + (m_r * sin(j * twoPi / m_triangleCount));
    }
}

