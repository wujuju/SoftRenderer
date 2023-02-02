#pragma once
#include "component.h"
#include "coordinator.h"
#include "transform.h"

class GameObject
{
public:
    GameObject();
    GameObject(std::string name);

    void Update()
    {
        for (auto const& element : components)
        {
            if (!element->isStart)
            {
                element->isStart = true;
                element->Start();
            }
            element->Update();
        }
    }

    template <typename T>
    T& GetComponent()
    {
        return gCoordinator->GetComponent<T>(entity);
    }

    template <typename T>
    T& AddComponent()
    {
        gCoordinator->AddComponent(entity, *(new T));
        auto& component = gCoordinator->GetComponent<T>(entity);
        static_cast<Component&>(component).Init(this);
        components.push_back(&component);
        return component;
    }

    static GameObject* find(std::string name);
    Entity entity;
    std::string name;
    std::vector<Component*> components;
    Transform* transform;
    static Coordinator* gCoordinator;
};
