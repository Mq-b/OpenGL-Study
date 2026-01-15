#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glutils {

inline void initGLFW(){
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

inline GLFWwindow* createWindow(std::size_t width, std::size_t height, const char* title){
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

// 类型可以用于 std::string 初始化
template<typename T>
concept as_string = std::convertible_to<T, std::string>;

/**
 * @brief OpenGL 着色器类型枚举
 * * 封装了 OpenGL 原生着色器类型的宏定义，方便在着色器类中进行类型识别和参数传递。
 */
enum ShaderType {
    /// @brief 顶点着色器 (Vertex Shader)
    Vertex          = GL_VERTEX_SHADER,
    /// @brief 片段/像素着色器 (Fragment Shader)
    Fragment        = GL_FRAGMENT_SHADER,
    /// @brief 几何着色器 (Geometry Shader)
    Geometry        = GL_GEOMETRY_SHADER,
    /// @brief 曲面细分控制着色器 (Tessellation Control Shader)
    TessControl     = GL_TESS_CONTROL_SHADER,
    /// @brief 曲面细分评估着色器 (Tessellation Evaluation Shader)
    TessEvaluation  = GL_TESS_EVALUATION_SHADER,
    /// @brief 计算着色器 (Compute Shader)
    Compute         = GL_COMPUTE_SHADER
};

// 基础着色器结构
struct ShaderSource {
    std::string source;
    
    ShaderSource(std::string src) : source(std::move(src)) {}
    virtual ~ShaderSource() = default;
    
    virtual ShaderType getType() const = 0;
};

struct VertexShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::Vertex; }
};

struct FragmentShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::Fragment; }
};

struct GeometryShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::Geometry; }
};

struct ComputeShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::Compute; }
};

struct TessControlShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::TessControl; }
};

struct TessEvaluationShaderSource : ShaderSource {
    using ShaderSource::ShaderSource;
    ShaderType getType() const override { return ShaderType::TessEvaluation; }
};

template<typename T>
concept is_shader_source = std::is_base_of_v<ShaderSource, T>;

/**
 * @brief 编译并链接多个着色器程序
 * 
 * @param ShaderObjects 自定义的着色器对象
 * @return unsigned int 返回链接好的着色器程序 ID
 */
unsigned int compileShader(const is_shader_source auto&... ShaderObjects) {
    auto compileIndividualShader = [](const char* source,int type) {
        static char infoLog[512]{};
        static int success{};

        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, std::size(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
            std::memset(infoLog, 0, std::size(infoLog));
        }
        return shader;
    };
    // 解包参数并编译每个着色器
    std::array array = { compileIndividualShader(ShaderObjects.source.c_str(), ShaderObjects.getType())... };
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

class Shader{
public:
    // 加载着色器文件返回 std::string
    static inline std::string loadShaderFromFile(const as_string auto& shaderPath) {
        std::ifstream shaderFile(shaderPath);
        if (!shaderFile.is_open()) {
            std::cerr << "Failed to open shader file: " << shaderPath << std::endl;
            exit(-1);
        }
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        return buffer.str();
    }

    // 单独编译链接一个着色器程序 编译成功与失败检查

    static inline unsigned int compileShader(const as_char_ptr auto& source, int type) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        // 检查编译状态
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[64]{};
            glGetShaderInfoLog(shader, 64, NULL, infoLog);
            std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
            exit(-1);
        }

        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, shader);
        glLinkProgram(shaderProgram);
        glDeleteShader(shader);

        return shaderProgram;
    }

    // 单参版本构造函数，一个着色器文件路径以及着色器类型
    Shader(const as_string auto& shaderPath, int shaderType) {
        std::string shaderSource = loadShaderFromFile(shaderPath);
        programID = compileShader(shaderSource.c_str(), shaderType);
    }
    Shader() {
        
    }
    ~Shader() {
        glDeleteProgram(programID);
    }
    void use() const {
        glUseProgram(programID);
    }
    unsigned int getID() const {
        return programID;
    }

private:
    
    unsigned int programID;
};

}