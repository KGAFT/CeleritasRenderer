cmake_minimum_required(VERSION 3.24)
project(CeleritasDX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/cxx/*.h)
file(COPY src/hlsl
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

add_executable(CeleritasDX ${HPP_FILES} ${CPP_FILES})
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

target_link_libraries(CeleritasDX d3d11 d3dcompiler dxgi dxguid glfw glm -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)