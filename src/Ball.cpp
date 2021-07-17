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

