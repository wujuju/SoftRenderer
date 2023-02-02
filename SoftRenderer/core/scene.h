#pragma once
#include "gameobject.h"

class Scene
{
public:
    void Update()
    {
        for (auto element : gameObjects)
        {
            element->Update();
        }
    }

    static void Add(GameObject* obj)
    {
        defaultScene->gameObjects.push_back(obj);
    }

    std::vector<GameObject*> gameObjects;
    static Scene* defaultScene;
};
