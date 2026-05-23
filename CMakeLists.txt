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

# GLAD — generated with: python -m glad --profile core --api gl=4.3 --generator c --out-path vendor/glad
# Outputs to vendor/glad/src/glad.c and vendor/glad/include/
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_executable(f1sim src/main.cpp)
target_include_directories(f1sim PRIVATE src vendor/glad/include)
target_link_libraries(f1sim PRIVATE glfw glm glad)
