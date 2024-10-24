# Model层 #Graphic

好的，`Model` 层在三维图形引擎中负责存储和管理所有的场景数据、几何信息、材质、物理状态等逻辑，通常包括对象的实体模型、场景管理、物理模拟和资源加载等部分。下面是一个详细的 `Model` 层架构设计，帮助你更好地组织数据和逻辑。

### Model 层详细架构

`Model` 层主要由以下几个核心模块组成：

1. **场景管理（Scene Management）**
2. **实体系统（Entity System）**
3. **组件系统（Component System）**
4. **物理系统（Physics System）**
5. **资源管理（Resource Management）**
6. **动画系统（Animation System）**
7. **层级系统与场景图（Hierarchy & Scene Graph）**

### 1. **场景管理（Scene Management）**

场景管理模块负责维护当前加载的场景及其内容（如物体、光源、摄像机等），并将场景中的数据与渲染器连接。你可以将场景管理看作是管理游戏或引擎中的整个世界的入口。

#### 核心功能：
- **场景加载与保存**：能够从文件或资源中加载场景，并支持保存场景数据。
- **场景更新**：管理场景中所有对象的更新（如逻辑更新、物理更新等）。
- **场景切换**：支持在多个场景之间切换，或保存场景状态以供恢复。
- **场景遍历**：提供遍历场景中所有对象的功能，支持对不同对象执行操作（如更新、渲染等）。

#### 场景管理接口：
```cpp
class Scene {
public:
    void Load(const std::string& filePath);   // 加载场景
    void Save(const std::string& filePath);   // 保存场景
    void Update(float deltaTime);             // 更新场景
    void AddEntity(Entity* entity);           // 添加实体
    void RemoveEntity(Entity* entity);        // 删除实体
    const std::vector<Entity*>& GetEntities() const; // 获取场景中的实体列表
};
```

### 2. **实体系统（Entity System）**

实体系统是 `Model` 层的核心，它定义了场景中的所有物体。这些物体可以是静态几何体、动态物体、摄像机、光源等。每个实体通常由多个组件（Component）组成，以便实现其特定的功能。

#### 核心功能：
- **唯一标识**：每个实体都拥有一个唯一的 ID，以便进行管理和查询。
- **组件化设计**：实体通过不同的组件实现各种功能（如变换、渲染、物理、光照等）。
- **实体生命周期**：管理实体的创建、更新、销毁等生命周期操作。

#### 实体系统接口：
```cpp
class Entity {
public:
    Entity();
    ~Entity();
    
    unsigned int GetID() const;                   // 获取实体的唯一ID
    void AddComponent(Component* component);      // 添加组件
    void RemoveComponent(Component* component);   // 移除组件
    template<typename T>
    T* GetComponent() const;                      // 获取指定类型的组件

    void Update(float deltaTime);                 // 更新实体
};
```

### 3. **组件系统（Component System）**

组件系统使得每个实体可以通过不同的组件来添加功能。通常情况下，每个组件负责一个特定的功能，如变换组件（用于管理位置、旋转和缩放）、渲染组件、物理组件等。

#### 常见的组件类型：
- **变换组件（Transform Component）**：管理物体的位移、旋转和缩放。每个实体都应有一个变换组件。
- **渲染组件（Render Component）**：管理物体的几何体数据、材质、纹理等，最终在 `View` 层渲染。
- **物理组件（Physics Component）**：管理物体的物理状态（如速度、碰撞体积等）。
- **灯光组件（Light Component）**：为实体添加光源属性，如点光源、聚光灯等。

#### 组件接口：
```cpp
class Component {
public:
    virtual ~Component() = default;
    virtual void Update(float deltaTime) = 0;     // 更新组件
};

class TransformComponent : public Component {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    TransformComponent();
    void Update(float deltaTime) override;
};

class RenderComponent : public Component {
public:
    Mesh* mesh;
    Material* material;

    RenderComponent(Mesh* mesh, Material* material);
    void Update(float deltaTime) override;
};
```

### 4. **物理系统（Physics System）**

物理系统负责模拟物体的运动、碰撞检测、重力等。你可以选择集成第三方物理引擎（如 Bullet、PhysX）来处理复杂的物理交互。

