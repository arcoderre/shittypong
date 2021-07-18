#include <math.h>
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
}

void Ball::reset()
{
    m_hposition = 0;
    m_vposition = 0;
    m_hvelocity = defaultHVelocity;
    m_vvelocity = defaultVVelocity;
    m_radius = defaultRadius;
}

bool Ball::collidesWith(float x, float y)
{
    return pow(x - m_hposition, 2) + pow(y - m_vposition, 2) <= pow(m_radius, 2);
}

bool Ball::isLeftOf(float x)
{
    return m_hposition - m_radius <= x;
}

bool Ball::isRightOf(float x)
{
    return m_hposition + m_radius >= x;
}

void Ball::bounceVertically()
{
    if (m_vposition + m_radius >= 1 || m_vposition - m_radius <= -1)
    {
        m_vvelocity = -m_vvelocity;
    }
}

