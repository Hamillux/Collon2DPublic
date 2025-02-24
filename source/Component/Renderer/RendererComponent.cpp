#include "RendererComponent.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"

AnimRendererComponent::AnimRendererComponent()
	: _is_playing(false)
	, _anim_override_time(0.f)
	, _blend_value(255)
{}

AnimRendererComponent::~AnimRendererComponent()
{
}

void AnimRendererComponent::Initialize()
{
	__super::Initialize();
	_is_playing = false;
	_anim_state = std::make_unique<AnimationState>();
}

void AnimRendererComponent::Tick(const float delta_seconds)
{
	__super::Tick(delta_seconds);

	if (!_is_playing)
	{
		return;
	}

	auto update_anim_state = [&delta_seconds](std::unique_ptr<AnimationState>& anim_state) 
		{
			const MdAnimation& md_animation = MdAnimation::Get(anim_state->anim_info.animation_id);
			const int max_loop = md_animation.max_loop;
			const int num_frames = md_animation.num_frames;
			const int offset = md_animation.loop_start_offset;
			const float frame_duration = md_animation.default_frame_duration;

			const bool is_frame_update_time
				= anim_state->time >= (anim_state->curernt_frame + 1) * frame_duration;

			const bool is_reached_last_frame =
				max_loop > 0 &&
				anim_state->curernt_frame >= offset + (num_frames - offset) * max_loop - 1;

			anim_state->time += delta_seconds * anim_state->anim_info.play_speed;

			if (is_frame_update_time && is_reached_last_frame)
			{
				anim_state.reset();
			}
			else if (is_frame_update_time)
			{
				anim_state->curernt_frame++;
			}
			
		};

	if (_anim_state != nullptr)
	{
		update_anim_state(_anim_state);
	}

	if (_override_anim_state != nullptr)
	{
		update_anim_state(_override_anim_state);

		if (!_override_anim_state)
		{
			_anim_override_time = 0.f;
		}

		if (_anim_override_time > 0.f && _override_anim_state->time >= _anim_override_time)
		{
			ClearOverrideAnimation();
		}
	}
}

void AnimRendererComponent::RendererDraw(const CameraParams& camera_params)
{
	if (!_is_playing)
	{
		return;
	}

	auto draw_impl = [this, &camera_params](std::unique_ptr<AnimationState>& target_anim_state)
		{
			const MdAnimation& md_animation = MdAnimation::Get(target_anim_state->anim_info.animation_id);
			const std::vector<int>& sprite_graph_handles = GraphicResourceManager::GetInstance().GetSprite(md_animation.sprite_id);
			int num_frames = md_animation.num_frames;
			int first_frame = md_animation.first_frame;
			int i_sprite = 0;
			if (target_anim_state->curernt_frame < num_frames)
			{
				i_sprite = first_frame + target_anim_state->curernt_frame;
			}
			else
			{
				const int offset = md_animation.loop_start_offset;
				i_sprite = first_frame + offset + (target_anim_state->curernt_frame - num_frames) % (num_frames - offset);
			}

			const int graph_handle = sprite_graph_handles.at(i_sprite);
			const Vector2D play_position = Vector2D::WorldToViewport(GetWorldPosition(), camera_params);
			const float play_rotation = GetWorldRotation();

			const int blend_mode = MasterHelper::GetAnimationBlendModeForDxLib(MdAnimation::Get(target_anim_state->anim_info.animation_id));

			DxLib::SetDrawBlendMode(blend_mode, _blend_value);

			DxLib::DrawRotaGraphF(
				play_position.x, play_position.y,
				target_anim_state->anim_info.ex_rate / camera_params.screen_scale * 1.25,	// 1.25 = ビューポートサイズ/スクリーンサイズ
				play_rotation,
				graph_handle,
				TRUE,
				target_anim_state->anim_info.reverse_x,
				target_anim_state->anim_info.reverse_y
			);

			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		};

	if (_override_anim_state != nullptr)
	{
		draw_impl(_override_anim_state);
	}
	else if (_anim_state != nullptr)
	{
		draw_impl(_anim_state);
	}
}

void AnimRendererComponent::SetAnimation(const AnimPlayInfo& anim_info, const bool should_play_immediately)
{
	_anim_state = std::make_unique<AnimationState>(anim_info);
	_is_playing = should_play_immediately;
}

InAnimateRenderer::InAnimateRenderer()
	: _handle(-1)
	, _icon_id(0)
	, _ex_rate(1.f)
{
}

InAnimateRenderer::~InAnimateRenderer()
{
}

void InAnimateRenderer::RendererDraw(const CameraParams& camera_params)
{
	if (_handle == -1)
	{
		return;
	}

	const Vector2D screen_pos = Vector2D::WorldToViewport(GetWorldPosition(), camera_params);
	const float actual_ex_rate = 1.f / camera_params.screen_scale * 1.25f * _ex_rate;

	DxLib::DrawRotaGraph(
		screen_pos.x,
		screen_pos.y,
		actual_ex_rate,
		0.0,
		_handle,
		TRUE
	);
}

void InAnimateRenderer::SetIcon(const MasterDataID icon_id)
{
	_handle = MasterHelper::GetGameIconHandleForDxLib(icon_id);
	_icon_id = icon_id;
}
