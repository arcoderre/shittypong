#include "Paddle.h"

const float width = 0.05; // no idea at this point
const float length = 0.30; //yep
const float offset = 0.80;

const float max_velocity = 0.4; // ?
const float velocity_increment = 0.005;  // ?

Paddle::Paddle()
{
    m_width = width;
    m_length = length;
    reset();
}

void Paddle::setLeftPaddle()
{
    m_hposition = -offset;
}

void Paddle::setRightPaddle()
{
    m_hposition = offset;
}

void Paddle::reset()
{
    m_vposition = 0;
    m_velocity = 0;
}

void Paddle::tick()
{
    float new_position = m_vposition + m_velocity;
    if (new_position + m_length / 2 < 1 && new_position - m_length / 2 > -1)
        m_vposition = new_position;
    else
        m_velocity = 0;
}

void Paddle::up()
{
    if (m_velocity < max_velocity)
        m_velocity += velocity_increment;
}

void Paddle::down()
{
    if (m_velocity > -max_velocity)
        m_velocity -= velocity_increment;
}

Coords Paddle::getCoords()
{
    return Coords { 
        (m_hposition - m_width / 2), // top left
        (m_vposition + m_length / 2),
        (m_hposition + m_width / 2),
        (m_vposition - m_length / 2) // bottom right
    };
}
