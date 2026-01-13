# OpenGL-Study

OpenGL 学习项目，使用现代 C++ 和 OpenGL 进行图形编程学习。

## 项目依赖

- **OpenGL** - 图形渲染 API
- **GLFW** - 窗口和输入管理
- **GLAD** - OpenGL 函数加载器
- **GLM** - 数学库（矩阵、向量运算）
- **nlohmann/json** - JSON 解析

---

## 🛠 开发环境

- **构建系统**：CMake ≥ 3.16

- **语言标准**：C++20

- **依赖管理**：vcpkg（推荐）

- **测试平台**：

  - **Windows 11 (x86_64)**

    - MSVC（visual studio 17）
    - Clang 21

  - **Linux (x86_64)**

    - 发行版：ubuntu-latest
    - 编译器：GCC14

## 构建

使用 vcpkg 安装依赖：

```shell
vcpkg install glfw3 glad glm nlohmann-json
```

构建项目：

```shell
git clone https://github.com/Mq-b/OpenGL-Study
cmake -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg路径]/scripts/buildsystems/vcpkg.cmake
cmake --build build -j
```
