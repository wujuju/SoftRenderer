#pragma once
#include <vector>

#include "material.h"

class Mesh
{
public:
    Mesh()
    {
    }
    ~Mesh()
    {
        std::vector<Vector3f>().swap(vertices);
        std::vector<Vector3f>().swap(normals);
        std::vector<Vector2f>().swap(uv);
        std::vector<Vector3f>().swap(tangent);
        if(material)
        {
            delete material;
            material = nullptr;
        }
    }
    std::vector<Vector3f> vertices;
    std::vector<Vector3f> normals;
    std::vector<Vector2f> uv;
    std::vector<Vector3f> tangent;
    Material* material;
};
