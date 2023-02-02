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
    ~RenderSystem()
    {
        if (frame_buff)
        {
            delete[] frame_buff;
            frame_buff = nullptr;
        }
        if (shadow_buff)
        {
            delete[] shadow_buff;
            shadow_buff = nullptr;
        }
        if (gl)
        {
            delete gl;
            gl = nullptr;
        }
    }

    FrameBuff* frame_buff;
    FrameBuff* shadow_buff;
    GlobalShaderData* gl;
    int _min_x; // 三角形外接矩形
    int _min_y;
    int _max_x;
    int _max_y;
    void Init();
    void Update(float dt);
    bool isDrawShadow = false;
    bool isDrawLine = false;
    int fillTriangleType = 0;
};
