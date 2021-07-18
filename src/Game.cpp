#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <stdio.h>
#include <math.h>
#include <chrono>

#include "Game.h"

#include "Paddle.h"
#include "Ball.h"

const int defaultMillisPerTick = 10;
const float twicePi = 3.14159*2;

GLFWwindow* window;

Paddle leftPaddle;
Paddle rightPaddle;
Ball ball;

bool is_running = false;

using std::chrono::steady_clock;

steady_clock::time_point last_tick;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "error %i: %s\n", error, description);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void handleInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key) {

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;

            case GLFW_KEY_SPACE:
                is_running = true;
                break;

            case GLFW_KEY_UP:
                rightPaddle.up();
                break;

            case GLFW_KEY_DOWN:
                rightPaddle.down();
                break;

            case GLFW_KEY_A:
                leftPaddle.up();
                break;

            case GLFW_KEY_Z:
                leftPaddle.down();
                break;
        }
    }
}
#pragma GCC diagnostic pop

int setupGameWindow()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Pong, but shitty", NULL, NULL);

    if (!window)
    { 
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, handleInput);
    glfwSwapInterval(1); // Draw every frame

    return 1;
}

Game::Game()
{
    leftPaddle.setLeftPaddle();

    rightPaddle.setRightPaddle();

    m_millisPerTick = defaultMillisPerTick;

    m_leftScore = 0;
    m_rightScore = 0;
    m_allGood = true;

    is_running = false;
}

Game::~Game()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int Game::run()
{
    if (!setupGameWindow())
        return -1;
    
    last_tick = steady_clock::now();

    while (m_allGood && !glfwWindowShouldClose(window))
    {
        tick();
        render();
    }
    return 0;
}

void Game::reset()
{
    leftPaddle.reset();
    rightPaddle.reset();
    ball.reset();
    
    is_running = false;
}

void Game::tick()
{
    auto current_time = steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_tick);

    if (is_running && elapsed_time.count() > m_millisPerTick)
    {
        last_tick = current_time;

        rightPaddle.tick();
        leftPaddle.tick();
        ball.tick();
        
        checkCollision();
    }
}

void Game::checkCollision()
{
    // Check top and bottom edges for bounces:
    if (ball.getVPosition() + ball.getRadius() >= 1 ||
        ball.getVPosition() - ball.getRadius() <= -1)
    {
        ball.vReverse();
    }

    // Check left and right edges for scores:
    if (ball.getHPosition() - ball.getRadius() <= -1)
    {
        m_rightScore++;
        reset();
    }
    else if (ball.getHPosition() + ball.getRadius() >= 1)
    {
        m_leftScore++;
        reset();
    }

    // Check horizontal collisions with the paddles:
    Coords leftPaddleCoords = leftPaddle.getCoords();
    Coords rightPaddleCoords = rightPaddle.getCoords();

    // Has the ball reached the rightmost point of the left paddle?
    if ((ball.getHPosition() - ball.getRadius()) <= leftPaddleCoords.x2)
    {
        // three collision modes possible: top corner, flat mid, bot corner.
        
        // If the ball hits on the flat part of the paddle, reverse the ball's direction
        if (ball.getVPosition() < leftPaddleCoords.y1 &&
            ball.getVPosition() > leftPaddleCoords.y2)
        {
            ball.hReverse();
        }

        // If we're on the top or bottom corner, the check gets more fun:
        else if (
            (pow(leftPaddleCoords.x2 - ball.getHPosition(), 2) +
            pow(leftPaddleCoords.y1 - ball.getVPosition(), 2) <=
            pow(ball.getRadius(), 2)) 
            ||            
            (pow(leftPaddleCoords.x2 - ball.getHPosition(), 2) +
            pow(leftPaddleCoords.y2 - ball.getVPosition(), 2) <=
            pow(ball.getRadius(), 2)))
        {
            // Todo: find the angle and rebound accordingly
            // for now just reverse it:
            ball.hReverse();
        }
    }
    else if ((ball.getHPosition() + ball.getRadius()) >= rightPaddleCoords.x1)
    {
        // three collision modes possible: top corner, flat mid, bot corner.
        
        // If the ball hits on the flat part of the paddle, reverse the ball's direction
        if (ball.getVPosition() < rightPaddleCoords.y1 &&
            ball.getVPosition() > rightPaddleCoords.y2)
        {
            ball.hReverse();
        }

        // If we're on the top or bottom corner, the check gets more fun:
        // top:
        else if (
            (pow(rightPaddleCoords.x1 - ball.getHPosition(), 2) +
            pow(rightPaddleCoords.y1 - ball.getVPosition(), 2) <=
            pow(ball.getRadius(), 2)) 
            ||            
            (pow(rightPaddleCoords.x1 - ball.getHPosition(), 2) +
            pow(rightPaddleCoords.y2 - ball.getVPosition(), 2) <=
            pow(ball.getRadius(), 2)))
        {
            // Todo: find the angle and rebound accordingly
            // for now just reverse it:
            ball.hReverse();
        }
    }
}

void Game::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    Coords leftPaddleCoords = leftPaddle.getCoords();

    glBegin(GL_POLYGON);
    glVertex2f(leftPaddleCoords.x1, leftPaddleCoords.y1);
    glVertex2f(leftPaddleCoords.x2, leftPaddleCoords.y1);
    glVertex2f(leftPaddleCoords.x2, leftPaddleCoords.y2);
    glVertex2f(leftPaddleCoords.x1, leftPaddleCoords.y2);
    glEnd();

    Coords rightPaddleCoords = rightPaddle.getCoords();
    
    glBegin(GL_POLYGON);
    glVertex2f(rightPaddleCoords.x1, rightPaddleCoords.y1);
    glVertex2f(rightPaddleCoords.x2, rightPaddleCoords.y1);
    glVertex2f(rightPaddleCoords.x2, rightPaddleCoords.y2);
    glVertex2f(rightPaddleCoords.x1, rightPaddleCoords.y2);
    glEnd();


    int triangles = 20;
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ball.getHPosition(), ball.getVPosition()); // center of circle
    for(int i = 0; i <= triangles; i++) { 
        glVertex2f(
            ball.getHPosition() + (ball.getRadius() * cos(i *  twicePi / triangles)), 
            ball.getVPosition() + (ball.getRadius() * sin(i * twicePi / triangles))
        );
    }
    glEnd();


    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}

