cmake_minimum_required(VERSION 3.24)
project(CeleritasEngine)

set(VulkanSDKPath C:/VulkanSDK/1.3.239.0)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    include_directories(${VulkanSDKPath}/Include)
    link_directories(${VulkanSDKPath}/Bin ${VulkanSDKPath}/Lib)
    set(VulkanLibList shaderc_shared vulkan-1)
    set(SystemNatives -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)
endif()

if(UNIX)
    include_directories(${VulkanSDKPath}/include)
    link_directories(${VulkanSDKPath}/bin ${VulkanSDKPath}/lib)
    file(GLOB_RECURSE BINARIES LibNativeLin/bin/*.so)

    set(VulkanLibList vulkan libshaderc_combined.a)
    set(SystemNatives -static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
endif()

file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

file(GLOB_RECURSE CPP_FILES src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES src/cxx/*.h)

add_executable(CeleritasEngine ${HPP_FILES} ${CPP_FILES})

target_link_libraries(CeleritasEngine ${VulkanLibList} glfw glm ${SystemNatives})