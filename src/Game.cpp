#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <stdio.h>
#include <math.h>
#include <chrono>
#include <string>
#include <vector>
#include <signal.h>

#include "Game.h"

#include "Paddle.h"
#include "Ball.h"

#define ASSERT(x) if (!(x)) raise(SIGTRAP);
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    bool is_ok = true;
    while (GLenum error = glGetError())
    {
        fprintf(stderr, "[OpenGL Error] (%i): %s :: %s : %i", error, function, file, line);
        is_ok = false;
    }
    return is_ok;
}

const int defaultMillisPerTick = 10;


GLFWwindow* window;
GLuint vbo, ibo; // Vertex and index buffers

// 2-element vertices, 4 per paddle, 1 + $triangles for the ball (centre + ring)
const int triangles = 20;
const int vertex_count = 2 * (4 * 2 + 1 + triangles);
GLfloat vertices[vertex_count];

// 3 indices per triangle; two triangles on each paddle, then 20 triangles for the ball
// Paddle indices start top left and go clockwise
// Ball indices start with the centre, then positive x and go counterclockwise (i.e. math standard)
const int index_count = 3 * (2 * 2 + triangles);
GLuint indices[index_count];

Paddle leftPaddle(Paddle::Position::LEFT);
Paddle rightPaddle(Paddle::Position::RIGHT);
Ball ball(triangles);

// Start the game in a "ready" state
bool is_running = false;

std::chrono::steady_clock::time_point last_tick;


void initIndices()
{
    int i = 0;
    leftPaddle.assignIndices(&i, indices, 0);
    rightPaddle.assignIndices(&i, indices, 4);
    ball.assignIndices(&i, indices, 8);
}

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
    GLCall(glUseProgram(programID));

    GLCall(glGenBuffers(1, &vbo));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), NULL, GL_DYNAMIC_DRAW));

    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    return 1;
}

Game::Game()
{
    m_millisPerTick = defaultMillisPerTick;

    m_leftScore = 0;
    m_rightScore = 0;
    m_allGood = true;

    is_running = false;

    initIndices();
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
    int i = 0;
    leftPaddle.assignVertices(&i, vertices);
    rightPaddle.assignVertices(&i, vertices);
    ball.assignVertices(&i, vertices);

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count * sizeof(float), &vertices));

    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    GLCall(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
}

