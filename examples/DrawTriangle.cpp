#include <iostream>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void initGLFW(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWindow(std::size_t width, std::size_t height, const char* title){
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, width, height);
    return window;
}

template<typename T>
concept as_char_ptr = std::convertible_to<T, const char*>;

/**
 * @brief 编译并链接多个着色器程序
 * 
 * @tparam types OpenGL 的着色器类型，如 GL_VERTEX_SHADER, GL_FRAGMENT_SHADER 等
 * @param ShaderSources OpenGL 着色器源码字符串
 * @return unsigned int 返回链接好的着色器程序 ID
 */
template<std::size_t... types>
unsigned int compileShader(const as_char_ptr auto&... ShaderSources) {
    auto compileIndividualShader = [](const char* source,int type) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        return shader;
    };
    // 解包参数并编译每个着色器
    std::array array = { compileIndividualShader(ShaderSources,types)... };
    // 链接
    unsigned int shaderProgram  = glCreateProgram();
    for(const auto& shader : array)
        glAttachShader(shaderProgram, shader);

    glLinkProgram(shaderProgram);
    // 释放
    for(const auto& shader : array)
        glDeleteShader(shader);

    return shaderProgram;
}

// 顶点着色器源码
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
       gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

// 片段着色器源码：决定最终像素的颜色
const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
       FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); // 橘黄色 (RGBA)
    }
)";

int main(){
    // 初始化 glfw 使用 OpenGL3.3 核心模式
    initGLFW();
    // 创建窗口，设置OpenGL上下文，并加载 GLAD 激活 OpenGL 的函数指针。
    auto window = createWindow(800, 600, "OpenGL Triangle");

    // 编译链接着色器程序
    auto shaderProgram = compileShader<GL_VERTEX_SHADER, GL_FRAGMENT_SHADER>(vertexShaderSource, fragmentShaderSource);

    // 三角形顶点数据
    std::array vertices {
        0.0f,  0.5f, 0.0f,  // 顶点1 (顶部)
        -0.5f, -0.5f, 0.0f,  // 顶点2 (左下)
        0.5f, -0.5f, 0.0f   // 顶点3 (右下)
    };
    // 创建顶点缓冲对象和顶点数组对象并绑定
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 顶点数据上传到显存，GL_STATIC_DRAW 选项表示数据不会频繁改变
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), std::data(vertices), GL_STATIC_DRAW);
    // 告诉 OpenGL 如何解析刚才上传到显存的数据：从 0 号位置开始，每三个 float 为一个顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    // 启用顶点属性；0 是索引，表示对应 glVertexAttribPointer 中的第一个参数，三角形的坐标数据关联到了 0 号槽位。所以必须开启 0 号槽位显卡才能读取这里的坐标
    glEnableVertexAttribArray(0);

    // 循环渲染
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // 设置清屏颜色，四个参数代表 RGBA
        glClear(GL_COLOR_BUFFER_BIT);         // 执行清屏操作

        // 激活着色器程序
        glUseProgram(shaderProgram);

        // 重复绑定 顶点数组对象（只绘制这一个图形，可有可无）
        glBindVertexArray(VAO);
        // 使用线框模式绘制 默认是填充模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // 绘图指令：通知 GPU 按照“三角形”规则，处理当前绑定的 VAO 里的前 3 个顶点数据，在后台绘制三角形
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // GLFW 检查是否按下了 ESC 键
        processInput(window);
        // 交换缓冲：把画好的后台图像推到前台显示（双缓冲机制）
        glfwSwapBuffers(window);
        // GLFW 处理窗口事件，比如键盘鼠标输入等
        glfwPollEvents();
    }
    // 清理着色器程序资源
    glDeleteProgram(shaderProgram);
    // 释放 GLFW 资源
    glfwTerminate();
}