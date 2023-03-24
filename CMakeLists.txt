cmake_minimum_required(VERSION 3.24)
project(CeleritasEngine)

set(VulkanSDKPath C:/VulkanSDK/1.3.239.0)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        include_directories(${VulkanSDKPath}/Include LibNativeWin/gcc/include)
        link_directories(${VulkanSDKPath}/Bin ${VulkanSDKPath}/Lib LibNativeWin/gcc/lib)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        include_directories(${VulkanSDKPath}/Include LibNativeWin/msvc/include)
        link_directories(${VulkanSDKPath}/Bin ${VulkanSDKPath}/Lib LibNativeWin/msvc/lib)
    endif()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        file(GLOB_RECURSE BINARIES LibNativeWin/gcc/bin/*.dll)
        set(NativesLibs libassimpd.dll)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        file(GLOB_RECURSE BINARIES LibNativeWin/msvc/bin/*.*)
        set(NativeLibs assimp-vc143-mtd)
    endif()
    set(VulkanLibList shaderc_shared vulkan-1)
    set(SystemNatives -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)
endif()

if(UNIX)
    include_directories(${VulkanSDKPath}/include LibNativeLin/include)
    link_directories(${VulkanSDKPath}/bin ${VulkanSDKPath}/lib LibNativeLin/lib)
    file(GLOB_RECURSE BINARIES LibNativeLin/bin/*.so)

    set(VulkanLibList vulkan libshaderc_combined.a)
    set(NativesLibs libassimp.so)
    set(SystemNatives -static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
endif()

file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

file(GLOB_RECURSE CPP_FILES src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES src/cxx/*.h)

add_executable(CeleritasEngine ${HPP_FILES} ${CPP_FILES})

target_link_libraries(CeleritasEngine ${VulkanLibList} glfw glm ${NativesLibs} ${SystemNatives})