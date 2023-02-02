#include "rendersystem.h"


#include "camera.h"
#include "coordinator.h"
#include "light.h"

extern Coordinator gCoordinator;
GlobalShaderData* gl;
int _min_x; // 三角形外接矩形
int _min_y;
int _max_x;
int _max_y;

inline bool is_back_facing(shader_struct_v2f* v2f)
{
    Vector4f& a = v2f[0].pos;
    Vector4f& b = v2f[1].pos;
    Vector4f& c = v2f[2].pos;
    float signed_area = a.x * b.y - a.y * b.x +
        b.x * c.y - b.y * c.x +
        c.x * a.y - c.y * a.x;
    return signed_area <= 0;
}

void RenderSystem::Init()
{
    frame_buff = new FrameBuff(ScreenW,ScreenH);
    shadow_buff = new FrameBuff(ScreenW,ScreenH);
    gl = new GlobalShaderData();
    gl->targetBuffer = frame_buff;
}

void RenderSystem::Update(float dt)
{
    frame_buff->clear_color();
    frame_buff->clear_depth(0);
    // frame_buff->clear_depth(std::numeric_limits<float>::max());
    if (isDrawShadow)
    {
        shadow_buff->clear_color();
        shadow_buff->clear_depth(std::numeric_limits<float>::max());
        gl->shadow_buff = shadow_buff;
    }
    else
    {
        gl->shadow_buff = nullptr;
    }

    auto& camera = GameObject::find("camera")->GetComponent<Camera>();
    auto& light = GameObject::find("light")->GetComponent<Light>();

    gl->light_color = light.color;
    gl->light_dir = light.gameObject->transform->position;
    gl->view_Pos = camera.gameObject->transform->position;
    gl->projection_matrix = camera.projectionMatrix();
    gl->view_matrix = camera.worldToCameraMatrix();
    gl->light_vp_matrix = orthographic(camera.aspect, 1, 0, 5) * lookat(gl->light_dir,
                                                                        camera.target, Vector3f::up);
    gl->camera_vp_matrix = gl->projection_matrix * gl->view_matrix;


    for (auto const& entity : mEntities)
    {
        auto& meshRenders = gCoordinator.GetComponent<MeshRenderComponent>(entity);
        Mesh* mesh = &meshRenders.mesh;
        if (mesh == nullptr)
            continue;
        Shader* shader = mesh->material->shader;
        if (shader == nullptr)
            continue;
        const bool isRenderShadow = mesh->material->shadow_shader != nullptr && isDrawShadow;

        if (isRenderShadow)
            RasterizeFrameBuff(&meshRenders, mesh->material->shadow_shader, shadow_buff);
        RasterizeFrameBuff(&meshRenders, mesh->material->shader, frame_buff);
    }
}

