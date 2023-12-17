#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include <Walnut/Timer.h>

#include "../Renderer.h"
#include "../Camera.h"

#include "glm/gtc/type_ptr.hpp"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() :
		m_camera(45.0f, 0.1f, 100.0f) 
	{
		{
			Sphere sphere;
			sphere.Position = {0.0f, 0.0f, 0.0f};
			sphere.Radius = 0.5f;
			sphere.Albedo = { 0.0f, 1.0f, 1.0f };
			m_Scene.SphereList.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { -0.1f, 0.2f, -3.0f };
			sphere.Radius = 1.0f;
			sphere.Albedo = { 0.5f, 1.0f, 0.3f };
			m_Scene.SphereList.push_back(sphere);
		}
	}
	
	virtual void OnUpdate(float ts) override
	{
		m_camera.OnUpdate(ts);
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_lastRenderTime);

		if (ImGui::Button("Render")) {
			Render();
		}

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.SphereList.size(); i++) {
			Sphere& sphere = m_Scene.SphereList[i];

			std::string objectIdStr = std::to_string(i);
			
			std::string posStr = "Position Object " + objectIdStr;
			std::string radiusStr = "Radius Object " + objectIdStr;
			std::string albedoStr = "Albedo Object " + objectIdStr;

			ImGui::PushID(i);

			ImGui::DragFloat3(posStr.c_str(), glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat(radiusStr.c_str(), &sphere.Radius, 0.1f);
			ImGui::ColorEdit3(albedoStr.c_str(), glm::value_ptr(sphere.Albedo));
			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();

		ImGui::Begin("Logger");
		ImGui::Text("Logs: %s", m_LogString.c_str());
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(
				image->GetDescriptorSet(),
				{ (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}
	void Render() {
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_camera, m_LogString);

		m_lastRenderTime = timer.ElapsedMillis();
		
	}
private:
	Renderer m_Renderer;
	Scene m_Scene;
	Camera m_camera;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;
	std::string m_LogString = "";
	float m_lastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "M-Renderer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}