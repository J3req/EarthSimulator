#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// -------------------------------------------
// Structy Vec2 i Particle
// -------------------------------------------
struct Vec2 {
    float x, y;
    Vec2(float x_=0, float y_=0) : x(x_), y(y_) {}
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float divide) const { return Vec2(x / divide, y / divide); }
};

struct Color {
    float r, g, b;
    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct Particle {
    Vec2 pos;
    Vec2 vel;
    Color col;
};

// -------------------------------------------
// Callback do zmiany rozmiaru okna
// -------------------------------------------
void zmiana_rozmiaru_okna(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// -------------------------------------------
// Program główny
// -------------------------------------------
int main()
{
    srand(time(nullptr));

    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Particles 2D", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, zmiana_rozmiaru_okna);

    // -------------------------------------------
    // Inicjalizacja cząstek
    // -------------------------------------------
    const int NUM_PARTICLES = 50;
    Particle particles[NUM_PARTICLES];

    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].pos = Vec2((rand() % 100 - 50) / 50.0f, (rand() % 100 - 50) / 50.0f);
        particles[i].vel = Vec2(0.0f, -0.002f);
        particles[i].col = Color(
            static_cast<float>(rand() % 256) / 255.0f,
            static_cast<float>(rand() % 256) / 255.0f,
            static_cast<float>(rand() % 256) / 255.0f
        );
    }

    // -------------------------------------------
    // VAO + VBO dla punktów i kolorów
    // -------------------------------------------
    unsigned int VAO, VBO, colorVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    // VBO dla pozycji
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // VBO dla kolorów
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPointSize(10.0f);

    // -------------------------------------------
    // Inicjalizacja shadera
    // -------------------------------------------

    const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec3 aColor;
    out vec3 fragColor;
    void main()
    {
        gl_Position = vec4(aPos, 0.0, 1.0);
        fragColor = aColor;
    }
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 fragColor;
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(fragColor, 1.0);
    }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // po skompilowaniu shadery można usunąć
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);


    // Inicjalizacja zmiennych wektorowych
    Vec2 gravity(0.0f, -0.005f);
    Vec2 ruch(0.01f, 0.0f);
    const float EPSILON = 0.00001f;

    // -------------------------------------------
    // Pętla renderująca
    // -------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Aktualizacja pozycji cząstek
        for (int i = 0; i < NUM_PARTICLES; i++) {
            float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;

            particles[i].pos = particles[i].pos + particles[i].vel;

            if (particles[i].vel.x == 0.0f) {
                particles[i].vel.x = (sin(angle)) / 1000.0f;
            }

            // odbicia od ścian
            if (particles[i].pos.y < -1.0f) {
                particles[i].pos.y = -1.0f;
                particles[i].vel.y *= -1.0f;
            }

            if (particles[i].pos.y > 1.0f) {
                particles[i].pos.y = 1.0f;
                particles[i].vel.y *= -1.0f;
            }

            if (particles[i].pos.x < -1.0f) {
                particles[i].pos.x = -1.0f;
                particles[i].vel.x *= -1.0f;
            }

            if (particles[i].pos.x > 1.0f) {
                particles[i].pos.x = 1.0f;
                particles[i].vel.x *= -1.0f;
            }
        }

        // Wypełnienie VBO
        float points[NUM_PARTICLES * 2];
        for (int i = 0; i < NUM_PARTICLES; i++) {
            points[i * 2 + 0] = particles[i].pos.x;
            points[i * 2 + 1] = particles[i].pos.y;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_PARTICLES * 2 * sizeof(float), points);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // wypełnienie VBO kolorami
        float colors[NUM_PARTICLES * 3];
        for (int i = 0; i < NUM_PARTICLES; i++) {
            colors[i*3+0] = particles[i].col.r;
            colors[i*3+1] = particles[i].col.g;
            colors[i*3+2] = particles[i].col.b;
        }
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_PARTICLES * 3 * sizeof(float), colors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
