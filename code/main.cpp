#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

constexpr float window_width = 800.0;
constexpr float window_height = 600.0;

GLuint vao;
GLuint vbo;
GLuint ebo;
GLuint shaderProgram;
GLFWwindow *window;

// Vertex data (position + color)
GLfloat vertices[] = {
    // Positions        // Colors
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Vertex 1: Red
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Vertex 2: Green
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // Vertex 3: Blue
};

// Indices for the triangle
GLuint indices[] = {0, 1, 2};

float t_angle = 0.0f;
glm::vec3 t_pos = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr float t_angle_speed = 1.0f;
float t_speed_x = 0.3f;
float t_speed_y = 0.5f;

float t_scale = 0.5f;
float t_scale_speed = 0.1f;
constexpr float t_scale_upp_limit = 0.8f;
constexpr float t_scale_low_limit = 0.1f;

// Vertex shader source code
const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    ourColor = aColor;
})";

// Fragment shader source code
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
})";

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Update(float deltaTime)
{
    t_angle += t_angle_speed * deltaTime;
    if (t_angle >= 360.0f)
    {
        t_angle -= 360.0f;
    }

    t_pos.x += t_speed_x * deltaTime;
    t_pos.y += t_speed_y * deltaTime;

    if (t_pos.x >= 1 || t_pos.x <= -1)
    {
        t_speed_x = -t_speed_x;
    }
    if (t_pos.y >= 1 || t_pos.y <= -1)
    {
        t_speed_y = -t_speed_y;
    }

    if (0) {
      t_scale += t_scale_speed * deltaTime;
      if (t_scale >= t_scale_upp_limit || t_scale <= t_scale_low_limit) {
        t_scale_speed = -t_scale_speed;
      }
    }
}

void Draw(float deltaTime)
{
    // Render
    glClear(GL_COLOR_BUFFER_BIT);

    // Use our shader program
    glUseProgram(shaderProgram);

    // Create a transformation matrix
    glm::mat4 transform = glm::mat4(1.0f);                                           // Identity matrix
    transform = glm::translate(transform, t_pos);                                    // Translation
    transform = glm::rotate(transform, (float)t_angle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotation
    transform = glm::scale(transform, glm::vec3(t_scale, t_scale, 1.0f));            // Scale

    // Set the uniform transform variable in the shader
    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    // Draw the triangle
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Swap buffers
    glfwSwapBuffers(window);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "OpenGL Triangle Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shaders as they're no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create Vertex Array Object (VAO)
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create Vertex Buffer Object (VBO)

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(indices), indices, GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GLfloat) * 6); // Position attribute
    glVertexArrayVertexBuffer(vao, 1, vbo, 0, sizeof(GLfloat) * 6); // Color attribute

    // Position attribute
    glEnableVertexArrayAttrib(vao, 0);                           // Enable position attribute
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0); // 3 components for position
    glVertexArrayAttribBinding(vao, 0, 0);                       // Bind to binding index 0

    // Color attribute
    glEnableVertexArrayAttrib(vao, 1);                                             // Enable color attribute
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3); // 3 components for color, offset by 3 floats
    glVertexArrayAttribBinding(vao, 1, 0);                                         // Bind to binding index 0

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(vao, ebo);

    double currentFrame = glfwGetTime();
    double lastFrame = currentFrame;
    double deltaTime;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        Update(deltaTime);
        Draw(deltaTime);

        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
