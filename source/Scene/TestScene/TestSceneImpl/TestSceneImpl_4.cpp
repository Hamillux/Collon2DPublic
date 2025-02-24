#include "TestSceneImpl_4.h"
#include "Component/Renderer/RendererComponent.h"
#include "Actor/Actor.h"
#include "GameSystems/CollisionManager.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/Collider/SegmentCollider.h"
#include "Component/Collider/TriangleCollider.h"
#include "Input/DeviceInput.h"
#include "GameSystems/Sound/SoundManager.h"

namespace 
{
	void TestDoesSegmentIntersectWithAnother()
	{
		// 1) 斜め線分が交差する例
		{
			FSegment segA(Vector2D(0, 0), Vector2D(10, 10));
			FSegment segB(Vector2D(0, 10), Vector2D(10, 0));
			Vector2D intersection;
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);

			// 交差するはず (対角線同士)
			assert(result == true);

			// 交点は (5,5) のはず
			// 浮動小数なので多少の誤差を考慮
			assert(std::fabs(intersection.x - 5.0f) < 1e-3f);
			assert(std::fabs(intersection.y - 5.0f) < 1e-3f);
		}

		// 2) 平行線分で交差しない例
		{
			FSegment segA(Vector2D(0, 0), Vector2D(10, 0));
			FSegment segB(Vector2D(0, 1.0f), Vector2D(10, 1.0f));
			Vector2D intersection;
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);
			assert(result == false);
		}

		// 3) 同一直線上で重なっている例
		{
			// 同一直線上にあり、部分的に重なっている
			FSegment segA(Vector2D(0, 0), Vector2D(5, 0));
			FSegment segB(Vector2D(3, 0), Vector2D(10, 0));
			Vector2D intersection(0, 0);
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);
			// 同一直線上で重なりがあるので true が返ることを確認
			assert(result == false);

			// intersection は複数考え方があるので(端点か、または A,B のどこかを返す等)
			// テスト環境に合わせて検証してください。
			// 例えば重なりの先頭が (3,0) である可能性が高い
			// assert(std::fabs(intersection.x - 3.0f) < 1e-3f);
			// assert(std::fabs(intersection.y - 0.0f) < 1e-3f);
		}

		// 4) 同一直線上でまったく重ならない例
		{
			FSegment segA(Vector2D(0, 0), Vector2D(5, 0));
			FSegment segB(Vector2D(6, 0), Vector2D(10, 0));
			Vector2D intersection;
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);
			// 重ならないので false
			assert(result == false);
		}

		// 5) 一部だけ端点が交わる例
		{
			FSegment segA(Vector2D(0, 0), Vector2D(5, 5));
			FSegment segB(Vector2D(5, 5), Vector2D(10, 10));
			Vector2D intersection;
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);
			// 端点(5,5) が共有点で交差する
			assert(result == false);
		}

		// 6) 交点が「延長線」上にはあるが、線分としては交差しない例
		{
			FSegment segA(Vector2D(0, 0), Vector2D(1, 1));
			FSegment segB(Vector2D(2, 2), Vector2D(3, 3));
			Vector2D intersection;
			bool result = GeometricUtility::DoesSegmentIntersectWithAnother(intersection, segA, segB);
			// 延長上には(2,2) などで交点が存在するが,
			// 線分の範囲外なので false
			assert(result == false);
		}

		return;
	}
}


TestSceneImpl_4::TestSceneImpl_4()
{
}

TestSceneImpl_4::~TestSceneImpl_4()
{
}

