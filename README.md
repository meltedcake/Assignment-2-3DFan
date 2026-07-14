# Assignment 2 — 3D Fan

A 3D table fan (base, pole, hub, four spinning blades) built from **one cube**
drawn seven times through the provided `Transform` matrix stack, running on
**Windows desktop**, **Android**, and **WebGL** from one shared C++ code base.

- Tap / click toggles the fan ON/OFF (logged as `Fan ON` / `Fan OFF`).
- Swipe / click-drag boosts the spin speed proportionally to drag velocity;
  releasing returns the fan to its base speed.

## Repository layout

```
Assignment-2-3DFan/
├── .github/workflows/deploy-pages.yml   CI/CD (build web + deploy to Pages)
├── CMakeLists.txt                       desktop + web entry point
├── script_build_and_run.bat             desktop build-and-run wrapper
├── build_web.bat / build_web.sh         web build (Emscripten)
├── cmake/ImportDependencies.cmake       fetches GLFW + GLEW when needed
├── lib/                                 local dependency copies (glm, ...)
├── Scene/                               shared by all three platforms
│   ├── Platform.h, Model.h, Renderer.h, Renderer.cpp, ShaderHelper.h
│   ├── Transform.h, Transform.cpp       (provided, unmodified)
│   ├── Fan.h, Fan.cpp                   (assignment implementation)
│   └── main.cpp
├── android/                             Android Studio / Gradle project
│   └── app/src/main/assets/shader/      FanVertex.glsl, FanFragment.glsl
└── README.md
```

Build output (`build/`, `web/`, `.gradle/`, `.idea/`, `app/build/`,
`app/.cxx/`) is generated and intentionally not committed.

## Build — Desktop (Windows)

Prerequisites:
- Visual Studio 2022 with the *Desktop development with C++* workload
- CMake 3.20+ on PATH

Steps (from the repo root):
```bat
script_build_and_run.bat
```
The first configure downloads GLFW and GLEW automatically (allow a few
minutes); GLM is used from `lib/glm`. The build output appears in
`build\Release\Fan3D.exe` and the script launches it with the shaders copied
next to the executable. **ESC** quits.

Expected result: brown base, gray pole, dark hub, four coloured blades
spinning. Click toggles ON/OFF (`Fan ON` / `Fan OFF` in the console);
click-drag speeds the blades up, release returns them to base speed.

## Build — Android

Prerequisites:
- Android Studio with NDK r27 and CMake components installed
- A device or emulator (API 24+)

Steps:
1. Open the `android/` folder in Android Studio.
2. Let Gradle sync, then press **Run**.
   (Command line alternative: `cd android` then `gradlew assembleDebug`;
   the APK appears in `android\app\build\outputs\apk\debug\`.)

The build passes `-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON`, so the app
installs on 16 KB page-size devices and emulator images.

Verify the gesture logs:
```bat
adb logcat -s Fan3D
```
(If `adb` is not on PATH it lives at
`%LOCALAPPDATA%\Android\Sdk\platform-tools\adb.exe`.)

## Build — Web (Emscripten)

Prerequisites — one-time Emscripten SDK install:
```powershell
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
.\emsdk install latest
.\emsdk activate latest
```

Per terminal, before building (PowerShell shown; in cmd use
`emsdk_env.bat` and skip the policy line):
```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
path\to\emsdk\emsdk_env.ps1
```

Build (from the repo root):
```bat
build_web.bat
```
The script requires Ninja; if it is not on PATH it automatically falls back
to the copy shipped with the Android SDK. Output appears as
`web\index.html` (plus `.js`, `.wasm`, and `.data` containing the preloaded
shaders).

Serve locally — must be run **from the repo root**:
```powershell
python -m http.server 8080 --directory web
```
Then open http://localhost:8080 in a browser. Gesture logs appear in the
browser developer console (F12).

## CI/CD and live build

`.github/workflows/deploy-pages.yml` runs on every push to `main`:
1. installs the Emscripten SDK on the runner,
2. builds the web target via `build_web.sh`,
3. deploys the `web/` folder to **GitHub Pages**.

Pages is configured under *Settings → Pages → Source: GitHub Actions*.
The live build (also entered in the class tracker) is served at:

**https://meltedcake.github.io/Assignment-2-3DFan/**

## Controls

| Input              | Effect                                          |
|--------------------|-------------------------------------------------|
| Tap / click        | Toggle fan ON/OFF (`Fan ON` / `Fan OFF` logged) |
| Swipe / click-drag | Spin speed boost proportional to drag velocity  |
| Release            | Boost resets; fan returns to base speed         |
| ESC (desktop)      | Quit                                            |
