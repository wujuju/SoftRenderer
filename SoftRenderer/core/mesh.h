#pragma once
#include <vector>

#include "material.h"

class Mesh
{
public:
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uv;
    std::vector<Vector3f> tangent;
    Material* material;
};
