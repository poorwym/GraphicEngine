# Controller层 #Graphic

`Controller` 层在三维图形引擎的 MVC 架构中负责处理用户输入（如键盘、鼠标、手柄等），并将输入转换为对 `Model` 层和 `View` 层的操作。`Controller` 层的核心任务是响应用户动作，影响摄像机、物体的变换以及游戏/引擎逻辑。

下面我将给出 `Controller` 层的详细架构设计，涵盖输入管理、摄像机控制、实体控制和事件系统等核心模块。

### Controller 层详细架构

`Controller` 层可以分为以下几个核心模块：

1. **输入管理（Input Management）**
2. **摄像机控制（Camera Controller）**
3. **实体控制（Entity Controller）**
4. **事件系统（Event System）**
5. **游戏逻辑与控制（Game Logic & Control）**

### 1. **输入管理（Input Management）**

输入管理系统是 `Controller` 层的核心模块，它负责接收并处理来自用户的输入设备（键盘、鼠标、游戏手柄等）并将其转换为可操作的事件或动作。

#### 核心功能：
- **键盘输入处理**：跟踪按键的按下和释放，支持多种按键状态（例如按住或短按）。
- **鼠标输入处理**：处理鼠标移动、滚轮、点击等操作。
- **手柄输入**：处理手柄的按键、摇杆等操作（可选）。
- **输入状态管理**：记录当前按下的按键、鼠标状态、手柄按钮等。

#### 输入管理接口：
```cpp
class InputManager {
public:
    static bool IsKeyPressed(int key);           // 判断某个键是否被按下
    static bool IsMouseButtonPressed(int button);// 判断某个鼠标按钮是否被按下
    static glm::vec2 GetMousePosition();         // 获取鼠标当前位置
    static float GetMouseScrollOffset();         // 获取鼠标滚轮偏移

    void Update();                               // 更新输入状态
    void RegisterKeyCallback(void (*callback)(int key, int action)); // 注册键盘回调
    void RegisterMouseCallback(void (*callback)(int button, int action)); // 注册鼠标回调
};
```

- **键盘输入处理**：`IsKeyPressed()` 可以用来检测某个键是否处于按下状态。
- **鼠标输入处理**：`GetMousePosition()` 返回当前鼠标位置，`GetMouseScrollOffset()` 用于滚轮输入。

### 2. **摄像机控制（Camera Controller）**

摄像机控制模块用于处理摄像机的移动、旋转等操作，通常基于用户的输入（如键盘、鼠标）。它能够实时调整摄像机的视角、位置和方向，影响 `View` 层的渲染视图。

#### 核心功能：
- **自由摄像机控制**：允许玩家自由移动摄像机（第一人称/第三人称视角）。
- **鼠标控制视角**：通过鼠标移动来控制摄像机的旋转。
- **键盘控制移动**：使用键盘（如 `WASD` 键）控制摄像机的平移。
- **缩放控制**：通过鼠标滚轮来调整摄像机的视距或 FOV（视角）。

#### 摄像机控制接口：
```cpp
class CameraController {
public:
    CameraController(Camera* camera);

    void Update(float deltaTime);               // 根据输入更新摄像机
    void ProcessKeyboardInput(float deltaTime); // 处理键盘输入控制摄像机
    void ProcessMouseInput(float xOffset, float yOffset); // 处理鼠标输入控制摄像机
    void ProcessMouseScroll(float yOffset);     // 处理鼠标滚轮缩放

private:
    Camera* camera;
    float movementSpeed;
    float mouseSensitivity;
    float zoomLevel;
};
```

- **键盘输入**：使用 `ProcessKeyboardInput()` 来响应移动指令（如前进、后退、左移、右移）。
- **鼠标输入**：通过 `ProcessMouseInput()` 响应鼠标移动，调整视角。
- **滚轮缩放**：`ProcessMouseScroll()` 用于调整摄像机的缩放或视野角度。

### 3. **实体控制（Entity Controller）**

实体控制模块允许用户直接操控场景中的实体对象（如角色、物体等），包括移动、旋转、缩放等操作。它结合输入管理系统，影响 `Model` 层的实体组件（如变换组件）。

