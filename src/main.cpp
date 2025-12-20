/// kompilacja
/// g++ -Iinclude src/main.cpp src/glad.c -Llib -lglfw3dll -o simulator.exe

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib> // dla rand()
#include <ctime>

using namespace std;

// -------------------------------------------
// Structy Vec3 i Particle
// -------------------------------------------
struct Vec3 {
    float x, y, z;
    Vec3(float x_=0, float y_=0, float z_=0) : x(x_), y(y_), z(z_) {}
    Vec3 operator+(const Vec3& other) const { return Vec3(x+other.x, y+other.y, z+other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x*scalar, y*scalar, z*scalar); }
};

struct Particle {
    Vec3 pos;
    Vec3 vel;
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

    srand( time(NULL) );

    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Kuki", NULL, NULL);
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

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, zmiana_rozmiaru_okna);

    // -------------------------------------------
    // Inicjalizacja cząstek
    // -------------------------------------------
    const int NUM_PARTICLES = 50;
    Particle particles[NUM_PARTICLES];

    for(int i=0; i < NUM_PARTICLES; i++){
        particles[i].pos = Vec3((rand()%100 - 50)/50.0f, (rand()%100)/50.0f + 0.5f, (rand()%100 - 50)/50.0f);
        particles[i].vel = Vec3(0.0f, -0.01f, 0.0f);
    }

    // -------------------------------------------
    // VAO + VBO dla punktów
    // -------------------------------------------
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glPointSize(10.0f); // większe punkty, żeby było widać

    // -------------------------------------------
    // Pętla renderująca
    // -------------------------------------------
    while(!glfwWindowShouldClose(window))
    {
        // Aktualizacja pozycji cząstek
        for(int i = 0; i < NUM_PARTICLES; i++){
            particles[i].pos = particles[i].pos + particles[i].vel;

            // odbicie od podłogi
            if(particles[i].pos.y < -1.0f){
                particles[i].pos.y = -1.0f;
                particles[i].vel.y *= -0.6f;
            }

            // efekt grawitacji
            if(particles[i].pos.y > 0.6f){
                particles[i].pos.y = 0.6f;
                particles[i].vel.y *= -1.0f;
            }
        }

        // Wypełnienie VBO aktualnymi pozycjami
        float points[NUM_PARTICLES * 3];
        for(int i = 0; i < NUM_PARTICLES; i++){
            points[i * 3 + 0] = particles[i].pos.x;
            points[i * 3 + 1] = particles[i].pos.y;
            points[i * 3 + 2] = particles[i].pos.z;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, NUM_PARTICLES * 3 * sizeof(float), points);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render
        glClearColor(0.0235f, 0.2039f, 0.4863f, 1.0f);
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