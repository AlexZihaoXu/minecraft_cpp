cmake_minimum_required(VERSION 3.22)
project(minecraft)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} libraries/ftgl/include libraries/freetype/include libraries/freetype/include libraries/glad/include libraries/glm libraries/imgui libraries/stb)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory(libraries/glfw)
add_subdirectory(libraries/freetype)

set(IMGUI_EXECUTABLES libraries/imgui/imgui.cpp libraries/imgui/imgui_demo.cpp libraries/imgui/imgui_draw.cpp libraries/imgui/imgui_demo.cpp libraries/imgui/imgui_widgets.cpp libraries/imgui/imgui_tables.cpp libraries/imgui/imgui_impl_glfw.cpp libraries/imgui/imgui_impl_opengl3.cpp engine/textures.h engine/fonts.h minecraft/client/client.h minecraft/common/world.h minecraft/common/constants.h minecraft/common/utils.h minecraft/common/common.h minecraft/common/blocks.h)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI_EXECUTABLES} libraries/glad/src/gl.c engine/Window.h engine/engine.h engine/logging.h engine/shaders.h engine/buffers.h)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glfw freetype)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})
