# View层 #Graphic

在三维图形引擎中，`View` 层的主要职责是将 `Model` 层中的数据转换为视觉效果，通过渲染器呈现到屏幕上。为了让 View 层架构更清晰，下面我提供一个更详细的 `View` 层架构设计，包括各个模块和它们之间的关系。

### `View` 层详细架构

`View` 层主要由以下几个核心模块组成：

1. **渲染器（Renderer）**
2. **摄像机系统（Camera System）**
3. **光照系统（Lighting System）**
4. **材质系统（Material System）**
5. **纹理系统（Texture System）**
6. **着色器管理（Shader Manager）**
7. **帧缓冲（Framebuffer）与后处理（Post-Processing）**
8. **可视化调试工具（Debugging Tools）**

### 1. **渲染器（Renderer）**

渲染器是 `View` 层的核心组件，负责整合场景中的各种渲染元素（如几何体、材质、纹理、光照等），并通过 OpenGL、Vulkan 等图形 API 将其渲染到屏幕上。

#### 渲染器核心模块：
- **Draw Call 管理**：渲染器负责管理所有的绘制调用（`draw call`），包括绑定 VAO（顶点数组对象）、EBO（索引缓冲对象）、VBO（顶点缓冲对象）以及其他状态设置。
- **渲染管线**：渲染管线负责处理从顶点输入到像素输出的整个过程。你可以设计一个抽象的渲染管线，并根据需要支持前向渲染（Forward Rendering）或延迟渲染（Deferred Rendering）。
  - **前向渲染**：直接对场景中所有物体进行渲染，适合较小的场景。
  - **延迟渲染**：先将所有几何信息渲染到多个缓冲区（G-Buffer），然后在片段着色阶段处理光照，适合复杂光照的场景。
- **批处理渲染**：减少状态切换和 `draw call` 的数量，通过将多个对象批量处理，提升渲染性能。

#### 渲染器接口：
```cpp
class Renderer {
public:
    void Init();                     // 初始化渲染器（创建VAO、VBO等）
    void BeginScene(const Camera&);   // 开始渲染场景（摄像机设置）
    void Submit(const Mesh&, const Material&); // 提交一个要渲染的物体
    void EndScene();                  // 结束场景，执行所有绘制命令
    void Render();                    // 触发实际的绘制
};
```

### 2. **摄像机系统（Camera System）**

摄像机系统负责处理视图矩阵（`View Matrix`）和投影矩阵（`Projection Matrix`），控制如何在三维空间中看到物体。常见的摄像机模式包括透视投影和正交投影。

#### 摄像机核心功能：
- **透视摄像机**：适合常见的三维场景。
- **正交摄像机**：适合特定需求（如二维场景、地图视图）。
- **摄像机控制**：提供摄像机的自由移动、旋转控制，通常通过用户输入（如鼠标、键盘）进行控制。
- **视锥体裁剪（Frustum Culling）**：通过摄像机的视锥体判断哪些物体在视野内，以提升性能。

#### 摄像机接口：
```cpp
class Camera {
public:
    Camera(float fov, float aspectRatio, float nearClip, float farClip);
    glm::mat4 GetViewMatrix() const;      // 获取视图矩阵
    glm::mat4 GetProjectionMatrix() const;// 获取投影矩阵
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void Update(float deltaTime);         // 根据输入更新摄像机
};
```

### 3. **光照系统（Lighting System）**

光照系统负责管理场景中的光源，并将其信息传递给渲染管线的着色器。常见的光源类型包括点光源、平行光和聚光灯等。

#### 光照系统核心功能：
- **光源类型管理**：支持不同类型的光源，每种光源有不同的光照计算方式。
- **阴影处理**：为光源生成阴影贴图（shadow mapping），提供逼真的光照效果。
- **多光源支持**：允许场景中同时存在多个光源，处理复杂的光照效果。

