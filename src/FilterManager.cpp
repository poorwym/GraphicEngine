#include "FilterManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void FilterManager::OnImGuiRender()
{
	 ImGui::SliderFloat("Contrast", &m_Filter.m_Alpha, 0.5f, 1.5f);
     ImGui::SliderFloat("Brightness", &m_Filter.m_Beta, -100.0f, 100.0f);
     ImGui::SliderFloat("Hue", &m_Filter.m_Hue, 0.0f, 360.0f);
     ImGui::SliderFloat("Saturation", &m_Filter.m_Saturation, 0.0f, 2.0f);
     ImGui::Checkbox("Invert", &m_Filter.m_ApplyInvert);
     ImGui::Checkbox("Grayscale", &m_Filter.m_ApplyGrayscale);
     ImGui::Checkbox("Sepia", &m_Filter.m_ApplySepia);
     ImGui::Checkbox("Gaussian Blur", &m_Filter.m_ApplyGaussianBlur);
     ImGui::Checkbox("Sharpen", &m_Filter.m_ApplySharpen);
}
