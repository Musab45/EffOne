cmake_minimum_required(VERSION 3.20)
project(f1sim C CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8)
FetchContent_MakeAvailable(glm)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

FetchContent_Declare(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui_lib PUBLIC glfw)

# GLAD — generated with: python -m glad --profile core --api gl=4.3 --generator c --out-path vendor/glad
# Outputs to vendor/glad/src/glad.c and vendor/glad/include/
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(f1sim ${SRC})
target_include_directories(f1sim PRIVATE src vendor/glad/include vendor/miniaudio)
target_link_libraries(f1sim PRIVATE glfw glm glad imgui_lib)
if(WIN32)
    target_link_libraries(f1sim PRIVATE winmm)
endif()

add_executable(track_builder tools/track_builder/main.cpp)
target_link_libraries(track_builder PRIVATE glm)
target_include_directories(track_builder PRIVATE vendor/glad/include)
