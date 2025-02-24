#include "GoalFlag.h"

#include "GameSystems/MasterData/MasterDataInclude.h"
#include "Component/Renderer/RendererComponent.h"
#include "Component/Collider/BoxCollider.h"

namespace 
{
	constexpr MasterDataID GOAL_FLAG_ICON_ID = 374;
}

GoalFlag::GoalFlag()
	: _renderer(nullptr)
	, _ghandle(-1)
{

}

GoalFlag::~GoalFlag()
{
}

void GoalFlag::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	const MdGameIcon& icon = MdGameIcon::Get(GOAL_FLAG_ICON_ID);
	const float height_per_width = static_cast<float>(icon.height) / icon.width;
	_renderer = CreateComponent<InAnimateRenderer>(this);
	_renderer->SetIcon(GOAL_FLAG_ICON_ID);
	_renderer->SetExtent(Vector2D{UNIT_TILE_SIZE, UNIT_TILE_SIZE * height_per_width});
	_renderer->SetLocalPosition(Vector2D{0.f, -16.f});

	_collider = CreateComponent<BoxCollider>(this);
	_collider->SetBoxColliderParams(
		CollisionType::OVERLAP,
		CollisionObjectType::GOAL_FLAG,
		{ CollisionObjectType::PLAYER },
		false,
		Vector2D{ UNIT_TILE_SIZE, UNIT_TILE_SIZE * 2.f}
		);
	_collider->SetLocalPosition(Vector2D{0, -16.f});
}

void GoalFlag::Finalize()
{
	__super::Finalize();
}

void GoalFlag::GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes)
{
	_collider->GetVertexPositions(out_vertexes);
}
