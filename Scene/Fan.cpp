#define LOG_TAG "Fan3D"

/*
 * Author: Sim Hui Min 2503506
 * Email: s.huimin@digipen.edu
 * File: Fan.cpp
 * Brief: Implementation of the 3d table fan (CSD2101 Assignment 2)
 */

#include "Fan.h"
#include "ShaderHelper.h"
 
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>
 
namespace {
 
// part colours -- uploaded through the PARTCOLOR uniform per draw call
const glm::vec3 kBaseColour(0.45f, 0.28f, 0.12f);   // brown
const glm::vec3 kPoleColour(0.55f, 0.55f, 0.58f);   // light gray
const glm::vec3 kHubColour (0.20f, 0.20f, 0.22f);   // dark gray
 
const glm::vec3 kBladeColours[4] = {
    { 0.90f, 0.10f, 0.10f },   // blade 0: red
    { 0.10f, 0.35f, 0.95f },   // blade 1: blue
    { 1.00f, 0.55f, 0.05f },   // blade 2: orange
    { 0.10f, 0.75f, 0.25f },   // blade 3: green
};
 
// 8 unique vertices of a unit cube centred at the origin (x, y, z)
const GLfloat cubePositions[] = {
    -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f,   // V0, V1  front (z = +0.5)
     0.5f,  0.5f,  0.5f,   -0.5f,  0.5f,  0.5f,   // V2, V3
    -0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f,   // V4, V5  back  (z = -0.5)
     0.5f,  0.5f, -0.5f,   -0.5f,  0.5f, -0.5f,   // V6, V7
};
 
// per-corner grayscale factor: front corners bright, back corners darker
const GLfloat cubeShades[] = {
    1.0f, 1.0f, 1.0f, 1.0f,
    0.6f, 0.6f, 0.6f, 0.6f,
};
 
// 36 indices (12 triangles), ccw winding to match GL_CULL_FACE / GL_CCW
const GLushort cubeIndices[] = {
    0, 1, 2,   0, 2, 3,    // front
    1, 5, 6,   1, 6, 2,    // right
    5, 4, 7,   5, 7, 6,    // back
    4, 0, 3,   4, 3, 7,    // left
    3, 2, 6,   3, 6, 7,    // top
    4, 5, 1,   4, 1, 0,    // bottom
};
 
} // namespace
 
#ifdef PLATFORM_ANDROID
Fan::Fan(AAssetManager* mgr) : assetMgr(mgr) { modelType = FanType; }
#else
Fan::Fan() { modelType = FanType; }
#endif
 
Fan::~Fan() {
    if (program) glDeleteProgram(program);
    if (vbo)     glDeleteBuffers(1, &vbo);
    if (ibo)     glDeleteBuffers(1, &ibo);
    if (vao)     glDeleteVertexArrays(1, &vao);
}
 
void Fan::InitModel() {
    LOGI("Fan::InitModel");
 
#ifdef PLATFORM_ANDROID
    program = ShaderHelper::buildProgramFromAssets(
        assetMgr, "shader/FanVertex.glsl", "shader/FanFragment.glsl");
#else
    program = ShaderHelper::buildProgramFromFile(
        "FanVertex.glsl", "FanFragment.glsl");
#endif
 
    if (!program) { LOGE("Fan: shader program failed"); return; }
 
    glUseProgram(program);
    mvpLocation   = glGetUniformLocation(program, "MODELVIEWPROJECTIONMATRIX");
    colorLocation = glGetUniformLocation(program, "PARTCOLOR");
 
    // one vbo, two sub-regions: positions first, shades appended
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions) + sizeof(cubeShades),
                 nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,                     sizeof(cubePositions), cubePositions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubePositions), sizeof(cubeShades),    cubeShades);
 
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
 
    // one vao capturing both layout-qualified attribute pointers + the ibo
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);   // layout(location = 0) VertexPosition
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);   // layout(location = 1) VertexShade
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(cubePositions));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBindVertexArray(0);
 
    // depth test, back-face culling, all four stacks reset to identity
    transform.TransformInit();
}
 
