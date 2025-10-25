#include "../include/hackrender.h"

HackRender hackRender;

void HackRender::render()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	ImGui::SetNextWindowSize(ImVec2(600, 400));
	ImGui::Begin("##Window", 0, windowFlags);
	{
		ImGui::Text("Hello World");
	}
	ImGui::End();
}
