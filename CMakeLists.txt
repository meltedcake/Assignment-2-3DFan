# CMakeLists.txt -- desktop (GLFW + GLEW) and web (Emscripten) entry point
# android/ has its own gradle + cmake build and does not use this file.

cmake_minimum_required(VERSION 3.20)
project(Fan3D CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SCENE_SOURCES
    Scene/main.cpp
    Scene/Renderer.cpp
    Scene/Transform.cpp
    Scene/Fan.cpp
)

# glm: prefer the copy the android build already uses, fetch otherwise
include(FetchContent)
if(EXISTS "${CMAKE_SOURCE_DIR}/lib/glm/glm/glm.hpp")
    set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/glm")
else()
    FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1)
    FetchContent_MakeAvailable(glm)
    set(GLM_INCLUDE_DIR "${glm_SOURCE_DIR}")
endif()

if(EMSCRIPTEN)
    # ---- web: emscripten provides GLFW and WebGL2 (= OpenGL ES 3.0) ----
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(index ${SCENE_SOURCES})
    target_include_directories(index PRIVATE Scene "${GLM_INCLUDE_DIR}")
    target_compile_options(index PRIVATE "-sUSE_SDL=2")
    target_link_options(index PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_GLFW=3"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sFULL_ES3=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "--preload-file" "${CMAKE_SOURCE_DIR}/android/app/src/main/assets@assets")
else()
    # ---- desktop: fetch glfw + glew on first configure ----
    include(cmake/ImportDependencies.cmake)

    add_executable(Fan3D ${SCENE_SOURCES})
    target_include_directories(Fan3D PRIVATE Scene "${GLM_INCLUDE_DIR}")
    target_compile_definitions(Fan3D PRIVATE PLATFORM_WINDOWS GLEW_STATIC)
    target_link_libraries(Fan3D PRIVATE glfw libglew_static opengl32)

    # shaders load from assets/shader/ relative to the working directory,
    # so mirror the android assets folder next to the executable
    add_custom_command(TARGET Fan3D POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/android/app/src/main/assets"
            "$<TARGET_FILE_DIR:Fan3D>/assets")
endif()
