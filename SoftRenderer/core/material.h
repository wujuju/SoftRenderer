#pragma once
#include "shader.h"

class Material
{
public:
    Shader* shader;
    Shader* shadow_shader;
};
