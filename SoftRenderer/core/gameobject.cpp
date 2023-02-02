#include "gameobject.h"
#include "scene.h"
Coordinator* GameObject::gCoordinator;

GameObject::GameObject()
{
    entity = gCoordinator->CreateEntity();
    transform = &AddComponent<Transform>();
    Scene::Add(this);
}

GameObject::GameObject(std::string name)
{
    this->name = name;
    entity = gCoordinator->CreateEntity();
    transform = &AddComponent<Transform>();
    Scene::Add(this);
}

GameObject* GameObject::find(std::string name)
{
    for (auto const& game_object : Scene::defaultScene->gameObjects)
    {
        if (game_object->name == name)
            return game_object;
    }
    return nullptr;
}
