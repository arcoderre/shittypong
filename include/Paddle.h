#ifndef PADDLE_H
#define PADDLE_H

#include "Coords.h"

class Paddle {
    private:
        float m_length;
        float m_width;
        float m_hposition;        // will differ between left and right paddle
        float m_vposition;
        float m_velocity; 

    public:
        Paddle();
        
        void setLeftPaddle();
        void setRightPaddle();

        void tick();

        void reset();

        // controls:
        void up();
        void down();

        struct Coords {
            float x1, y1, x2, y2;
        };

        Paddle::Coords getCoords();

};

#endif

