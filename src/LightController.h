#pragma once
#include "Light.h"
class LightController {
protected:
	Light* m_SelectedLight;
public:
	LightController(Light* selectedLight);
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
	void OnImGuiRender() override;
	void OnImGuiRender(DirectionalLight* selectedLight);
	void Update(float deltaTime) override;
	void SelectedLight(Light* light) override;
};

class PointLightController : public LightController {
private:
	PointLight* m_SelectedLight;
public:
	PointLightController();
	PointLightController(PointLight* selectedLight);
	void OnImGuiRender() override;
	void OnImGuiRender(PointLight* selectedLight);
	void Update(float deltaTime) override;
	void SelectedLight(Light* light) override;
};