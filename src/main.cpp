#include <iostream>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window){
    // 按下 ESC 键关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 初始化glfw
void initGLFW(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwInit();
    // 1. 设置 OpenGL 版本为 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 2. 使用核心模式放弃旧版特性
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
//创建窗口并设置上下文初始化GLAD并返回
GLFWwindow* createWindow(std::size_t width, std::size_t height, const char* title){
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    // 4. 设置当前上下文 opengl 的所有绘制操作都将在这个窗口中进行
    glfwMakeContextCurrent(window);
    // 5. 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, width, height);
    return window;
}

int main(){
    initGLFW();
    auto window = createWindow(800, 600, "OpenGL Triangle");
    
    //绘制三角形 由于 viewport 设置的不是一个正方形，三角形会被拉伸
    std::array vertices {
         0.0f,  0.5f, 0.0f,  // 顶点1
        -0.5f, -0.5f, 0.0f,  // 顶点2
         0.5f, -0.5f, 0.0f   // 顶点3
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 顶点数据上传到显存
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), std::data(vertices), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // 6. 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // A. 清屏 设置背景颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // B. 绘制
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        processInput(window);

        // C. 交换缓冲区 (双缓冲)
        glfwSwapBuffers(window);
        
        // D. 检查事件
        glfwPollEvents();
    }

    // 7. 释放所有资源并退出
    glfwTerminate();
}