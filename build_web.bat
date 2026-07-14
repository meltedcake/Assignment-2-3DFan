@echo off
rem build_web.bat -- emscripten web build. requires an activated emsdk.

where emcc >nul 2>nul
if errorlevel 1 (
    echo emcc not found. Activate the Emscripten SDK first:
    echo   cd path\to\emsdk
    echo   emsdk_env.bat
    pause
    exit /b 1
)

rem emcmake needs ninja; fall back to the copy in the Android SDK
where ninja >nul 2>nul
if errorlevel 1 (
    if exist "%LOCALAPPDATA%\Android\Sdk\cmake\3.22.1\bin\ninja.exe" (
        set "PATH=%PATH%;%LOCALAPPDATA%\Android\Sdk\cmake\3.22.1\bin"
    ) else (
        echo ninja not found. Install it or add it to PATH.
        pause
        exit /b 1
    )
)

call emcmake cmake -B web -G Ninja -DCMAKE_BUILD_TYPE=Release .
if errorlevel 1 (
    echo Configuration failed.
    pause
    exit /b 1
)

cmake --build web
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

echo Done. Serve it with:  python -m http.server 8080 --directory web
pause
