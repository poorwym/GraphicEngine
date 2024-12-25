#pragma once

class LightManager
{
public:
	void OnImGuiRender();
	void AddDirectionalLight();
    void AddPointLight();
    void AddSpotLight();
};