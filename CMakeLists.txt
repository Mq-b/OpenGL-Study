cmake_minimum_required(VERSION 3.16)

project(OpenGL-Study VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_CXX_STANDARD 20)

option(BUILD_EXAMPLES "Build example programs" ON)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
)

# 如果启用，构建示例程序
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 递归拷贝 res 目录
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res"
)