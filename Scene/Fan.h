#pragma once

/*
 * Author: Sim Hui Min 2503506
 * Email: s.huimin@digipen.edu
 * File: Fan.h
 * Brief: 3d table fan built from one shared cube (CSD2101 Assignment 2)
 */
 
#include "Platform.h"
#include "Model.h"
#include "Transform.h"
 
#include <glm/glm.hpp>
#include <chrono>
 
#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif
 
class Fan : public Model {
public:
#ifdef PLATFORM_ANDROID
    explicit Fan(AAssetManager* mgr);
#else
    Fan();
#endif
    ~Fan() override;
 
    void InitModel() override;
    void Render()    override;
    void Resize(int w, int h) override;
 
    void TouchEventDown(float x, float y)    override;
    void TouchEventMove(float x, float y)    override;
    void TouchEventRelease(float x, float y) override;
 
private:
    // uploads mvp + part colour, then draws the one shared cube
    void drawPart(const glm::vec3& colour);
 
#ifdef PLATFORM_ANDROID
    AAssetManager* assetMgr = nullptr;
#endif
 
    Transform transform;          // provided matrix stack (model/view/projection)
    float spinAngle = 0.0f;       // current blade angle (degrees)
    bool  fanOn     = true;       // toggled by a tap
    float dragBoost = 0.0f;       // extra speed while dragging, 0 otherwise
 
    static constexpr float kBaseSpeed    = 1.5f;   // deg/frame when ON
    static constexpr float kMaxBoost     = 20.0f;  // clamp for the drag boost
    static constexpr float kBoostScale   = 8.0f;   // px/ms -> deg/frame
    static constexpr float kTapThreshold = 12.0f;  // px: below this it's a tap
 
    float lastX = 0.0f, lastY = 0.0f;  // previous touch position
    float movedDistance = 0.0f;        // total movement since touch-down
    std::chrono::steady_clock::time_point lastMoveTime;
 
    // gl objects -- one cube, one vao, one program shared by all seven parts
    GLuint vao = 0, vbo = 0, ibo = 0, program = 0;
    GLint  mvpLocation = -1, colorLocation = -1;
};