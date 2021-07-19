#include <math.h>
#include <stdio.h>
#include "Ball.h"

const float defaultVVelocity = 0.002;
const float defaultHVelocity = 0.015;
const float defaultRadius = 0.05;

Ball::Ball()
{
    reset();
}

void Ball::tick()
{
    m_hposition += m_hvelocity;
    m_vposition += m_vvelocity;

    bounceVertically();
}

void Ball::reset()
{
    m_hposition = 0;
    m_vposition = 0;
    m_hvelocity = defaultHVelocity;
    m_vvelocity = defaultVVelocity;
    m_radius = defaultRadius;
}

void Ball::collideWithPaddle(Coords paddleCoords)
{

    float x = m_hvelocity > 0 ? paddleCoords.x1 : paddleCoords.x2;
    float y_top = paddleCoords.y1;
    float y_bot = paddleCoords.y2;

    // Can only collide if the x-position is within the ball's range
    if (x >= m_hposition - m_radius && x <= m_hposition + m_radius)
    {
        // flat bounce, just reverse horizontal velocity
        if (m_vposition <= y_top && m_vposition >=y_bot)
        {
            m_hvelocity = -m_hvelocity;
        }
        // miss: do nothing, but avoid calculations below
        else if (y_top < m_vposition - m_radius || y_bot > m_vposition + m_radius)
        {
        }
        else if (collidesWith(x, y_top))
        {
            bounceOnPoint(x, y_top);
        }
        else if (collidesWith(x, y_bot))
        {
            bounceOnPoint(x, y_bot);
        }
    }
}

bool Ball::collidesWith(float x, float y)
{
    return pow(x - m_hposition, 2) + pow(y - m_vposition, 2) <= pow(m_radius, 2);
}

void Ball::bounceOnPoint(float x, float y)
{
    // angle is from contact point to ball
    float angle = atan2(m_vposition - y, m_hposition - x);

    // maintain the same total velocity along the new angle:
    // todo: add paddle speed component?
    float current_velocity = sqrt(pow(m_vvelocity, 2) + pow(m_hvelocity, 2));
    m_hvelocity = current_velocity * cos(angle);
    m_vvelocity = current_velocity * sin(angle);
}

bool Ball::touches(float x)
{
    return m_hposition - m_radius <= x &&  m_hposition + m_radius >= x;
}

void Ball::bounceVertically()
{
    if (m_vposition + m_radius >= 1 || m_vposition - m_radius <= -1)
    {
        m_vvelocity = -m_vvelocity;
    }
}

