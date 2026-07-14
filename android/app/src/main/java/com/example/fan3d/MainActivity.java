package com.example.fan3d;

/**
 * MainActivity.java
 *
 * Hosts a GLSurfaceView with an OpenGL ES 3.0 context and forwards touch
 * input to native code.
 *
 * The GL render thread is NOT the UI thread: onTouchEvent runs on the UI
 * thread, so every native call is wrapped in glSurfaceView.queueEvent(...)
 * to execute it safely on the render thread. The gesture mapping matches
 * desktop/web:
 *   ACTION_DOWN        -> nativeTouchDown
 *   ACTION_MOVE        -> nativeTouchMove
 *   ACTION_UP / CANCEL -> nativeTouchRelease
 */

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {

    // must match the native library name in android/app CMakeLists (add_library)
    static {
        System.loadLibrary("glpiframeworkintro");
    }

    // JNI bridge -- implemented in NativeTemplate.cpp; the exported symbols
    // must follow the rule Java_com_example_fan3d_MainActivity_<method>
    public static native void nativeSetAssetManager(AssetManager mgr);
    public static native void nativeInit();
    public static native void nativeResize(int width, int height);
    public static native void nativeRender();
    public static native void nativeTouchDown(float x, float y);
    public static native void nativeTouchMove(float x, float y);
    public static native void nativeTouchRelease(float x, float y);

    private FanGLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // hand the APK asset manager to native code before GL init so
        // ShaderHelper can load the .glsl files from assets/shader/
        nativeSetAssetManager(getAssets());
        glView = new FanGLSurfaceView(this);
        setContentView(glView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        glView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        glView.onResume();
    }

    // -----------------------------------------------------------------------
    // GLSurfaceView -- ES 3.0 context + touch forwarding via queueEvent
    // -----------------------------------------------------------------------
    private static class FanGLSurfaceView extends GLSurfaceView {

        FanGLSurfaceView(Context context) {
            super(context);
            setEGLContextClientVersion(3);
            setEGLConfigChooser(8, 8, 8, 8, 24, 8);   // rgba8, depth 24, stencil 8
            setRenderer(new FanRenderer());
        }

        @Override
        public boolean onTouchEvent(final MotionEvent event) {
            final float x = event.getX();
            final float y = event.getY();

            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    queueEvent(new Runnable() {
                        @Override public void run() { nativeTouchDown(x, y); }
                    });
                    break;

                case MotionEvent.ACTION_MOVE:
                    queueEvent(new Runnable() {
                        @Override public void run() { nativeTouchMove(x, y); }
                    });
                    break;

                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    queueEvent(new Runnable() {
                        @Override public void run() { nativeTouchRelease(x, y); }
                    });
                    break;
            }
            return true;
        }

        private static class FanRenderer implements GLSurfaceView.Renderer {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                nativeInit();
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                nativeResize(width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                nativeRender();
            }
        }
    }
}