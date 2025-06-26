#pragma once
#include <imgui.h>

namespace UIComponents
{
	bool VectorPicker( float& x, float& y, float& z, float& w, const char* format = "%.3f", ImGuiSliderFlags flags = 0 )
	{
		bool isDirty = false;

		isDirty = ImGui::InputFloat( "x", &x, -1.0f, 1.0f, format, flags );
		ImGui::SameLine();
		isDirty = ImGui::InputFloat( "y", &y, -1.0f, 1.0f, format, flags );
		ImGui::SameLine();
		isDirty = ImGui::InputFloat( "z", &z, -1.0f, 1.0f, format, flags );
		ImGui::SameLine();
		isDirty = ImGui::InputFloat( "w", &w, -1.0f, 1.0f, format, flags );

		return isDirty;
	}
}