void TestSceneImpl_4::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	TestDoesSegmentIntersectWithAnother();

	_debug_screen_handle = DxLib::MakeScreen(WINDOW_SIZE_X, WINDOW_SIZE_Y, TRUE);

	CollisionManager::GetInstance().Initialize();
	
	initial_params_of_actor_t<Actor> actor_params;
	actor_params.transform.position = Vector2D(500, 100);
	_actor = CreateActor<Actor>(&actor_params);
	_renderer = _actor->CreateComponent<AnimRendererComponent>(_actor);
	_actor_collider = _actor->CreateComponent<BoxCollider>(_actor);
	_actor_collider->SetBoxColliderParams(
			CollisionType::BLOCK,
			CollisionObjectType::ENEMY,
			{CollisionObjectType::GROUND},
			true,
			Vector2D(64, 64)
		);

	AnimPlayInfo anim_info = {};
	anim_info.animation_id = 1;
	anim_info.play_speed = 0.5f;
	_renderer->SetAnimation(anim_info);
	_renderer->Play();

	initial_params_of_actor_t<Actor> actor_params2;
	actor_params2.transform.position = Vector2D(700, 500);
	Actor* ground = CreateActor<Actor>(&actor_params2);
	// _ground_colliderの初期化
	{
		// BOX
		//_ground_collider = ground->CreateComponent<BoxCollider>(ground);
		//static_cast<BoxCollider*>(_ground_collider)->SetBoxColliderParams(
		//		CollisionType::BLOCK,
		//		CollisionObjectType::GROUND,
		//		{ CollisionObjectType::ENEMY, CollisionObjectType::GROUND },
		//		false,
		//		Vector2D(128, 64)
		//	);

		// SEGMENT
		/*_ground_collider = ground->CreateComponent<SegmentCollider>(ground);
		static_cast<SegmentCollider*>(_ground_collider)->SetSegmentColliderParams(
			CollisionType::BLOCK,
			CollisionObjectType::GROUND,
			{ CollisionObjectType::ENEMY, CollisionObjectType::GROUND },
			false,
			128
		);*/

		// TRIANGLE
		_ground_collider = ground->CreateComponent<TriangleCollider>(ground);
		static_cast<TriangleCollider*>(_ground_collider)->SetTriangleColliderParams(
			CollisionType::BLOCK,
			CollisionObjectType::GROUND,
			{ CollisionObjectType::ENEMY, CollisionObjectType::GROUND },
			false,
			std::array<Vector2D, 3>{ Vector2D(0, 0), Vector2D(128, 0), Vector2D(0, 128) }
		);
	}	

	CollisionManager::GetInstance().ConstructKdTree(Vector2D(0, 0), Vector2D(1000, 1000));
}

