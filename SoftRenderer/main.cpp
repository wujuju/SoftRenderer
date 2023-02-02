#include <GLFW/glfw3.h>
#include <time.h>
#include <chrono>
#include "base/model.h"
#include "core/camera.h"
#include "core/coordinator.h"
#include "core/light.h"
#include "core/meshrendercomponent.h"
#include "core/rendersystem.h"
#include "core/scene.h"
//定义宽度高度
const int width = 800;
const int height = 800;
Scene* Scene::defaultScene;
GameObject* camera;
GameObject* light;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_handle(GLFWwindow* window);

Coordinator gCoordinator;
RenderSystem* render_system;

void InitCoordinator()
{
    Scene::defaultScene = new Scene();
    GameObject::gCoordinator = &gCoordinator;
    gCoordinator.RegisterComponent<Camera>();
    gCoordinator.RegisterComponent<Transform>();
    gCoordinator.RegisterComponent<Light>();
    gCoordinator.RegisterComponent<MeshRenderComponent>();
    gCoordinator.RegisterComponent<Model>();

    auto renderSystem = gCoordinator.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.GetComponentType<MeshRenderComponent>());
        signature.set(gCoordinator.GetComponentType<Transform>());
        gCoordinator.SetSystemSignature<RenderSystem>(signature);
    }
    render_system = renderSystem.get();
}

void InitGameObject()
{
    camera = new GameObject("camera");
    camera->AddComponent<Camera>();
    camera->transform->position = Vector3f(0, 0, 3);

    light = new GameObject("light");
    light->AddComponent<Light>().color = Color(255.f / 255, 244.f / 255, 214.f / 255);
    light->transform->position = Vector3f(2, 2, 2);

    auto gameObject = new GameObject("model");
    auto& model = gameObject->AddComponent<Model>();
    gameObject->AddComponent<MeshRenderComponent>();
    bool tag = false;
    // tag = true;
    if (tag)
        model.set_file_name("obj/diablo3_pose/diablo3_pose.obj");
    else
        model.set_file_name("obj/african_head/african_head.obj");
}

void DrwaBuff(FrameBuff* buff)
{
    for (int y = 0; y < buff->height; y++)
    {
        for (int x = 0; x < buff->width; x ++)
        {
            int32_t intColor = buff->get_color(x, y);
            if (intColor == 0)
                continue;
            Color color = vector_from_color(intColor);
            glColor3f(color.r, color.g, color.b);
            glVertex3f(x * ScreenW2 - 1, y * ScreenH2 - 1, 0);
        }
    }
}

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Soft Renderer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //设置按键回调
    glfwSetKeyCallback(window, key_callback);

    //使用回调
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    InitCoordinator();
    InitGameObject();

    render_system->Init();
    int frame = 0;
    float dt = 0.0f, runTime = 0.f, frameTime = 0.f;
    while (!glfwWindowShouldClose(window))
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        frame += 1;
        if (frameTime > 1.f)
        {
            std::cout << "fps:" << frame << std::endl;
            frameTime -= 1.f;
            frame = 0;
        }
        Scene::defaultScene->Update();
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_POINTS);
        render_system->Update(dt);
        DrwaBuff(render_system->frame_buff);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        auto stopTime = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
        runTime += dt;
        frameTime += dt;
        mouse_handle(window);
    }

    glfwTerminate();
    return 0;
}

Vector3f curr_rotation;
Vector2f curr_pos;
Vector2f last_pos;
bool isRightButtion;

void mouse_handle(GLFWwindow* window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        auto model = GameObject::find("model");
        if (!isRightButtion)
        {
            last_pos = curr_pos;
            curr_rotation = model->transform->rotation;
            isRightButtion = true;
        }
        auto delta_pos = curr_pos - last_pos;
        // std::cout << "x:" << delta_pos.x << "y:" << delta_pos.y << std::endl;
        model->transform->rotation.y = curr_rotation.y + delta_pos.x;
        model->transform->rotation.x = curr_rotation.x + delta_pos.y;
    }
    else
    {
        isRightButtion = false;
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (action != GLFW_PRESS)
        return;
    if (key == GLFW_KEY_ESCAPE) //按下esc退出程序
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }
    switch (key)
    {
    case GLFW_KEY_S:
        render_system->isDrawShadow = !render_system->isDrawShadow;
        std::cout << "shadow:" << render_system->isDrawShadow << std::endl;
        break;
    case GLFW_KEY_L:
        render_system->isDrawLine = !render_system->isDrawLine;
        std::cout << "Line:" << render_system->isDrawLine << std::endl;
        break;
    case GLFW_KEY_Q:
        render_system->fillTriangleType = render_system->fillTriangleType == 0 ? 1 : 0;
        std::cout << "FillTriangleType:" << render_system->fillTriangleType << std::endl;
        break;
    case GLFW_KEY_R:
        auto& model = GameObject::find("model")->GetComponent<Model>();
        model.autoRotation = !model.autoRotation;
        std::string str = model.autoRotation ? "true" : "false";
        std::cout << "rotation:" << str << std::endl;
        break;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    curr_pos.x = (float)xpos;
    curr_pos.y = (float)ypos;
    // std::cout << "xpos:" << xpos << "   ypos:" << ypos << "  status:" << glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)
    //     << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->transform->position.z -= yoffset * 0.2;
}
