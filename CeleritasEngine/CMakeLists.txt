cmake_minimum_required(VERSION 3.24)
project(CeleritasEngine)


#Set path to your's vulkan sdk here
set(VulkanSDKPath C:/VulkanSDK/1.3.246.1/)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        include_directories(LibNativeWin/gcc/include)
        link_directories(LibNativeWin/gcc/lib)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        include_directories(LibNativeWin/msvc/include)
        link_directories(LibNativeWin/msvc/lib)
    endif()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        file(GLOB_RECURSE BINARIES LibNativeWin/gcc/bin/*.dll)
        set(NativesLibs libassimpd.dll)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        file(GLOB_RECURSE BINARIES LibNativeWin/msvc/bin/*.*)
        set(NativeLibs assimp-vc143-mtd.lib)
    endif()
    set(SystemNatives -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)
endif()

if(UNIX)
    include_directories(LibNativeLin/include)
    link_directories(LibNativeLin/lib)
    file(GLOB_RECURSE BINARIES LibNativeLin/bin/*.so)

    set(NativesLibs libassimp.so)
    set(SystemNatives -static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic)
endif()

file(COPY shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../models DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${BINARIES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(modules/glfw)
add_subdirectory(modules/glm)

add_subdirectory(lib/VulkanContext)

file(GLOB_RECURSE CPP_FILES src/cxx/*.cpp)
file(GLOB_RECURSE HPP_FILES src/cxx/*.h)

add_executable(CeleritasEngine ${HPP_FILES} ${CPP_FILES})

target_link_libraries(CeleritasEngine VulkanContext glfw glm ${NativesLibs} ${SystemNatives})