void RenderSystem::RasterizeFrameBuff(MeshRenderComponent* meshRender, Shader* shader, FrameBuff* buff)
{
    const Mesh* mesh = &meshRender->mesh;
    shader->gl = gl;
    shader->model_matrix = meshRender->gameObject->transform->GetModelMatrix();
    shader->model_matrix_I = matrix_invert(shader->model_matrix);
    shader->mvp_matrix = gl->camera_vp_matrix * shader->model_matrix;
    const size_t count = mesh->vertices.size();
    for (int i = 0; i < count; i += 3)
    {
        for (int j = 0; j < 3; j++)
        {
            shader_struct_v2f& vertex = shader->inputs[j];
            shader_struct_a2v& a2v = shader->a2v[j];
            a2v.vertex = mesh->vertices[i + j];
            a2v.normal = mesh->normals[i + j];
            a2v.tangent = mesh->tangent[i + j];
            a2v.uv = mesh->uv[i + j];
            vertex.pos = shader->vertex(a2v, vertex.context);
            // vertex.pos = a2v.vertex.xyz1();
            // 齐次坐标空间 /w 归一化到单位体积 cvv
            vertex.pos *= 1 / vertex.pos.w;
            vertex.rhz = 1 / vertex.pos.z;
            // 计算屏幕坐标
            vertex.spf.x = (vertex.pos.x + 1.0f) * ScreenW * 0.5f;
            vertex.spf.y = (vertex.pos.y + 1.0f) * ScreenH * 0.5f;
            vertex.spf.z = (vertex.pos.z + 1.0f) * 0.5f;
            // 整数屏幕坐标：加 0.5 的偏移取屏幕像素方格中心对齐
            Vector2i& spi = vertex.spi;
            spi.x = (int)(vertex.spf.x);
            spi.y = (int)(vertex.spf.y);

            // 更新外接矩形范围
            if (j == 0)
            {
                _min_x = _max_x = Between(0, RenderW, spi.x);
                _min_y = _max_y = Between(0, RenderH, spi.y);
            }
            else
            {
                _min_x = Between(0, RenderW, Min(_min_x, spi.x));
                _max_x = Between(0, RenderW, Max(_max_x, spi.x));
                _min_y = Between(0, RenderH, Min(_min_y, spi.y));
                _max_y = Between(0, RenderH, Max(_max_y, spi.y));
            }
        }

        this->RasterizeTriangle(shader, buff);
    }
}

void RenderSystem::RasterizeTriangle(Shader* shader, FrameBuff* buff)
{
    shader_struct_v2f* inputs = shader->inputs;
    // 背面剔除
    if (shader->isCull && is_back_facing(inputs))
        return;

    shader_struct_v2f& inputA = inputs[0];
    shader_struct_v2f& inputB = inputs[1];
    shader_struct_v2f& inputC = inputs[2];

    // 绘制线框
    if (isDrawLine)
    {
        DrawLine(inputA.spi, inputB.spi, buff, Color::white);
        DrawLine(inputA.spi, inputC.spi, buff, Color::white);
        DrawLine(inputC.spi, inputB.spi, buff, Color::white);

        return;
    }


    if (fillTriangleType == 0)
    {
        FillTriangle(inputA, inputB, inputC, shader, buff);
    }
    else
    {
        FillTriangle2(inputA, inputB, inputC, shader, buff);
    }
}


void RenderSystem::FillTriangle2(shader_struct_v2f& inputA, shader_struct_v2f& inputB, shader_struct_v2f& inputC,
                                 Shader* shader, FrameBuff* buff)
{
    Vector2i point;
    Vector3f vectorUVA;
    Vector3f vectorUVB;

    vectorUVA.x = inputA.spf.x - inputC.spf.x;
    vectorUVA.y = inputB.spf.x - inputC.spf.x;

    vectorUVB.x = inputA.spf.y - inputC.spf.y;
    vectorUVB.y = inputB.spf.y - inputC.spf.y;

    for (point.x = _min_x; point.x <= _max_x; point.x++)
    {
        for (point.y = _min_y; point.y <= _max_y; point.y++)
        {
            vectorUVA.z = inputC.spf.x - point.x;
            vectorUVB.z = inputC.spf.y - point.y;

            Vector3f u = vector_cross(vectorUVA, vectorUVB);
            float a = u.x / u.z;
            float b = u.y / u.z;
            float c = 1.f - a - b;

            if (a < 0 || b < 0 || c < 0)
                continue;

            float rhz = inputA.rhz * a + inputB.rhz * b + inputC.rhz * c;
            // 深度测试
            if (rhz < buff->get_depth(point))
                continue;
            buff->set_depth(point, rhz);
            float z = 1.0f / ((rhz != 0.0f) ? rhz : 1.0f);

            a = inputA.rhz * a * z;
            b = inputB.rhz * b * z;
            c = inputC.rhz * c * z;

            // 计算三个顶点插值 varying 的系数
            shader->varying(a, b, c);
            // fragment shader
            Color color = shader->fragment();
            // buff->set_depth(point, frag_depth);
            buff->set_color(point, vector_to_color(color));
        }
    }
}

