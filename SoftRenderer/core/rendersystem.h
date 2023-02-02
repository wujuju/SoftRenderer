#pragma once
#include "meshrendercomponent.h"
#include "shader.h"
#include "system.h"


class RenderSystem : public System
{
private:
    void RasterizeTriangle(Shader* shader, FrameBuff* buff);
    void RasterizeFrameBuff(MeshRenderComponent* meshRender, Shader* shader, FrameBuff* buff);
    void DrawLine(Vector2i a, Vector2i b, FrameBuff* image, Color color);
    void FillTriangle(shader_struct_v2f& inputA, shader_struct_v2f& inputB, shader_struct_v2f& inputC,
                               Shader* shader, FrameBuff* buff);
    void FillTriangle2(shader_struct_v2f& inputA, shader_struct_v2f& inputB, shader_struct_v2f& inputC,
                               Shader* shader, FrameBuff* buff);
public:
    FrameBuff* frame_buff;
    FrameBuff* shadow_buff;
    void Init();
    void Update(float dt);
    bool isDrawShadow = false;
    bool isDrawLine = false;
    int fillTriangleType = 0;
};
