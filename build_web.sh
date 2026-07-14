#!/usr/bin/env bash
# build_web.sh -- emscripten web build (mirror of build_web.bat)
# requires an activated emsdk (emcmake / emmake on path)
set -e
 
emcmake cmake -B web -DCMAKE_BUILD_TYPE=Release .
cmake --build web
 
# the pipeline serves the web/ folder -- ensure the produced page is index.html.
# if your cmake target outputs a different name (e.g. Fan3D.html), rename it:
if [ ! -f web/index.html ]; then
    html=$(ls web/*.html 2>/dev/null | head -n 1)
    if [ -n "$html" ]; then
        cp "$html" web/index.html
    fi
fi