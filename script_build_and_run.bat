@echo off
rem script_build_and_run.bat -- configure, build, and run the desktop target.
rem first configure downloads GLFW, GLEW, and GLM -- allow a few minutes.

cmake -B build -A x64
if errorlevel 1 (
    echo Configuration failed.
    pause
    exit /b 1
)

cmake --build build --config Release
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

cd build\Release
Fan3D.exe
cd ..\..
