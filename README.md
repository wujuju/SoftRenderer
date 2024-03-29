架构参考了unity，用了组件模式

```
    auto gameObject = new GameObject("model");
    auto& model = gameObject->AddComponent<Model>();
    gameObject->AddComponent<MeshRenderComponent>();
```

熟悉unity的应该很快可以上手

渲染部分用了opengl，不过只用了画点api  

```
glBegin(GL_POINTS);
```

因为使用了glfw框架，实现画图就几行代码搞定

```
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
```

先来画个线框,这个比较简单，直接上代码

```
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
```

![img](https://pic1.zhimg.com/80/v2-a777607c4e3f37a68b6af9b1f34e9bbb_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

现在来填充三角形，填充三角形有2种方式

**Bresenham画线算法** 

![img](https://picx.zhimg.com/80/v2-e87b60bed10a84ea9c9b4c3febac39ab_720w.png?source=d16d100b)



编辑

添加图片注释，不超过 140 字（可选）

画一条只线需要2个点，先求出3个向量，AB,AC,CB

然后计算a点和b点,

扫码线设定为 i  从y1扫到y3  总高度设定为H(y3-y1)

上半部三角形,算出高度y1到y2,设定为h, 就是求   a点： AC**(i/H)  b点：AB\*(i/h)*

下半部三角形,算出高度y2到y3,设定为h2,就是求  a点：AC**(i/H)   b点：*CB**((i-h)/h2)*

然后用求面积公式，算出u,v,w差值

![img](https://pica.zhimg.com/80/v2-2a7f8f77afeb0102be65d207336d9218_720w.png?source=d16d100b)



编辑

添加图片注释，不超过 140 字（可选）

![img](https://picx.zhimg.com/80/v2-2219bfd47590c1e0ea3418fc83246302_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

```
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
            ...
```

[在线LaTeX公式编辑器-编辑器](https://www.latexlive.com/#JTVDYmVnaW4lN0JhbGlnbiolN0QlMEFQPXUqQSt2KkIrdypDJTBBJTVDJTVDJTBBdSt2K3clMjA9MSUwQSU1QyU1QyUwQXUlM0U9MCUyMCU1Q3F1YWQlMjB2JTNFPTAlMjAlNUNxdWFkJTIwdyUzRT0wJTBBJTVDJTVDJTBBUD11KkErdipCKygxLXUtdikqQyUwQSU1QyU1QyUwQVAtQz11KihBLUMpK3YoQi1DKSUwQSU1QyU1QyUwQSU1Q292ZXJyaWdodGFycm93JTdCQ1BfJTdCeCU3RCU3RD11KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0FfJTdCeCU3RCU3RCUyMCt2KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0JfJTdCeCU3RCU3RCUwQSU1QyU1QyUwQSU1Q292ZXJyaWdodGFycm93JTdCQ1BfJTdCeSU3RCU3RD11KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0FfJTdCeSU3RCU3RCUyMCt2KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0JfJTdCeSU3RCU3RCUwQSU1QyU1QyUwQXU9JTVDZnJhYyU3QiU1Q292ZXJyaWdodGFycm93JTdCQ1AlN0QlNUN0aW1lcyUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0IlN0QlN0QlN0IlNUNvdmVycmlnaHRhcnJvdyU3QkNBJTdEJTVDdGltZXMlNUNvdmVycmlnaHRhcnJvdyU3QkNCJTdEJTdEJTIwJTBBJTVDJTVDJTBBdj0lNUNmcmFjJTdCJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQSU3RCU1Q3RpbWVzJTVDb3ZlcnJpZ2h0YXJyb3clN0JDUCU3RCU3RCU3QiU1Q292ZXJyaWdodGFycm93JTdCQ0ElN0QlNUN0aW1lcyU1Q292ZXJyaWdodGFycm93JTdCQ0IlN0QlN0QlMjAlMEElNUMlNUMlMEF3PTEtdS12JTBBJTVDZW5kJTdCYWxpZ24qJTdE)

**重心坐标画点**

![img](https://pic1.zhimg.com/80/v2-305785f874ae388bcdae143c421bab00_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

先算出三角形的包围盒，然后判断包围盒内的所有点，是否在三角形内，在就渲染

我们用前面的公式，优化下算法，可以看出，[CA,CB,PC] 新组成的向量和 [u,v,1] 垂直

所以可以用 向量[CAx,CBx,PCx] 叉乘 向量[CAy,CBy,PCy] 得出 [u,v,1]

一个叉乘就可以得出u,v,w

![img](https://pic1.zhimg.com/80/v2-3a901aa1349812938280f16ef6d49eab_720w.jpeg?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

```
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
            ...
```

[在线LaTeX公式编辑器-编辑器](https://www.latexlive.com/#JTVDYmVnaW4lN0JhbGlnbiolN0QlMEElNUNvdmVycmlnaHRhcnJvdyU3QkNQXyU3QnglN0QlN0Q9dSolMjAlNUNvdmVycmlnaHRhcnJvdyU3QkNBXyU3QnglN0QlN0QlMjArdiolMjAlNUNvdmVycmlnaHRhcnJvdyU3QkNCXyU3QnglN0QlN0QlMEElNUMlNUMlMEElNUNvdmVycmlnaHRhcnJvdyU3QkNQXyU3QnklN0QlN0Q9dSolMjAlNUNvdmVycmlnaHRhcnJvdyU3QkNBXyU3QnklN0QlN0QlMjArdiolMjAlNUNvdmVycmlnaHRhcnJvdyU3QkNCXyU3QnklN0QlN0QlMEElNUMlNUMlMEElRTglQkQlQUMlRTYlOEQlQTIlRTYlODglOTAlMEElNUMlNUMlMEF1KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0FfJTdCeCU3RCU3RCUyMCt2KiUyMCU1Q292ZXJyaWdodGFycm93JTdCQ0JfJTdCeCU3RCU3RCslNUNvdmVycmlnaHRhcnJvdyU3QlBDXyU3QnglN0QlN0Q9MCUwQSU1QyU1QyUwQXUqJTIwJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQV8lN0J5JTdEJTdEJTIwK3YqJTIwJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQl8lN0J5JTdEJTdEKyU1Q292ZXJyaWdodGFycm93JTdCUENfJTdCeSU3RCU3RD0wJTBBJTVDJTVDJTBBJUU4JUJEJUFDJUU2JThEJUEyJUU2JTg4JTkwJTBBJTVDJTVDJTBBJTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTIwdSUyMCYlMjB2JTIwJjElMEElNUNlbmQlN0JibWF0cml4JTdEJTBBJTBBJTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQV8lN0J4JTdEJTdEJTBBJTIwJTVDJTVDJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQl8lN0J4JTdEJTdEJTBBJTIwJTVDJTVDJTVDb3ZlcnJpZ2h0YXJyb3clN0JQQ18lN0J4JTdEJTdEJTBBJTVDZW5kJTdCYm1hdHJpeCU3RCUwQT0wJTBBJTVDJTVDJTBBJTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTIwdSUyMCYlMjB2JTIwJjElMEElNUNlbmQlN0JibWF0cml4JTdEJTBBJTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQV8lN0J5JTdEJTdEJTBBJTIwJTVDJTVDJTVDb3ZlcnJpZ2h0YXJyb3clN0JDQl8lN0J5JTdEJTdEJTBBJTIwJTVDJTVDJTVDb3ZlcnJpZ2h0YXJyb3clN0JQQ18lN0J5JTdEJTdEJTBBJTVDZW5kJTdCYm1hdHJpeCU3RCUwQT0wJTBBJTVDZW5kJTdCYWxpZ24qJTdE)

然后我们加上光照和zbuffer

![img](https://picx.zhimg.com/80/v2-1fcf95f9bd37a583cca71fffb7cc75d0_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

## 然后我们上透视

![img](https://picx.zhimg.com/80/v2-9ef31dc2df5df553bb458ba0ec40e6ec_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

透视投影就是把P点，投射到近平面上，p2的xy和容易求出来，根据相似三角形定理

p2  **x`=-nx/z   y`=-ny/z  z`=n**

z`我们不能直接等于n,我们计算zbuffer和插值的时候还需要用到,我们需要把z变换到同样的空间来计算

因为仿射变换，我们在二维空间中可以把z`写成**z`=az+b**

因为x和y是线性关系，和z不是，和1/z是线性关系，所有**z`=-(az+b)/z**

![img](https://picx.zhimg.com/80/v2-6fa6de132cc32e67d550abbb4e782067_720w.png?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

[在线LaTeX公式编辑器-编辑器](https://www.latexlive.com/#JTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTIwbiUyMCYlMjAwJTIwJiUyMDAlMjAmMCUyMCU1QyU1QyUwQSUyMDAlMjAmJTIwbiUyMCYwJTIwJTIwJjAlMjAlNUMlNUMlMEElMjAwJTIwJiUyMDAlMjAmJTIwYSUyMCYlMjBiJTIwJTVDJTVDJTBBJTIwMCUyMCYlMjAwJTIwJiUyMC0xJTIwJjAlMEElNUNlbmQlN0JibWF0cml4JTdEJTBBJTVDYmVnaW4lN0JibWF0cml4JTdEJTBBeCUwQSUyMCU1QyU1Q3klMEElMjAlNUMlNUN6JTBBJTVDJTVDMSUwQSU1Q2VuZCU3QmJtYXRyaXglN0QlMEE9JTBBJTVDYmVnaW4lN0JCbWF0cml4JTdEJTBBbnglMEElMjAlNUMlNUNueSUwQSUyMCU1QyU1Q2F6K2IlMEElNUMlNUMlMEEteiUwQSU1Q2VuZCU3QkJtYXRyaXglN0QlMEElMEElNUNMb25ncmlnaHRhcnJvdyUyMCUwQSU1Q2JlZ2luJTdCQm1hdHJpeCU3RCUwQSU1Q2ZyYWMlN0ItbnglN0QlN0J6JTdEJTIwJTBBJTIwJTVDJTVDJTVDZnJhYyU3Qi1ueSU3RCU3QnolN0QlMjAlMEElMjAlNUMlNUMlNUNmcmFjJTdCLShheitiKSU3RCU3QnolN0QlMjAlMEElMjAlNUMlNUMxJTBBJTBBJTVDZW5kJTdCQm1hdHJpeCU3RA==)

然后经过ccv之后，**n=-1,f=1,z=n和z=f**，求a , b

**-(az+b)/z=1  => -(af+b)/f=1** 

**-(az+b)/z=-1=> -(an+b)/n=-1**

解二次方程

**a=-(f+n)/(f-n)  b=2nf/(f-n)**

然后把x,y也压缩到[-1,1]之间，已知r,l,b,t(屏幕上下左右),和fov(角度),aspect(宽高的比值)

**r=n\*tan(fov/2)\*aspect             l=-n\*tan(fov/2)\*aspect**

**t=n\*tan(fov/2)                         b=-n\*tan(fov/2)**

**x`=n\*(2/(r-l))    y`=n\*(2/(t-b))**

把r,l,b,t代入

**x`=cot(fov/2)/aspect               y`=cot(fov/2)**

代入上面的矩阵

![img](https://pic1.zhimg.com/80/v2-76aad32f844ad770eaaa042fe4a4e2e1_720w.jpeg?source=d16d100b)



编辑切换为居中

添加图片注释，不超过 140 字（可选）

[在线LaTeX公式编辑器-编辑器](https://www.latexlive.com/#JTVDYmVnaW4lN0JibWF0cml4JTdEJTBBJTIwJTVDZnJhYyU3QmNvdChmb3YvMiklN0QlN0Jhc3BlY3QlN0QlMjAmJTIwMCUyMCYlMjAwJTIwJjAlMjAlNUMlNUMlMEElMjAwJTIwJiUyMGNvdChmb3YvMiklMjAmMCUyMCUyMCYwJTIwJTVDJTVDJTBBJTIwMCUyMCYlMjAwJTIwJiUyMCU1Q2ZyYWMlN0ItKGYrbiklN0QlN0JmLW4lN0QlMjAlMjAmJTIwJTVDZnJhYyU3QjJuZiklN0QlN0JmLW4lN0QlMjAlNUMlNUMlMEElMjAwJTIwJiUyMDAlMjAmJTIwLTElMjAmMCUwQSU1Q2VuZCU3QmJtYXRyaXglN0QlMEE=)

再加上光照

![img](https://picx.zhimg.com/80/v2-2fe51b5f95d52499a95ccd25e9c86269_720w.png?source=d16d100b)



编辑切换为居中

完整的Blinn Pong模型

源码地址：

参考资料：
