#pragma once
 
/**
 * Model.h
 *
 * Abstract base class for all renderable objects.
 * Platform-agnostic – shared by Android, Desktop, and WebGL builds.
 */
 
enum ModelType {
    TriangleType = 0,
    CubeType     = 1,
    FanType      = 2
};
 
class Model {
public:
    Model() {}
    virtual ~Model() {}
 
    virtual void InitModel() = 0;
    virtual void Render()    = 0;
    virtual void Update(float dt) {}
    virtual void Resize(int w, int h) {}
    virtual ModelType GetModelType() { return modelType; }
 
    virtual void TouchEventDown(float x, float y)    {}
    virtual void TouchEventMove(float x, float y)    {}
    virtual void TouchEventRelease(float x, float y) {}
 
protected:
    ModelType modelType = TriangleType;
};
 