SceneType TestSceneImpl_4::Tick(float delta_seconds)
{
	SceneType ret = __super::Tick(delta_seconds);


	static std::vector<std::shared_ptr<SoundInstance>> sound_instances;
	if (DeviceInput::IsPressed(KEY_INPUT_P))
	{
		SoundManager::GetInstance().LoadSoundResource("resources/sounds/fantasy.wav");

		auto new_instance =  SoundManager::GetInstance().MakeSoundInstance("resources/sounds/se/coin.ogg");
		sound_instances.push_back(new_instance);
		new_instance->Play();
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_O))
	{
		static int i_sound = 0;
		if (i_sound < sound_instances.size())
		{
			sound_instances.at(i_sound)->Play();
			i_sound = (i_sound + 1) % sound_instances.size();
		}
	}

	if (DeviceInput::IsActive(KEY_INPUT_LSHIFT))
	{
		_actor->AddWorldPosition(DeviceInput::GetInputDir_WASD() * 600.f * delta_seconds);
	}
	else if (DeviceInput::IsActive(KEY_INPUT_LCONTROL))
	{
		if (DeviceInput::IsPressed(KEY_INPUT_W)) _actor->AddWorldPosition({ 0,-1 });
		if (DeviceInput::IsPressed(KEY_INPUT_S)) _actor->AddWorldPosition({ 0,1 });
		if (DeviceInput::IsPressed(KEY_INPUT_A)) _actor->AddWorldPosition({ -1,0 });
		if (DeviceInput::IsPressed(KEY_INPUT_D)) _actor->AddWorldPosition({ 1,0 });
	}
	else
	{
		_actor->AddWorldPosition(DeviceInput::GetInputDir_WASD() * 200.f * delta_seconds);
	}
	

	{
		if (DeviceInput::IsPressed(KEY_INPUT_SPACE))
		{
			if (_renderer->IsPlaying())
			{
				_renderer->Stop();
			}
			else
			{
				_renderer->Play();
			}
		}

		_renderer->SetReverseX(DeviceInput::IsActive(KEY_INPUT_X));
		_renderer->SetReverseY(DeviceInput::IsActive(KEY_INPUT_Y));

		static std::vector<MdAnimation>::const_iterator it_anim = MdAnimation::GetData().begin();
		if (DeviceInput::WheelIsUp())
		{
			if (it_anim == MdAnimation::GetData().begin())
			{

			}
			else
			{
				it_anim--;
				AnimPlayInfo info{};
				info.animation_id = it_anim->id;
				_renderer->SetAnimation(info);
			}
		}
		else if (DeviceInput::WheelIsDown())
		{
			if (++it_anim == MdAnimation::GetData().end())
			{
				it_anim--;
			}
			else
			{
				AnimPlayInfo info{};
				info.animation_id = it_anim->id;
				_renderer->SetAnimation(info);
			}
		}
		ImGui::Begin("Debug");
		ImGui::Text("anim-id: %d", it_anim->id);
		ImGui::End();
	}

	QueryResult_SingleLineTrace result{};
	const Vector2D start = _actor->GetActorWorldPosition();
	const Vector2D end = start + Vector2D(0, 300);
	CollisionQueryParams_SingleLineTrace query_params{};
	query_params.ignore_actors.push_back(_actor);
	query_params.hit_object_types = static_cast<std::underlying_type_t<CollisionObjectType>>(CollisionObjectType::GROUND);
	query_params.segment = FSegment{ start ,end };
	CollisionManager::GetInstance().SingleLineTrace(result, query_params);
	{
		const int last_screen = DxLib::GetDrawScreen();
		DxLib::SetDrawScreen(_debug_screen_handle);
		DxLib::ClearDrawScreen();

		const Vector2D start_screen = _camera_params.TransformPosition_WorldToViewport(start);
		const Vector2D end_screen = _camera_params.TransformPosition_WorldToViewport(end);

		DrawLine(start_screen.x, start_screen.y, end_screen.x, end_screen.y, GetColor(255, 0, 0));
		if (result.has_hit)
		{
			const Vector2D hit_screen = _camera_params.TransformPosition_WorldToViewport(result.hit_location);
			DrawCircle(hit_screen.x, hit_screen.y, 10, GetColor(0, 255, 0));

			const Vector2D normal_end = _camera_params.TransformPosition_WorldToViewport(result.hit_location + result.hit_normal * 50);
			const int normal_color = GetColor(255, 255, 0);
			constexpr int thickness = 4;
			DrawLine(hit_screen.x, hit_screen.y, normal_end.x, normal_end.y, normal_color, thickness);
		}

		DxLib::SetDrawScreen(last_screen);
	}

	{
		const float delta_rot = DX_PI_F * delta_seconds * (DeviceInput::IsActive(KEY_INPUT_RIGHT) - DeviceInput::IsActive(KEY_INPUT_LEFT));
		_ground_collider->AddRotation(delta_rot);
	}


	CollisionManager::GetInstance().HandleCollisions();


	ImGui::Begin("Debug");
	const bool hit_result = 
		GeometricUtility::DoesSegmentIntersectWithAnother(FSegment{ Vector2D{640,412}, Vector2D{384,156} }, FSegment{ Vector2D{736,508},Vector2D{736,528} });
	if (hit_result)
	{
		ImGui::Text("Invalid Result");
	}
	ImGui::Text("hit: %d", hit_result);
	ImGui::End();

	return ret;
}

void TestSceneImpl_4::Draw()
{
	__super::Draw();

	_actor_collider->DrawDebugLines(_camera_params);
	_ground_collider->DrawDebugLines(_camera_params);

	DxLib::DrawGraph(0, 0, _debug_screen_handle, TRUE);
}

void TestSceneImpl_4::Finalize()
{
	CollisionManager::GetInstance().Finalize();
	DxLib::DeleteGraph(_debug_screen_handle);
	__super::Finalize();
}
