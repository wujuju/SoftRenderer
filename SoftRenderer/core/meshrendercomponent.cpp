#include "meshrendercomponent.h"
#include "gameobject.h"
#include "../base/model.h"


void MeshRenderComponent::Start()
{
    auto model = &gameObject->GetComponent<Model>();

    mesh.material = new Material();
    mesh.material->shader = new NormalShader();
    // mesh.material->shader = new NormalShader();
    mesh.material->shader->texture = &model->diffusemap_;
    mesh.material->shader->normalTexture = &model->normalmap_;
    // mesh.material->shader = new NormalShader();
    mesh.material->shadow_shader = new ShadowShader();
    mesh.material->shadow_shader->texture = &model->diffusemap_;
    mesh.material->shadow_shader->normalTexture = &model->normalmap_;
    for (int i = 0; i < model->nfaces(); i++)
    {
        //创建face数组用于保存一个face的三个顶点坐标
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vector3f v = model->vert(face[j]);
            mesh.vertices.push_back(v);

            Vector2f uv = model->uv(i, j);
            mesh.uv.push_back(uv);

            Vector3f n = model->normal(i, j);
            mesh.normals.push_back(n);

            mesh.tangent.push_back(model->tangent(i, j));
        }
    }
}