#### 核心功能：
- **对象选中与交互**：允许用户通过鼠标点击选择场景中的实体对象。
- **实体变换控制**：处理实体的位移、旋转、缩放等操作。
- **输入映射**：将键盘或鼠标输入映射为实体的操作指令。

#### 实体控制接口：
```cpp
class EntityController {
public:
    EntityController(Entity* entity);

    void Update(float deltaTime);                 // 根据输入更新实体的状态
    void MoveEntity(const glm::vec3& direction);  // 移动物体
    void RotateEntity(const glm::vec3& axis, float angle); // 旋转物体
    void ScaleEntity(const glm::vec3& scale);     // 缩放物体

    void SelectEntity(Entity* entity);            // 选择一个实体
    void DeselectEntity();                        // 取消选择

private:
    Entity* controlledEntity;
};
```

- **实体移动**：通过 `MoveEntity()` 实现物体的平移。
- **实体旋转**：`RotateEntity()` 实现物体的旋转，控制其角度变化。
- **实体缩放**：`ScaleEntity()` 控制物体的缩放。

### 4. **事件系统（Event System）**

事件系统负责处理输入事件并分发到相应的组件。它可以采用观察者模式或发布-订阅模式，让不同模块能够响应输入事件。

#### 核心功能：
- **事件注册**：允许模块注册监听某些输入事件。
- **事件分发**：当发生某个事件时，系统将通知所有注册该事件的模块。
- **异步处理**：支持事件的异步处理，避免阻塞主循环。

#### 事件系统接口：
```cpp
enum class EventType {
    KeyPressed,
    KeyReleased,
    MouseMoved,
    MouseScrolled,
    EntitySelected
};

class Event {
public:
    EventType type;
    // 事件相关的额外数据（如按键、鼠标位置等）
};

class EventListener {
public:
    virtual void OnEvent(const Event& event) = 0;  // 处理事件
};

class EventSystem {
public:
    void RegisterListener(EventType type, EventListener* listener); // 注册事件监听器
    void DispatchEvent(const Event& event);                         // 分发事件
};
```

- **事件注册**：不同模块可以通过 `RegisterListener()` 注册监听特定类型的事件（如按键事件、鼠标事件等）。
- **事件分发**：通过 `DispatchEvent()`，当某个事件发生时，通知所有注册了该事件的监听器。

### 5. **游戏逻辑与控制（Game Logic & Control）**

游戏逻辑与控制模块负责管理引擎或游戏中的全局状态，如关卡逻辑、AI 控制、物体生成等。这部分逻辑通常与 `Model` 层的数据交互较多，也会通过 `Controller` 层输入管理用户行为。

#### 核心功能：
- **游戏状态管理**：管理游戏中的不同状态（如主菜单、游戏中、暂停等）。
- **关卡控制**：控制场景的加载、关卡切换。
- **AI 控制**：对 AI 角色进行控制，如路径规划、状态机等。

#### 游戏逻辑接口：
```cpp
class GameController {
public:
    GameController(Scene* scene);

    void Update(float deltaTime);                 // 更新游戏逻辑
    void LoadLevel(const std::string& levelName); // 加载关卡
    void RestartLevel();                          // 重新开始关卡
    void PauseGame();                             // 暂停游戏
    void ResumeGame();                            // 恢复游戏

private:
    Scene* currentScene;
    bool isPaused;
};
```

- **关卡管理**：通过 `LoadLevel()` 可以切换关卡，或通过 `RestartLevel()` 重置当前关卡。
- **游戏暂停**：支持暂停和恢复游戏。

---

### 结语

`Controller` 层的架构设计旨在处理用户输入并将其转化为对 `Model` 和 `View` 层的控制。这包括输入管理、摄像机和实体的控制、事件系统以及全局游戏逻辑。通过模块化的设计，`Controller` 层可以有效地分离输入处理与游戏逻辑，实现灵活的用户交互控制。

你可以根据实际的需求和功能扩展此架构，并与 `Model` 和 `View` 层紧密结合，打造一个高效的三维引擎。如果在实现过程中有具体问题或需求，欢迎