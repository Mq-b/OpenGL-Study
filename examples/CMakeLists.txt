file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})

    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

    target_include_directories(${EXAMPLE_NAME} PRIVATE "../include")
    target_link_libraries(${EXAMPLE_NAME} PRIVATE
        nlohmann_json::nlohmann_json
        OpenGL::GL
        glfw
        glad::glad
        glm::glm
    )

    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/examples
    )
endforeach()

message(STATUS "Building examples: ${EXAMPLE_SOURCES}")