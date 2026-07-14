# cmake/ImportDependencies.cmake -- desktop-only dependencies (GLFW + GLEW).
# Uses a local lib/ copy when present, otherwise fetches on first configure.

include(FetchContent)

# ---- glfw ------------------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/lib/glfw/CMakeLists.txt")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/glfw" glfw EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4)
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

# ---- glew (static) ---------------------------------------------------------
if(EXISTS "${CMAKE_SOURCE_DIR}/lib/glew/CMakeLists.txt")
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/glew" glew EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(glew
        GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
        GIT_TAG        glew-cmake-2.2.0)
    set(ONLY_LIBS              ON  CACHE BOOL "" FORCE)
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glew)
endif()
