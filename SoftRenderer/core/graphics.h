#pragma once

#include "../base/mathapi.h"
class Material;
class FrameBuff;

struct shader_context
{
    Vector2f uv;
    Vector3f normal;
    Vector3f pos;
    Vector3f tangent;
    Vector3f binormal;
};

struct shader_struct_a2v
{
    Vector3f vertex;
    Vector3f normal;
    Vector2f uv;
    Vector3f tangent;
};

struct shader_struct_v2f
{
    shader_context context;
    Vector4f pos;
    Vector3f spf; // 浮点数屏幕坐标
    Vector2i spi; // 整数屏幕坐标
    float rhz;
};

struct GlobalShaderData
{
    FrameBuff* targetBuffer;
    FrameBuff* shadow_buff;
    Vector3f view_Pos;
    Vector3f light_dir;
    Color light_color;
    Color ambient = Color(54.f / 255, 58.f / 255, 66.f / 255);

    Matrix4x4 view_matrix;
    Matrix4x4 projection_matrix;
    Matrix4x4 light_vp_matrix;
    Matrix4x4 camera_vp_matrix;
};
