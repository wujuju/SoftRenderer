#pragma once
class GameObject;
class Component
{
public:
    bool isStart;
    GameObject* gameObject;

    void Init(GameObject* obj)
    {
        gameObject = obj;
        Awake();
    }

    virtual void Awake()
    {
    }

    virtual void Start()
    {
    }

    virtual void Update()
    {
    }
};
