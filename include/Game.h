#ifndef GAME_H
#define GAME_H
#include <GLFW/glfw3.h>
#include "Paddle.h"
#include "Ball.h"

class Game
{
    private:
        long m_millisPerTick;

        int m_leftScore;
        int m_rightScore;

        bool m_allGood;

        void checkCollision();

    public:
        Game();
        ~Game();

        int run();

        void tick();

        void render();

        void reset();
};

#endif