void Fan::Resize(int w, int h) {
    float aspect = (h > 0) ? static_cast<float>(w) / static_cast<float>(h) : 1.0f;
 
    transform.TransformSetMatrixMode(PROJECTION_MATRIX);
    transform.TransformLoadIdentity();
    transform.TransformSetPerspective(glm::radians(60.0f), aspect, 0.01f, 1000.0f, 0.0f);
 
    // view stack stays identity
    transform.TransformSetMatrixMode(VIEW_MATRIX);
    transform.TransformLoadIdentity();
 
    transform.TransformSetMatrixMode(MODEL_MATRIX);
    LOGI("Fan::Resize %d x %d  aspect=%.3f", w, h, aspect);
}
 
void Fan::Render() {
    if (!program) return;
 
    // part 2 -- advance the rotation
    if (fanOn) spinAngle += kBaseSpeed + dragBoost;
    if (spinAngle >= 360.0f) spinAngle -= 360.0f;

    // stretch: after release the boost decays each frame (fling wind-down)
    if (!dragging && dragBoost > 0.0f) {
        dragBoost *= kFlingDecay;
        if (dragBoost < 0.01f) dragBoost = 0.0f;
    }
 
    glUseProgram(program);
    glBindVertexArray(vao);
 
    transform.TransformSetMatrixMode(MODEL_MATRIX);
    transform.TransformLoadIdentity();
 
    // world: pull-back + 3d tilt, shared by every part
    transform.TransformTranslate(0.0f, 0.8f, -8.0f);
    transform.TransformRotate(glm::radians(20.0f), 0.0f, 1.0f, 0.0f);
 
    // base
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, -2.6f, 0.0f);
    transform.TransformScale(1.6f, 0.25f, 0.8f);
    drawPart(kBaseColour);
    transform.TransformPopMatrix();
 
    // pole
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, -1.21f, 0.0f);
    transform.TransformScale(0.15f, 2.53f, 0.15f);
    drawPart(kPoleColour);
    transform.TransformPopMatrix();
 
    // hub
    transform.TransformPushMatrix();
    transform.TransformTranslate(0.0f, 0.2f, 0.0f);
    transform.TransformScale(0.3f, 0.3f, 0.3f);
    drawPart(kHubColour);
    transform.TransformPopMatrix();
 
    // blades -- pivot at the hub centre, 90 deg apart, spun by spinAngle
    for (int i = 0; i < 4; ++i) {
        transform.TransformPushMatrix();
        transform.TransformTranslate(0.0f, 0.2f, 0.15f);
        transform.TransformRotate(glm::radians(spinAngle + i * 90.0f), 0.0f, 0.0f, 1.0f);
        transform.TransformTranslate(0.0f, 0.55f, 0.0f);
        transform.TransformScale(0.22f, 0.8f, 0.05f);
        drawPart(kBladeColours[i]);
        transform.TransformPopMatrix();
    }
 
    glBindVertexArray(0);
}
 
void Fan::drawPart(const glm::vec3& colour) {
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE,
        glm::value_ptr(*transform.TransformGetModelViewProjectionMatrix()));
    glUniform3fv(colorLocation, 1, glm::value_ptr(colour));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
}
 
void Fan::TouchEventDown(float x, float y) {
    dragging = true;
    lastX = x;
    lastY = y;
    movedDistance = 0.0f;
    lastMoveTime  = std::chrono::steady_clock::now();
}
 
void Fan::TouchEventMove(float x, float y) {
    float dx   = x - lastX;
    float dy   = y - lastY;
    float dist = std::sqrt(dx * dx + dy * dy);      // pixels this event
    movedDistance += dist;
 
    auto  now  = std::chrono::steady_clock::now();
    float dtMs = std::chrono::duration<float, std::milli>(now - lastMoveTime).count();
 
    if (dtMs > 0.0f) {
        float velocity = dist / dtMs;               // pixels per millisecond
        dragBoost = std::min(velocity * kBoostScale, kMaxBoost);
        LOGI("Fan: swipe velocity %.3f px/ms -> boost %.2f", velocity, dragBoost);
    }
 
    lastMoveTime = now;
    lastX = x;
    lastY = y;
}
 
void Fan::TouchEventRelease(float /*x*/, float /*y*/) {
    dragging = false;

    // little movement between down and release -> it was a tap
    if (movedDistance < kTapThreshold) {
        fanOn = !fanOn;
        dragBoost = 0.0f;   // a tap carries no fling
        LOGI("Fan %s", fanOn ? "ON" : "OFF");
    }
    // a swipe keeps its boost -- Render() decays it (stretch goal)
}