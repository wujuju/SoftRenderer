#pragma once
#include "framebuff.h"
#include "graphics.h"
#include "../base/tgaimage.h"

struct Shader
{
    GlobalShaderData* gl;
    bool isCull = true;
    float gloss = 100;
    Color specularColor = Color::white;
    Color difuseColor = Color::white;
    TGAImage* texture;
    TGAImage* normalTexture;
    shader_struct_a2v a2v[3];
    shader_struct_v2f inputs[3];
    shader_context context;

    int num_floats = sizeof(shader_context) / sizeof(float);
    float* contextA2 = (float*)(&inputs[0].context);
    float* contextB2 = (float*)(&inputs[1].context);
    float* contextC2 = (float*)(&inputs[2].context);
    float* contextDist = (float*)(&context);

    Matrix4x4 mvp_matrix;
    Matrix4x4 model_matrix;
    Matrix4x4 model_matrix_I;

    inline virtual Vector4f vertex(shader_struct_a2v& a2v, shader_context& output) = 0;
    inline virtual Color fragment() = 0;
    // inline virtual void varying(const float a, const float b, const float c) = 0;

    inline virtual void varying(float a, float b, float c)
    {
        for (int i = 0; i < num_floats; i++)
        {
            contextDist[i] = contextA2[i] * a + contextB2[i] * b + contextC2[i] * c;
        }
    }

    inline Vector4f ObjectToClipPos(Vector3f& pos)
    {
        return mvp_matrix * pos.xyz1();
    }

    inline Vector4f ObjectToViewPos(Vector3f& pos)
    {
        return gl->light_vp_matrix * model_matrix * (pos).xyz1();
    }

    inline Vector3f ObjectToWorldPos(Vector3f& pos)
    {
        return (model_matrix * (pos).xyz1()).xyz();
    }

    inline Vector3f ObjectToWorldDir(Vector3f& dir)
    {
        return (model_matrix * dir.xyz0()).xyz();
    }

    inline Vector3f ObjectToWorldNormal(Vector3f& normal)
    {
        return ((normal.xyz1() * model_matrix_I).xyz());
    }

    inline Vector3f ObjSpaceViewDir(Vector3f& pos)
    {
        return (gl->view_Pos.xyz1() * model_matrix_I).xyz() - pos;
    }

    inline Vector3f ObjSpaceLightDir(Vector3f& pos)
    {
        return (gl->light_dir.xyz1() * model_matrix_I).xyz() - pos;
    }

    inline Vector3f WorldSpaceViewDir(Vector3f& worldPos)
    {
        return gl->view_Pos - worldPos;
    }

    inline Vector3f WorldSpaceLightDir(Vector3f& worldPos)
    {
        return gl->light_dir - worldPos;
    }

    inline Vector4f Normal2(const Vector2f& uvf)
    {
        Vector2i uv(uvf[0] * normalTexture->get_width(), uvf[1] * normalTexture->get_height());
        TGAColor c = normalTexture->get(uv[0], uv[1]);
        Vector4f res;
        for (int i = 0; i < 3; i++)
            res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
        return res;
    }

    inline Color tex2D(const Vector2f& uv)
    {
        return texture->getInt(uv.x * texture->get_width(), uv.y * texture->get_height());
    }

    float isInShadow(Vector4f depth_pos, float n_dot_l)
    {
        float widht = gl->shadow_buff->width;
        float height = gl->shadow_buff->height;

        Vector3f ndc_coords = (depth_pos / depth_pos.w).xyz();
        Vector3f pos = viewport_transform(widht, height, ndc_coords);
        float depth_bias = 0.05f * (1 - n_dot_l);
        if (depth_bias < 0.005f) depth_bias = 0.01f;
        float current_depth = depth_pos[2] - depth_bias;

        if (pos.x < 0 || pos.y < 0 || pos.x >= widht || pos.y >= height)
            return 1;

        float closest_depth = vector_from_color(gl->shadow_buff->get_color(pos.x, pos.y)).r;
        return current_depth < closest_depth;
    }
};




