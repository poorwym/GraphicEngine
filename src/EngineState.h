#pragma once

class EngineState
{
public:
    EngineState();
    float RenderingProgress;
    bool isRunning;
    bool needUpdate;
    bool MaterialUpdate;
    bool symbolVisible;
};