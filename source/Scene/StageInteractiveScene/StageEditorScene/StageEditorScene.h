#pragma once
#include "Scene/StageInteractiveScene/StageInteractiveScene.h"
#include "Scene/SceneState/SceneState.h"
#include "Utility/ImGui/ImGuiInclude.h"
#include "EditorSceneInitialParams.h"
#include "EditorCommand/EditorCommands.h"
#include "Actor/EntityType.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <stack>

struct StageEditorConfig;
struct SpawnActorInfo;
class Stage;
class EditorMessageManager;
class ParamEditGroup;

class StageEditorScene : public StageInteractiveScene
{
	friend class StageEditorSceneState;
	friend class StageEditorSceneState_Edit;
	friend class StageEditorSceneState_Edit_Summon;
	friend class StageEditorSceneState_Edit_Modify;
	friend class StageEditorSceneState_Edit_Move;
	friend class StageEditorSceneState_Paused;
	friend class StageEditorSceneState_ResizeStage;
	friend class StageEditorSceneState_ThumbnailShooting;
	friend class StageEditorSceneState_StageSettings;
	friend class StageEditorSceneState_EditorSettings;
	friend class StageEditorSceneState_EditStageBg;
	friend class StageEditorSceneState_EditStageBGM;

	friend class stage_editor_scene::CmdManipulateActorWaitingRoom;
	friend class stage_editor_scene::CmdSummonActor;
	friend class stage_editor_scene::CmdRemoveActors;
	friend class stage_editor_scene::CmdChangeActorPosition;
	friend class stage_editor_scene::CmdChangeStageBackground;
	friend class stage_editor_scene::CmdChangeStageBGM;
	friend class stage_editor_scene::CmdChangeStageLength;

public:
	StageEditorScene();
	virtual ~StageEditorScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_time) override;
	virtual void Draw() override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::EDITOR_SCENE; }
	// virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const override;
	virtual void UpdateCameraParams(const float delta_seconds) override;

private:
	virtual bool ShouldSpawnSceneAnimRendererActor() const override
	{
		return false;
	}
	//~ End SceneBase interface

private:
	void LoadStageEditorConfig(const StageEditorConfig& config);

	Actor* GetActorAt(const Vector2D& viewport_position) const;
	void DrawActorConvex(Actor* actor, int color = 0xFFFFFF, int alpha = 128);

	/// <summary>
	/// マウスがステージ上のオブジェクトを選択できる状態かどうか
	/// </summary>
	bool IsMouseOnStage(const Vector2D& mouse_pos) const;

	bool _is_end_scene_requested = false;
	SceneType destination_scene;
	void EndScene();

	// サイドバー
	struct SidebarInfo
	{
		SidebarInfo()
			: is_first_loop(true)
			, common_button_size(32)
		{}

		/// <summary>
		/// SidebarInfo
		/// </summary>
		void OnLoopEnd()
		{
			is_first_loop = false;
		}

		// 最初のループでレイアウト調整のためUIサイズを計算する
		bool is_first_loop;

		ImGui::Texture img_editmode_add;
		ImGui::Texture img_editmode_modify;
		ImGui::Texture img_editmode_move;
		ImGui::Texture img_editmode_remove;
		ImGui::Texture img_undo;
		ImGui::Texture img_redo;
		ImGui::Texture img_editor_settings;
		ImGui::Texture img_enter_thumbnail_shooting;
		ImGui::Texture img_stage_settings;
		ImGui::Texture img_enter_change_stage_length;
		ImGui::Texture img_open_modal_background;
		ImGui::Texture img_open_modal_bgm;
		ImGui::Texture img_open_leftbar;
		ImGui::Texture img_close_leftbar;

		int common_button_size;

		struct SidebarState {
			SidebarState()
				: is_open(true)
				, current_width(0)
			{}
			bool is_open;
			int current_width;
			ImGui::SideBarDesc desc;
		};
		SidebarState left;
		SidebarState right;
	};
	std::unique_ptr<SidebarInfo> sidebar_info;
	void MakeSidebarInfo();
	void ToggleSidebarOpenClosed(SidebarInfo::SidebarState& sidebar_state);
	void OpenSidebar(SidebarInfo::SidebarState& sidebar_state);

	// エディターメッセージ
	std::unique_ptr<EditorMessageManager> _editor_message_manager;

	/// <summary>
	/// エディターメッセージを表示する
	/// </summary>
	void PushEditorMessage(const std::string& message, const SoundInstance* const sound = nullptr);

	// 操作説明
	bool _should_show_controls;
	bool _is_control_window_expanded;
	bool BeginControlsImGuiWindow();
	void EndControlsImGuiWindow();

	// コマンド履歴
	std::shared_ptr<CommandHistory> _command_history;
	void OnNewCommandPushed(const std::shared_ptr<CommandBase>& new_command);

	// シーン, ステージから除外されたアクターを格納しておく場所
	std::vector<Actor*> _actor_waiting_room;
	void AddActorToWaitingRoom(Actor* const actor);

	// エンティティ生成情報
	struct SummonEntityInfo
	{
		MasterDataID entity_id;		// is_item_actorがtrueの場合は無視される
		nlohmann::json initial_params_json;	// 召喚するエンティティの初期化パラメータ
		bool is_item_actor;			// 召喚対象がItemActorの場合はtrueに設定される

		SummonEntityInfo()
			: entity_id(0)
			, is_item_actor(false)
		{
		}

		~SummonEntityInfo()
		{}
	};
	std::unordered_map<EEntityCategory, std::vector<std::shared_ptr<SummonEntityInfo>>> _summon_infos;
	void LoadSummonInfos();

	// 状態スタック
	// トップが現在の状態
	std::shared_ptr<SceneStateStack<StageEditorScene>> _state_stack;

	// 未保存の内容があることを示すフラグ
	bool _is_unsaved;
	bool IsUnsaved() const { return _is_unsaved; }
	void MarkAsUnsaved() { _is_unsaved = true; }
	void MarkAsSaved() { _is_unsaved = false; }
	void SaveStage();

	// サウンド
	struct EditorSceneSounds
	{
		std::shared_ptr<SoundInstance> bgm;
		std::shared_ptr<SoundInstance> se_place_actor;
		std::shared_ptr<SoundInstance> se_camera_shutter;
		std::shared_ptr<SoundInstance> se_warning;
		std::shared_ptr<SoundInstance> se_undo;
		std::shared_ptr<SoundInstance> se_redo;
		std::shared_ptr<SoundInstance> se_remove_actor;
		// TODO: SE追加手順
		// 1. メンバ変数を追加
		// 2. GetSEList()の戻り値に追加
		// 3. LoadEditorSceneSounds()でロード

		std::vector<std::shared_ptr<SoundInstance>> GetSEList() 
		{
			return { se_place_actor, se_camera_shutter, se_warning, se_undo, se_redo, se_remove_actor };
		}
	};
	std::unique_ptr<EditorSceneSounds> _editor_scene_sounds;
	void LoadEditorSceneSounds();
	void SetSoundsVolume(const StageEditorConfig& config);
};


template<>
struct SceneBase::traits<StageEditorScene>
{
	typedef StageInteractiveSceneInitialParams initial_params_type;
};
