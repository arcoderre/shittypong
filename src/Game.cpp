#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <stdio.h>
#include <math.h>
#include <chrono>
#include <string>
#include <vector>

#include "Game.h"

#include "Paddle.h"
#include "Ball.h"

const int defaultMillisPerTick = 10;
const float twoPi = 3.14159*2;

GLFWwindow* window;
GLuint buffer;

Paddle leftPaddle;
Paddle rightPaddle;
Ball ball;

// Start the game in a "ready" state
bool is_running = false;

std::chrono::steady_clock::time_point last_tick;


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

unsigned int loadShaders()
{
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec2 vertexPosition;\n"
        "\n"
        "void main(){\n"
        "  gl_Position.xy = vertexPosition;\n"
        "  gl_Position.z = 1.0;\n"
        "  gl_Position.w = 1.0;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "out vec3 color;\n"
        "void main(){\n"
        "  color = vec3(0.9, 0.9, 0.9);\n" // off-whitey
        "}";

    GLint result = GL_FALSE;
    int infoLogLength;

    char const * vertexShaderPtr = vertexShader.c_str();
    glShaderSource(vertexShaderId, 1, &vertexShaderPtr, NULL);
    glCompileShader(vertexShaderId);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 )
    {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        printf("%s\n", &vertexShaderErrorMessage[0]);
    }

    char const * fragmentShaderPtr = fragmentShader.c_str();
    glShaderSource(fragmentShaderId, 1, &fragmentShaderPtr, NULL);
    glCompileShader(fragmentShaderId);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 )
    {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        printf("%s\n", &fragmentShaderErrorMessage[0]);
    }

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderId);
    glAttachShader(programID, fragmentShaderId);
    glLinkProgram(programID);

    // Check program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 )
    {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
        printf("%s\n", &programErrorMessage[0]);
    }

    glDetachShader(programID, vertexShaderId);
    glDetachShader(programID, fragmentShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programID;
}

int setupGameWindow()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 640, "Pong, but shitty", NULL, NULL);

    if (!window)
    { 
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        fprintf(stderr, "Error initializing GLEW!");
    
    glfwSetKeyCallback(window, handleInput);
    glfwSwapInterval(1); // Draw every frame

    GLuint programID = loadShaders();
    glUseProgram(programID);

    glGenBuffers(1, &buffer);

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
    
    last_tick = std::chrono::steady_clock::now();
    render();

    while (m_allGood && !glfwWindowShouldClose(window))
    {
        tick();

        /* Poll for and process events */
        glfwPollEvents();
    }

    return 0;
}

void Game::reset()
{
    leftPaddle.reset();
    rightPaddle.reset();
    ball.reset();
    
    is_running = false;

    fprintf(stdout, "Score is: %i to %i\n", m_leftScore, m_rightScore);
}

void Game::tick()
{
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_tick);

    if (is_running && elapsed_time.count() > m_millisPerTick)
    {
        last_tick = current_time;

        rightPaddle.tick();
        leftPaddle.tick();
        ball.tick();
        
        checkCollision();

        render();
    }
}

void Game::checkCollision()
{
    // Check left and right edges for scores:
    if (ball.touches(-1))
    {
        m_rightScore++;
        reset();
    }
    else if (ball.touches(1))
    {
        m_leftScore++;
        reset();
    }

    // Check horizontal collisions with the paddles:
    ball.collideWithPaddle(leftPaddle);
    ball.collideWithPaddle(rightPaddle);
 }

void Game::render()
{
    // 2-element vertices, 3 vertices per triangle; two triangles on each paddle, then 20 triangles for the ball
    const int triangles = 20;
    GLfloat buffer_data[2 * 3 * (2 * 2 + triangles)];

    Paddle::Coords leftPaddleCoords = leftPaddle.getCoords();
    Paddle::Coords rightPaddleCoords = rightPaddle.getCoords();
    Ball::Coords ballCoords = ball.getCoords();

    int i = 0;
    // Left paddle:
    // Top left
    buffer_data[i++] = leftPaddleCoords.x1;
    buffer_data[i++] = leftPaddleCoords.y1;

    // Top right
    buffer_data[i++] = leftPaddleCoords.x2;
    buffer_data[i++] = leftPaddleCoords.y1;

    // Bottom left
    buffer_data[i++] = leftPaddleCoords.x1;
    buffer_data[i++] = leftPaddleCoords.y2;

    // Left paddle, second triangle:
    // Bottom left
    buffer_data[i++] = leftPaddleCoords.x1;
    buffer_data[i++] = leftPaddleCoords.y2;

    // Bottom right:
    buffer_data[i++] = leftPaddleCoords.x2;
    buffer_data[i++] = leftPaddleCoords.y2;

    // Top right
    buffer_data[i++] = leftPaddleCoords.x2;
    buffer_data[i++] = leftPaddleCoords.y1;

    // Right paddle:
    // Top left
    buffer_data[i++] = rightPaddleCoords.x1;
    buffer_data[i++] = rightPaddleCoords.y1;

    // Top right
    buffer_data[i++] = rightPaddleCoords.x2;
    buffer_data[i++] = rightPaddleCoords.y1;

    // Bottom right
    buffer_data[i++] = rightPaddleCoords.x1;
    buffer_data[i++] = rightPaddleCoords.y2;

    // Right paddle, second triangle:
    // Bottom right
    buffer_data[i++] = rightPaddleCoords.x1;
    buffer_data[i++] = rightPaddleCoords.y2;

    // Bottom right:
    buffer_data[i++] = rightPaddleCoords.x2;
    buffer_data[i++] = rightPaddleCoords.y2;
    
    // Top right
    buffer_data[i++] = rightPaddleCoords.x2;
    buffer_data[i++] = rightPaddleCoords.y1;

    for (int j = 0; j < triangles; j++)
    {
        buffer_data[i++] = ballCoords.x;
        buffer_data[i++] = ballCoords.y;

        buffer_data[i++] = ballCoords.x + (ballCoords.r * cos(j * twoPi / triangles));
        buffer_data[i++] = ballCoords.y + (ballCoords.r * sin(j * twoPi / triangles));

        buffer_data[i++] = ballCoords.x + (ballCoords.r * cos((j+1) * twoPi / triangles));
        buffer_data[i++] = ballCoords.y + (ballCoords.r * sin((j+1) * twoPi / triangles));
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3 * (4 + triangles));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}

