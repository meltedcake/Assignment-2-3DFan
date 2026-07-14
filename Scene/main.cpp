#define LOG_TAG "Main"

/**
 * main.cpp
 *
 * Desktop (GLFW + GLEW) and WebGL (Emscripten + GLFW) entry point.
 *
 * Maps the left mouse button to the same three touch calls Android sends:
 *   press            -> Renderer::TouchEventDown
 *   drag while held  -> Renderer::TouchEventMove
 *   release          -> Renderer::TouchEventRelease
 * So click = tap and click-drag = swipe, identical gesture logic on all
 * three platforms. ESC quits (desktop).
 */

#include "Platform.h"
#include "Renderer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef PLATFORM_EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

static GLFWwindow* g_window    = nullptr;
static bool        g_mouseDown = false;

// ---------------------------------------------------------------------------
// Mouse -> touch mapping
// ---------------------------------------------------------------------------
static void onMouseButton(GLFWwindow* win, int button, int action, int /*mods*/)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    double x = 0.0, y = 0.0;
    glfwGetCursorPos(win, &x, &y);

    if (action == GLFW_PRESS) {
        g_mouseDown = true;
        Renderer::Instance().TouchEventDown(static_cast<float>(x),
                                            static_cast<float>(y));
    } else if (action == GLFW_RELEASE) {
        g_mouseDown = false;
        Renderer::Instance().TouchEventRelease(static_cast<float>(x),
                                               static_cast<float>(y));
    }
}

static void onCursorPos(GLFWwindow* /*win*/, double x, double y)
{
    // only a held button counts as a drag -- hovering is not a touch
    if (g_mouseDown)
        Renderer::Instance().TouchEventMove(static_cast<float>(x),
                                            static_cast<float>(y));
}

static void onKey(GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}

static void onFramebufferSize(GLFWwindow* /*win*/, int w, int h)
{
    Renderer::Instance().resize(w, h);
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------
static void mainLoop()
{
    Renderer::Instance().render();
    glfwSwapBuffers(g_window);
    glfwPollEvents();
}

int main()
{
    if (!glfwInit()) {
        LOGE("glfwInit failed");
        return -1;
    }

#ifdef PLATFORM_EMSCRIPTEN
    // WebGL 2.0 == OpenGL ES 3.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    // desktop core 4.3 exposes GL_ARB_ES3_compatibility, so the
    // "#version 300 es" shaders compile unchanged
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    g_window = glfwCreateWindow(540, 960, "Assignment 2 -- 3D Fan", nullptr, nullptr);
    if (!g_window) {
        LOGE("glfwCreateWindow failed");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);

#ifdef PLATFORM_WINDOWS
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        LOGE("glewInit failed");
        glfwTerminate();
        return -1;
    }
#endif

    glfwSetMouseButtonCallback(g_window, onMouseButton);
    glfwSetCursorPosCallback(g_window, onCursorPos);
    glfwSetKeyCallback(g_window, onKey);
    glfwSetFramebufferSizeCallback(g_window, onFramebufferSize);

    Renderer::Instance().initializeRenderer();

    // push the initial size so the projection matrix exists before frame 1
    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(g_window, &fbw, &fbh);
    Renderer::Instance().resize(fbw, fbh);

#ifdef PLATFORM_EMSCRIPTEN
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!glfwWindowShouldClose(g_window))
        mainLoop();
    glfwTerminate();
#endif

    return 0;
}
