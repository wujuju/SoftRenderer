#pragma once
#include "shader.h"

class Material
{
public:
    ~Material()
    {
        if (shader)
        {
            delete shader;
            shader = nullptr;
        }
        if (shadow_shader)
        {
            delete shadow_shader;
            shadow_shader = nullptr;
        }
    }

    Shader* shader;
    Shader* shadow_shader;
};