#### 核心功能：
- **刚体（RigidBody）模拟**：为物体添加刚体属性，模拟物体的动态行为。
- **碰撞检测**：检测物体之间的碰撞，并处理相应的响应。
- **力和扭矩**：应用力和扭矩来影响物体的运动。

#### 物理系统接口：
```cpp
class PhysicsSystem {
public:
    void AddRigidBody(RigidBody* body);           // 添加刚体
    void RemoveRigidBody(RigidBody* body);        // 移除刚体
    void Simulate(float deltaTime);               // 更新物理模拟

    void SetGravity(const glm::vec3& gravity);    // 设置重力
};

class RigidBody {
public:
    glm::vec3 velocity;
    glm::vec3 angularVelocity;

    void ApplyForce(const glm::vec3& force);      // 应用力
    void ApplyTorque(const glm::vec3& torque);    // 应用扭矩
};
```

### 5. **资源管理（Resource Management）**

资源管理系统负责加载和管理图形引擎中的各种资源，如模型、纹理、材质、着色器等。通过统一的资源管理，避免重复加载相同的资源，并有效地管理内存。

#### 核心功能：
- **资源加载与缓存**：支持从文件加载资源，并将其缓存以供重复使用。
- **资源引用计数**：通过引用计数管理资源的生命周期，确保在资源不再使用时释放内存。
- **异步加载**：在后台异步加载资源，提高引擎的加载速度。

#### 资源管理接口：
```cpp
class ResourceManager {
public:
    template<typename T>
    T* Load(const std::string& filePath);         // 从文件加载资源

    template<typename T>
    void Unload(T* resource);                     // 卸载资源
};

class Mesh {
    // 网格数据
};

class Texture {
    // 纹理数据
};
```

### 6. **动画系统（Animation System）**

动画系统负责处理物体的骨骼动画、关键帧动画或其他变换动画。它可以与物理系统或渲染系统相结合，实现复杂的动态效果。

#### 核心功能：
- **骨骼动画**：通过骨骼蒙皮技术（skeletal animation）为角色或物体添加动画。
- **关键帧动画**：通过关键帧插值，生成物体的运动动画。
- **动画状态机**：通过状态机管理不同的动画状态与切换。

#### 动画系统接口：
```cpp
class AnimationSystem {
public:
    void Update(float deltaTime);                 // 更新动画系统
    void PlayAnimation(Entity* entity, const std::string& animationName);
};

class SkeletalAnimation {
public:
    void Apply(Entity* entity, float time);       // 将骨骼动画应用到实体上
};
```

### 7. **层级系统与场景图（Hierarchy & Scene Graph）**

场景图是一种树状结构，表示场景中物体的层级关系。每个物体可以有一个父物体和多个子物体，子物体的变换会受到父物体的影响。这种结构对复杂场景的组织和管理非常有用。

#### 核心功能：
- **层级关系管理**：支持物体的父子关系，子物体随父物体一起移动、旋转或缩放。
- **局部坐标与全局坐标**：每个物体都有局部变换矩阵（相对于父物体），场景图系统会自动计算全局变换矩阵。

#### 场景图接口：
```cpp
class SceneNode {
public:
    Entity* entity;
    SceneNode* parent;
    std::vector<SceneNode*> children;

    glm::mat4 GetLocalTransform() const;          // 获取局部变换矩阵
    glm::mat4 GetGlobalTransform() const;         // 获取全局变换矩阵
    void AddChild(SceneNode* child);              // 添加子节点
    void RemoveChild(SceneNode*

 child);           // 移除子节点
};
```

---

### 结语

通过以上详细的 `Model` 层架构设计，你的三维图形引擎将拥有一个灵活、模块化的基础框架，便于数据管理和扩展。将实体与组件、场景图、物理、动画等模块解耦，能够极大地提升代码的可维护性和可扩展性。

这个设计可以与 `View` 层的渲染器和 `Controller` 层的输入系统紧密结合，形成一个功能完备的三维引擎架构。如果你在实现过程中有更多的需求或问题，欢迎继续讨论！