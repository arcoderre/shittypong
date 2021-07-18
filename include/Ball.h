#ifndef BALL_H
#define BALL_H

class Ball 
{
    private:
        float m_radius;
        float m_hposition;
        float m_vposition;
        float m_hvelocity;
        float m_vvelocity;

    public:
        Ball();

        void tick();
        void reset();

        bool collidesWith(float x_coord, float y_coord);
        bool isLeftOf(float x_coord);
        bool isRightOf(float x_coord);

        void bounceVertically();

        void hReverse() { m_hvelocity = -m_hvelocity; }

        float getRadius() { return m_radius; }
        float getHPosition() { return m_hposition; }
        float getVPosition() { return m_vposition; }
};

#endif

