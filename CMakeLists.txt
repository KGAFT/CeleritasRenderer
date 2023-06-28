
cmake_minimum_required(VERSION 3.0)
project(CeleritasEngine)


#Set path to your's vulkan sdk here
set(VulkanSDKPath /home/kgaft/VulkanSDK/1.3.250.0/x86_64)

set(CMAKE_CXX_STANDARD 17)

include_directories("NativeModules/include")

file(GLOB_RECURSE LIN_SHARED_LIBS NativeModules/bin/*.so )
file(GLOB_RECURSE WIN_SHARED_LIBS NativeModules/bin/*.dll )
file(COPY ${LIN_SHARED_LIBS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${WIN_SHARED_LIBS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(modules/glm)
add_subdirectory(modules/VulkanContext)

file(GLOB_RECURSE CPP_FILES src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES src/cxx/*.h)

add_executable(CeleritasEngine ${HPP_FILES} ${CPP_FILES})

target_link_libraries(CeleritasEngine VulkanContext glfw glm ${LIN_SHARED_LIBS} ${WIN_SHARED_LIBS} ${SystemNatives})
