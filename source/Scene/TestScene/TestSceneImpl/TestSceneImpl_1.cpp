#include "TestSceneImpl_1.h"

#include "Input/DeviceInput.h";
#include <imgui.h>
#include "Utility/ImGui/ImGuiInclude.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/IEditableParameter.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "Component/EmitterComponent.h"
#include "Actor/AllActorsInclude_generated.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

static nlohmann::json particle_json;
constexpr MasterDataID PARTICLE_ID = 1;

TestSceneImpl_1::TestSceneImpl_1()
	: emitter_component(nullptr)
	, test_actor(nullptr)
{
}

TestSceneImpl_1::~TestSceneImpl_1()
{
}

void TestSceneImpl_1::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);
	ImGui::SetNextWindowSize(ImVec2(WINDOW_SIZE_X / 3, WINDOW_SIZE_Y / 3));

	const std::string PARTICLES_DIR = "resources/particles/";
	std::ifstream json_file(PARTICLES_DIR + MdParticle::Get(PARTICLE_ID).json_name);
	particle_json = nlohmann::json::parse(json_file);

	initial_params_of_actor_t<Actor> params;
	params.transform.position = Vector2D(320, 320);
	params.transform.rotation = 0.f;
	test_actor = CreateActor<Actor>(&params);
	emitter_component = test_actor->CreateComponent<EmitterComponent>(test_actor);
	emitter_component->SetLocalPosition(Vector2D(128, 0));
	emitter_component->SetLocalRotation(0);

	EmitterComponentParams emitter_params = {};
	emitter_params.spawn_rate = 2500.f;

	ParticleSpawnDesc spawn_desc = {};
	spawn_desc.FromJsonObject(particle_json);

	emitter_params.spawn_desc = spawn_desc;
	emitter_component->SetEmitterParams(emitter_params);
}

SceneType TestSceneImpl_1::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);

	test_actor->AddActorRotation(DX_PI_F * delta_seconds);
	//emitter_component->AddRotation(-DX_PI_F * delta_seconds);


	ImGui::Begin("TestInputInt");
	static int x = 0;
	ImGui::InputInt("testInputIntBody", &x);
	ImGui::End();

	constexpr int PopStyleColorCount = 2;
	ImGui::PushStyleColorU32_rgba(ImGuiCol_ChildBg, 0xFFCFDF'FF);
	ImGui::PushStyleColorU32_rgba(ImGuiCol_Text, 0x212121'FF);
	ImGui::PushStyleColors_Button(ImGui::ColorConvertU32ToFloat4(ImGui::SwizzleImU32_rgba_abgr(0xFEFDCA'FF)));
	static const auto style = std::make_shared<ParamEditGroupStyle>(0xFEFDCA'FF, 0.75, 0.5);

	ImGui::PopStyleColor(PopStyleColorCount);
	ImGui::PopStyleColors_Button();

	// アイコンギャラリー
	{
		ImGui::Text("IsAnyWindowHovered? == %d", ImGui::IsAnyWindowHovered());

		static int icon_preview_page = 0;
		constexpr MasterDataID FIRST_VALID_ID = 1;
		constexpr int NUM_X = 8;
		constexpr int NUM_Y = 12;
		constexpr ImVec2 PREVIEW_SIZE = { 48,48 };

		{
			ImGui::SetNextWindowSize(ImVec2(700, 700));
			if (ImGui::Begin("IconPreview"))
			{
				for (int y = 0; y < NUM_Y; y++)
				{
					for (int x = 0; x < NUM_X; x++)
					{
						int i = y * NUM_X + x;
						try
						{
							MasterDataID id = static_cast<MasterDataID>(NUM_X * NUM_Y * icon_preview_page + i) + FIRST_VALID_ID;
							ImGui::Texture texture{};
							MasterHelper::GetGameIconImguiImage(id, texture.im_tex_id, texture.u0(), texture.v0(), texture.u1(), texture.v1());

							ImGui::Text("%d", static_cast<int>(id));
							ImGui::SameLine();

							std::string name = "icon" + std::to_string(static_cast<int>(id));
							ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_FramePadding, ImVec2(0, 0));
							if (ImGui::ImageButton("##", texture, PREVIEW_SIZE))
							{
								DxLib::printfDx(_T("selected icon: %d\n"), static_cast<int>(id));
							}
							ImGui::PopStyleVar();
							ImGui::SameLine();
						}
						catch (std::exception& e)
						{
							break;
						}
					}
					ImGui::NewLine();
				}
			}
			ImGui::End();


			if (!DeviceInput::WheelIsStopped())
			{

				DeviceInput::GetMouseWheelState();
				const int delta = DeviceInput::WheelIsDown() ? -1 : 1;
				int new_page = icon_preview_page + delta;
				new_page = clamp(new_page, 0, new_page);
				try
				{
					MdGameIcon::Get(static_cast<MasterDataID>(NUM_X * NUM_Y * new_page + FIRST_VALID_ID));
					icon_preview_page = new_page;
				}
				catch (std::exception& e)
				{

				}
			}
		}
	}

	// パーティクルテスト
	/*static float transform_rot = 0.f;
	static Vector2D transform_pos = Vector2D();
	transform_rot += delta_seconds * DX_PI_F * 2;
	transform_pos = Vector2D::ViewportToWorld(DeviceInput::GetMousePosition(), camera_params);
	ParticleSpawnDesc desc = {};
	desc.FromJsonObject(particle_json);
	desc.spawn_velocity.y = 5000;
	desc.world_transform.rotation = transform_rot;
	ParticleManager::GetInstance().Spawn(desc, 3);

	desc.world_transform.position = Vector2D();
	desc.world_transform.rotation = 0.f;
	ParticleManager::GetInstance().Spawn(desc);*/


	if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		return SceneType::TITLE_SCENE;
	}


	ImGui::Text("This is TestScene");

	return GetSceneType();
}

void TestSceneImpl_1::Draw()
{
	DxLib::DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0xFF444444, 1);
	__super::Draw();
}

void TestSceneImpl_1::UpdateCameraParams(const float delta_seconds)
{
	_camera_params.world_offset += DeviceInput::GetInputDir_WASD() * 128.f * delta_seconds / _camera_params.screen_scale;
	_camera_params.screen_scale += DeviceInput::GetMouseWheelMove() * 0.25f;
	_camera_params.screen_scale = clamp(_camera_params.screen_scale, 0.25f, 3.f);
	if (DeviceInput::IsPressed(MOUSE_INPUT_MIDDLE))
	{
		_camera_params.screen_scale = 1.f;
	}
}
