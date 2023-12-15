#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include <Walnut/Timer.h>
#include "../Renderer.h"
#include "../Camera.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() :
		m_camera(45.0f, 0.1f, 100.0f) {}
	
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
		m_Renderer.Render(m_camera);

		m_lastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_camera;
	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;
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