#ifndef PADDLE_H
#define PADDLE_H

class Paddle {
    private:
        const float m_x1, m_x2; // x-positions are constant
        float m_y1, m_y2;       // y-positions are variable
        float m_velocity;       // distance moved per tick in the y-direction

    public:
        enum Position { LEFT = -1, RIGHT = 1 };

        Paddle(Position);

        float getVelocity() { return m_velocity; }

        void tick();

        void reset();

        // controls:
        void up();
        void down();

        struct Coords {
            float x1, y1, x2, y2;
        };

        Paddle::Coords getCoords();

        void assignIndices(int * index, unsigned int * indexArray, unsigned int offset);
};
#endif

