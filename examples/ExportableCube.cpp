#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>

void exportSceneToSTL(float* vertices, unsigned int* indices, int indexCount, const char* filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "错误：无法创建导出文件！" << std::endl;
        return;
    }

    // 获取当前绝对路径
    std::filesystem::path absolutePath = std::filesystem::absolute(filename);

    file << "solid ExportedCube" << std::endl;
    for (int i = 0; i < indexCount; i += 3) {
        int i0 = indices[i] * 6;
        int i1 = indices[i + 1] * 6;
        int i2 = indices[i + 2] * 6;

        file << "  facet normal 0 0 0" << std::endl;
        file << "    outer loop" << std::endl;
        file << "      vertex " << vertices[i0] << " " << vertices[i0 + 1] << " " << vertices[i0 + 2] << std::endl;
        file << "      vertex " << vertices[i1] << " " << vertices[i1 + 1] << " " << vertices[i1 + 2] << std::endl;
        file << "      vertex " << vertices[i2] << " " << vertices[i2 + 1] << " " << vertices[i2 + 2] << std::endl;
        file << "    endloop" << std::endl;
        file << "  endfacet" << std::endl;
    }
    file << "endsolid ExportedCube" << std::endl;
    file.close();

    std::cout << "\n========================================" << std::endl;
    std::cout << "导出成功！" << std::endl;
    std::cout << "文件名: " << filename << std::endl;
    std::cout << "绝对路径: " << absolutePath.string() << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void processInput(GLFWwindow* window, float* vertices, unsigned int* indices) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool sPressed = false;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !sPressed) {
        exportSceneToSTL(vertices, indices, 36, "my_cool_cube.stl");
        sPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) sPressed = false;
}

// 着色器保持 3.3 标准
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
uniform mat4 model;
void main() {
    gl_Position = model * vec4(aPos, 1.0);
    ourColor = aColor;
})";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
})";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Press 'S' to Save", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  1.0f,0.0f,0.0f,  0.5f,-0.5f,-0.5f,  0.0f,1.0f,0.0f,
         0.5f, 0.5f,-0.5f,  0.0f,0.0f,1.0f, -0.5f, 0.5f,-0.5f,  1.0f,1.0f,0.0f,
        -0.5f,-0.5f, 0.5f,  1.0f,0.0f,1.0f,  0.5f,-0.5f, 0.5f,  0.0f,1.0f,1.0f,
         0.5f, 0.5f, 0.5f,  1.0f,1.0f,1.0f, -0.5f, 0.5f, 0.5f,  0.5f,0.5f,0.5f
    };
    unsigned int indices[] = {
        0,1,2, 2,3,0, 4,5,6, 6,7,4, 0,4,7, 7,3,0,
        1,5,6, 6,2,1, 3,2,6, 6,7,3, 0,1,5, 5,4,0
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, vertices, indices);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // --- 恢复炫酷旋转矩阵 ---
        float t = (float)glfwGetTime();
        float s = sin(t), c = cos(t);
        // 一个包含 X 和 Y 轴旋转的复合矩阵，并缩小一点 (0.5倍) 方便观察
        float model[16] = {
            c*0.5f, s*s*0.5f, s*c*0.5f, 0,
            0,      c*0.5f,   -s*0.5f,  0,
            -s*0.5f,c*s*0.5f, c*c*0.5f, 0,
            0,      0,        0,        1
        };

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}