#include "Paddle.h"

const float width = 0.05;
const float length = 0.30;
const float offset = 0.80;

const float max_velocity = 0.4;
const float velocity_increment = 0.005;

Paddle::Paddle(Paddle::Position position) : 
    m_x1 (offset * position - width / 2),
    m_x2 (offset * position + width / 2)
{
    reset();
}

void Paddle::reset()
{
    m_y1 =  length / 2;
    m_y2 = -length / 2;
    m_velocity = 0;
}

void Paddle::tick()
{
    float new_y1 = m_y1 + m_velocity;
    float new_y2 = m_y2 + m_velocity;

    if (new_y1 <= 1 && new_y2 >= -1)
    {
        m_y1 = new_y1;
        m_y2 = new_y2;
    }
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

Paddle::Coords Paddle::getCoords()
{
    return Paddle::Coords {
        m_x1, m_y1, m_x2, m_y2
    };
}

void Paddle::assignIndices(int * index, unsigned int * indexArray, unsigned int vertexOffset)
{
    // Top left triangle:
    indexArray[(*index)++] = vertexOffset;
    indexArray[(*index)++] = vertexOffset + 1;
    indexArray[(*index)++] = vertexOffset + 3;
    // Bottom right triangle:
    indexArray[(*index)++] = vertexOffset + 1;
    indexArray[(*index)++] = vertexOffset + 2;
    indexArray[(*index)++] = vertexOffset + 3;
}

void Paddle::assignVertices(int * index, float * vertexArray)
{
    // Top left
    vertexArray[(*index)++] = m_x1;
    vertexArray[(*index)++] = m_y1;

    // Top right
    vertexArray[(*index)++] = m_x2;
    vertexArray[(*index)++] = m_y1;

    // Bottom right
    vertexArray[(*index)++] = m_x2;
    vertexArray[(*index)++] = m_y2;

    // Bottom left
    vertexArray[(*index)++] = m_x1;
    vertexArray[(*index)++] = m_y2;
}

