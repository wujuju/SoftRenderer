#pragma once
#include "component.h"
#include "Mesh.h"

class MeshRenderComponent : public Component
{
    void Start() override;
public:
    Mesh mesh;
};
