#pragma once
#include "Light.h"
class LightController {
protected:
	Light* m_SelectedLight;
public:
	LightController(Light* selectedLight);
	virtual const char* GetImGuiTitle() { return nullptr; };
	virtual void OnImGuiRender() {};
	virtual void Update(float deltaTime) {};
	virtual void SelectedLight(Light* light) {};
};

class DirectionalLightController : public LightController {	
private:
	DirectionalLight* m_SelectedLight;
public:
	DirectionalLightController();
	DirectionalLightController(DirectionalLight* selectedLight);
	const char* GetImGuiTitle() override;
	void OnImGuiRender() override;
	void OnImGuiRender(DirectionalLight* selectedLight);
	void Update(float deltaTime) override;
	void SelectedLight(Light* light) override;
};