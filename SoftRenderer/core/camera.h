#pragma once
#include "component.h"
#include "framebuff.h"
#include "gameobject.h"
#include "meshrendercomponent.h"
#include "transform.h"

class Camera : public Component
{
public:

    float aspect;
    float Fov = TO_RADIANS(60);
    float Near = 0.3f;
    float Far = 1000;
    Vector3f target;
    
    void Start() override
    {
        aspect = ScreenW / ScreenH;
    }

    inline Matrix4x4 worldToCameraMatrix()
    {
        Matrix4x4 m = lookat(gameObject->transform->position, target, Vector3f::up);
        return m;
    }

    inline Matrix4x4 projectionMatrix()
    {
        Matrix4x4 m = perspective(Fov, aspect, Near, Far);
        return m;
    }
   
};
