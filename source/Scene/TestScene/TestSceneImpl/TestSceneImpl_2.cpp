#include "TestSceneImpl_2.h"
#include "Scene/TestScene/TestSceneImpl/TestSceneActor/ColliderHolder.h"
#include "Utility/ImGui/ImGuiInclude.h"
#include "Input/DeviceInput.h"
#include <fstream>
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

TestSceneImpl_2::TestSceneImpl_2()
{
}

TestSceneImpl_2::~TestSceneImpl_2()
{
}

void TestSceneImpl_2::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);
	std::ifstream json_file("Source/Scene/TestScene/TestSceneImpl/TestSceneImpl_2.json");
	nlohmann::json j = nlohmann::json::parse(json_file);

	for (auto params_json : j.at("colliderHolders"))
	{
		auto params = initial_params_of_actor_t<ColliderHolder>();
		params.FromJsonObject(params_json);
		CreateActor<ColliderHolder>(&params);
	}
}

SceneType TestSceneImpl_2::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);

	if (DeviceInput::IsActive(KEY_INPUT_ESCAPE))
	{
		return SceneType::TEST_SELECT_SCENE;
	}

	if (ImGui::Begin("TestSceneImpl_2"))
	{
		ImGui::Text("This is English text");
		ImGui::Text(u8"これは日本語のテキストです");

		ImTextureID texture_id = nullptr;
		float u0 = 0.f, v0 = 0.f, u1 = 0.f, v1 = 0.f;
		MasterHelper::GetGameIconImguiImage(123, texture_id, u0, v0, u1, v1);

        const float font_size = ImGui::GetFontSize();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (font_size * 0.1f)); // Adjust the Y position
        ImGui::Image(texture_id, ImVec2(font_size, font_size), ImVec2(u0, v0), ImVec2(u1, v1));
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (font_size * 0.1f)); // Reset the Y position
        ImGui::Text("Icon and Text");
	}
	ImGui::End();


	if (ImGui::Begin("FocusCheck"))
	{
		static float f = 0;
		ImGui::InputFloat("InputFloat", &f, 0.f, 0.f, "%.3f");
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			printfDx(_T("Deactivated\n"));
		}
		if (ImGui::IsItemActivated())
		{
			printfDx(_T("Activated\n"));
		}

		ImGui::SameLine();
		ImGui::Text("IsItemFocused: %d", ImGui::IsItemFocused());

		static int x = 0;
		ImGui::InputInt("InputInt", &x);
		ImGui::SameLine();
		ImGui::Text("IsItemFocused: %d", ImGui::IsItemFocused());

		static char str[256] = "";
		ImGui::InputText("InputText", str, sizeof(str));
		ImGui::SameLine();
		ImGui::Text("IsItemFocused: %d", ImGui::IsItemFocused());

		bool b = false;
		ImGui::Checkbox("Checkbox", &b);
		ImGui::SameLine();
		ImGui::Text("IsItemFocused: %d", ImGui::IsItemFocused());

		ImGui::Button("Button", ImVec2(32, 32));
		ImGui::SameLine();
		ImGui::Text("IsItemFocused: %d", ImGui::IsItemFocused());

		int arr[3] = { 0, 0, 0 };
		ImGui::InputInt3("InputInt3", arr);
		ImGui::SameLine();

		int flag = 0;
		flag |= ImGui::IsItemActivated() ? 1 : 0;
		flag |= ImGui::IsItemDeactivated() ? 2 : 0;
		flag |= ImGui::IsItemDeactivatedAfterEdit() ? 4 : 0;

		if (flag != 0)
		{
			printfDx(_T("flag: %d\n"), flag);
		}
		
		ImGui::Text("Focused: %d,  Edited: %d", ImGui::IsItemFocused(), ImGui::IsItemEdited());
	}
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(200, 100));
	if (ImGui::BeginPopupModal("Popup"))
	{
		ImGui::Text("Popup Content");
		ImGui::EndPopup();
	}
	if (DeviceInput::IsPressed(KEY_INPUT_SPACE))
	{
		ImGui::OpenPopup("Popup");
	}

	return GetSceneType();
}