struct NormalShader : Shader
{
    Matrix3x3 TBN;

    Vector4f vertex(shader_struct_a2v& a2v, shader_context& output) override
    {
        output.uv = a2v.uv;
        output.tangent = ObjectToWorldDir(a2v.tangent);
        output.pos = ObjectToWorldPos(a2v.vertex);
        output.normal = ObjectToWorldNormal(a2v.normal);
        output.binormal = vector_cross(output.normal, output.tangent);
        return ObjectToClipPos(a2v.vertex);
    }

    Color fragment() override
    {
        Vector3f lightDir = WorldSpaceLightDir(context.pos).normalize();
        Vector3f viewDir = WorldSpaceViewDir(context.pos).normalize();
        Vector3f bump = Normal2(context.uv).xyz();
        //*****************************另外一种法线算法******************************
        float x = context.normal.x;
        float y = context.normal.y;
        float z = context.normal.z;
        float xz = std::sqrt(x * x + z * z);
        context.tangent = Vector3f(x * y / xz, xz, z * y / xz);
        context.binormal = vector_cross(context.normal, context.tangent);
        bump.z = sqrt(1.0 - saturate(vector_dot(Vector2f(bump.x, bump.y), Vector2f(bump.x, bump.y))));
        //*****************************另外一种法线算法******************************
        
        TBN.SetCol(0, context.tangent);
        TBN.SetCol(1, context.binormal);
        TBN.SetCol(2, context.normal);
        
        Vector3f normal = (TBN * bump).normalize();
        // Vector3f normal = context.normal.normalize();

        Color albedo = tex2D(context.uv);
        Color ambient = gl->ambient * albedo;
        float n_dot_l = saturate(vector_dot(normal, lightDir));
        Color diffuse = albedo * n_dot_l;

        Vector3f halfDir = (viewDir + lightDir).normalize();
        float reflectDir = saturate(vector_dot(normal, halfDir));
        Color spcular = specularColor * pow(reflectDir, gloss);

        // return albedo;
        // return Color::White * saturate(vector_dot(normal, lightDir));
        float shadow = 1;
        if (gl->shadow_buff != nullptr)
        {
            Vector4f depth_pos = gl->light_vp_matrix * context.pos.xyz1();
            shadow = isInShadow(depth_pos, n_dot_l);
        }

        return (ambient + (diffuse + spcular) * gl->light_color * shadow);
    }
};


struct BlinnShader : Shader
{
    Matrix3x3 TBN;

    Vector4f vertex(shader_struct_a2v& a2v, shader_context& output) override
    {
        output.uv = a2v.uv;
        output.tangent = ObjectToWorldDir(a2v.vertex);
        output.normal = ObjectToWorldNormal(a2v.normal);
        return ObjectToClipPos(a2v.vertex);
    }

    Color fragment() override
    {
        Vector3f lightDir = WorldSpaceLightDir(context.tangent).normalize();
        Vector3f viewDir = WorldSpaceViewDir(context.tangent).normalize();
        Vector3f normal = context.normal.normalize();

        Color albedo = tex2D(context.uv);
        Color ambient = gl->ambient * albedo;
        Color diffuse = albedo * saturate(vector_dot(normal, lightDir));

        Vector3f halfDir = (viewDir + lightDir).normalize();
        float reflectDir = saturate(vector_dot(normal, halfDir));
        Color spcular = specularColor * pow(reflectDir, gloss);

        return (ambient + (diffuse + spcular) * gl->light_color);
    }
};

struct ShadowShader : Shader
{
    Vector4f vertex(shader_struct_a2v& a2v, shader_context& output) override
    {
        Vector4f pos = ObjectToViewPos(a2v.vertex);
        output.pos = pos.xyz();
        return pos;
    }

    Color fragment() override
    {
        return Color::white * context.pos.z;
    }
};
