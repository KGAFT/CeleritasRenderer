cmake_minimum_required(VERSION 3.24)
project(CeleritasEngine)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

file(GLOB_RECURSE CPP_FILES src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES src/cxx/*.h)

add_executable(CeleritasEngine ${HPP_FILES} ${CPP_FILES})

target_link_libraries(CeleritasEngine glfw glm)