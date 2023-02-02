#pragma once
#include "../base/mathapi.h"

class Transform : public Component
{
    // private:
public:
    Vector3f position;
    Vector3f localScale = Vector3f::one;
    Vector3f rotation;
    Transform* parent;

    inline Matrix4x4 GetModelMatrix()
    {
        Matrix4x4 m_translation = translate(position.x, position.y, position.z);
        Matrix4x4 m_rotate = rotate_z(TO_RADIANS(rotation.z)) * rotate_x(TO_RADIANS(rotation.x)) * rotate_y(
            TO_RADIANS(rotation.y));
        Matrix4x4 m_scale = scale(localScale.x, localScale.y, localScale.z);
        return m_translation * m_rotate * m_scale;
    }
};