#### 光照接口：
```cpp
class Light {
public:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;

    // 不同光源类型可以继承此类
    virtual void Apply(Shader& shader) const = 0;
};

class PointLight : public Light {
public:
    float range;
    void Apply(Shader& shader) const override;  // 将光照数据传递给着色器
};
```

### 4. **材质系统（Material System）**

材质系统管理场景中物体的材质信息。每个物体的材质通常包含漫反射、镜面反射、法线贴图等信息。现代图形引擎通常采用 PBR（物理基础渲染）材质模型。

#### 材质系统核心功能：
- **材质属性**：管理不同材质的颜色、反射率、金属度、粗糙度等属性。
- **PBR 支持**：支持基于物理的渲染材质，提升渲染的真实感。
- **材质库**：提供材质资源的加载与管理功能，避免重复加载。

#### 材质接口：
```cpp
class Material {
public:
    glm::vec3 albedo;
    float metallic;
    float roughness;

    void Apply(Shader& shader) const; // 将材质属性传递给着色器
};
```

### 5. **纹理系统（Texture System）**

纹理系统负责加载和管理所有的纹理资源。纹理通常用于表面细节的展示，如颜色贴图、法线贴图、光照贴图等。

#### 纹理系统核心功能：
- **纹理加载**：支持从文件加载各种格式的纹理（如 PNG、JPG、DDS）。
- **纹理管理**：管理所有加载的纹理资源，避免重复加载相同的纹理。
- **纹理绑定**：将纹理绑定到着色器的不同纹理单元（Texture Unit）。

#### 纹理接口：
```cpp
class Texture {
public:
    unsigned int ID;
    int width, height, channels;

    bool LoadFromFile(const std::string& filepath);
    void Bind(unsigned int unit) const;  // 绑定到指定纹理单元
};
```

### 6. **着色器管理（Shader Manager）**

着色器管理负责加载、编译、链接和管理所有的着色器程序。它将顶点着色器、片段着色器等组合成一个完整的着色器程序，并为渲染器提供接口。

#### 着色器管理核心功能：
- **着色器加载**：从文件加载 GLSL 着色器源代码，编译并链接成着色器程序。
- **Uniform 管理**：提供统一的接口将数据传递到着色器的 `uniform` 变量中。
- **着色器切换**：支持在不同渲染阶段之间切换不同的着色器程序。

#### 着色器接口：
```cpp
class Shader {
public:
    unsigned int ID;
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;                   // 使用着色器程序
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;
};
```

### 7. **帧缓冲与后处理（Post-Processing）**

帧缓冲对象（Framebuffer Object, FBO）用于将渲染输出保存到纹理中，而不是直接渲染到屏幕。你可以使用 FBO 进行后处理操作，如添加模糊、色调映射、抗锯齿等效果。

#### 后处理核心功能：
- **多重渲染目标（MRT）**：支持将不同的渲染数据输出到多个纹理（例如延迟渲染中的 G-Buffer）。
- **屏幕后处理**：可以在渲染完成后应用屏幕空间的后处理效果，如 HDR、模糊、边缘检测等。

###

8. **可视化调试工具（Debugging Tools）**

调试工具可以帮助你在开发中快速检测问题，查看实时渲染状态和数据。

#### 调试工具核心功能：
- **FPS 统计**：显示当前的帧率和渲染时间。
- **对象可视化**：在屏幕上实时显示物体的包围盒（Bounding Box）或法线等调试信息。
- **渲染状态查看**：可以检查当前的着色器状态、纹理绑定等信息。

---

### 结语

通过以上详细的 `View` 层架构设计，你的三维图形引擎可以更加模块化和灵活。每个子模块（如渲染器、摄像机、材质、着色器等）各司其职，减少耦合性，使得引擎更具扩展性和维护性。这个架构可以让你轻松地扩展或替换某些模块（如切换渲染管线、添加后处理效果等），从而应对不同的应用场景。

如果你在实现过程中需要更多的细节或代码示例，随时可以向我询问！