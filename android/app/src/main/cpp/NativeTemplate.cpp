#define LOG_TAG "NativeTemplate"

/**
 * NativeTemplate.cpp
 *
 * JNI bridge between MainActivity.java and the shared C++ Renderer.
 *
 * Symbol-name rule: each exported function must be named
 *   Java_<package with dots as underscores>_<class>_<method>
 * so com.example.fan3d.MainActivity.nativeInit() binds to
 * Java_com_example_fan3d_MainActivity_nativeInit. If any name drifts,
 * the app throws UnsatisfiedLinkError at runtime.
 */

#include <jni.h>
#include <android/asset_manager_jni.h>

#include "Platform.h"
#include "Renderer.h"

// global ref keeps the Java AssetManager alive for the lifetime of the
// native AAssetManager* handed to the Renderer
static jobject g_assetManagerRef = nullptr;

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeSetAssetManager(JNIEnv* env,
                                                          jclass /*clazz*/,
                                                          jobject assetManager)
{
    if (g_assetManagerRef) env->DeleteGlobalRef(g_assetManagerRef);
    g_assetManagerRef = env->NewGlobalRef(assetManager);
    Renderer::Instance().setAssetManager(
        AAssetManager_fromJava(env, g_assetManagerRef));
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeInit(JNIEnv* /*env*/, jclass /*clazz*/)
{
    Renderer::Instance().initializeRenderer();
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeResize(JNIEnv* /*env*/, jclass /*clazz*/,
                                                 jint width, jint height)
{
    Renderer::Instance().resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeRender(JNIEnv* /*env*/, jclass /*clazz*/)
{
    Renderer::Instance().render();
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeTouchDown(JNIEnv* /*env*/, jclass /*clazz*/,
                                                    jfloat x, jfloat y)
{
    Renderer::Instance().TouchEventDown(x, y);
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeTouchMove(JNIEnv* /*env*/, jclass /*clazz*/,
                                                    jfloat x, jfloat y)
{
    Renderer::Instance().TouchEventMove(x, y);
}

JNIEXPORT void JNICALL
Java_com_example_fan3d_MainActivity_nativeTouchRelease(JNIEnv* /*env*/, jclass /*clazz*/,
                                                       jfloat x, jfloat y)
{
    Renderer::Instance().TouchEventRelease(x, y);
}

} // extern "C"