cmake_minimum_required(VERSION 4.3.2)
project(OpenGL VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable building GLFW documentation and examples to speed up compile times
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(Resources/glfw-3.4)

# Look up the system's native OpenGL drivers (Windows opengl32.lib or Linux GL drivers)
find_package(OpenGL REQUIRED)

set(SRC_FILES
    Source.c++
    Camera.c++
    ShaderProgram.c++
    glad.c++)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"                            # For your own header files
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Includes")        # For glad, GLFW, glm, KHR folders

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    OpenGL::GL)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

configure_file(VertexShader.vs ${CMAKE_CURRENT_BINARY_DIR}/VertexShader.vs COPYONLY)
configure_file(FragmentShader.fs ${CMAKE_CURRENT_BINARY_DIR}/FragmentShader.fs COPYONLY)
configure_file(LightVertexShader.vs ${CMAKE_CURRENT_BINARY_DIR}/LightVertexShader.vs COPYONLY)
configure_file(LightFragmentShader.fs ${CMAKE_CURRENT_BINARY_DIR}/LightFragmentShader.fs COPYONLY)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Textures"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")