void RenderSystem::FillTriangle(shader_struct_v2f& inputA, shader_struct_v2f& inputB, shader_struct_v2f& inputC,
                                Shader* shader, FrameBuff* buff)
{
    Vector2i aPoint = inputA.spi, bPoint = inputB.spi, cPoint = inputC.spi;
    if (aPoint.y == bPoint.y && aPoint.y == cPoint.y)
        return;
    if (aPoint.x == bPoint.x && aPoint.x == cPoint.x)
        return;
    //根据y的大小对坐标进行排序
    if (aPoint.y > bPoint.y)
        std::swap(aPoint, bPoint);
    if (aPoint.y > cPoint.y)
        std::swap(aPoint, cPoint);
    if (bPoint.y > cPoint.y)
        std::swap(bPoint, cPoint);
    Vector2i AC = (cPoint - aPoint);
    Vector2i AB = (bPoint - aPoint);
    Vector2i BC = (cPoint - bPoint);
    float total_height = AC.y;

    //以高度差作为循环控制变量，此时不需要考虑斜率，因为着色完后每行都会被填充
    for (int i = 0; i <= total_height; i++)
    {
        //根据t1将三角形分割为上下两部分
        bool second_half = i > AB.y || AB.y == 0;
        //计算A,B两点的坐标
        float index = i;
        Vector2i A = aPoint.xy() + AC * (index / total_height);
        Vector2i B = second_half ? bPoint + BC * ((index - AB.y) / BC.y) : (aPoint + AB * (index / AB.y));

        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j ++)
        {
            Vector2f point = {(float)j, (float)(aPoint.y + i)};
            //上面可能对点做了调换，这里我们改成用PA PB PC去求面积
            Vector2f PA = inputA.spf.xy() - point;
            Vector2f PB = inputB.spf.xy() - point;
            Vector2f PC = inputC.spf.xy() - point;

            float a = Abs(vector_cross(PB, PC));
            float b = Abs(vector_cross(PC, PA));
            float c = Abs(vector_cross(PA, PB));
            float s = 1.f / (a + b + c);

            a = a * s;
            b = b * s;
            c = c * s;

            float rhz = inputA.rhz * a + inputB.rhz * b + inputC.rhz * c;
            // 深度测试
            if (rhz < buff->get_depth(point))
                continue;
            Vector2i pp = {(int)point.x, (int)point.y};
            buff->set_depth(pp, rhz);
            float z = 1.0f / ((rhz != 0.0f) ? rhz : 1.0f);

            a = inputA.rhz * a * z;
            b = inputB.rhz * b * z;
            c = inputC.rhz * c * z;

            // 计算三个顶点插值 varying 的系数 
            shader->varying(a, b, c);
            // // fragment shader
            Color color = shader->fragment();
            buff->set_color(pp, color);
        }
    }
}


void RenderSystem::DrawLine(Vector2i a, Vector2i b, FrameBuff* buff, Color color)
{
    //判断线段斜率的绝对值是否大于1
    bool steep = false;
    //大于1置为true，交换坐标各自的x和y。即变换为关于y=x或y=-x对称的点
    if (std::abs(a.x - b.x) < std::abs(a.y - b.y))
    {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
        steep = true;
    }
    //保证坐标2的x,y大于坐标1的x,y。
    if (a.x > b.x)
    {
        std::swap(a, b);
    }

    //此时x1大于x0，且斜率在-1到1之间，用x做循环控制变量
    for (int x = a.x; x <= b.x; x++)
    {
        //根据x计算线段对应的y
        float t = (x - a.x) / (float)(b.x - a.x);
        int y = a.y * (1. - t) + b.y * t;
        //若斜率大于1，真实坐标为(y,x)；否则为(x,y)
        if (steep)
            buff->set_color(y, x, color);
        else
            buff->set_color(x, y, color);
    }
}
