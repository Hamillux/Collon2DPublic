#pragma once

#include "SceneObject/Component/SceneComponent.h"
#include "CollisionType.h"
#include "HitResult.h"
#include <vector>

struct HitResult;
class BoxCollider;
class SegmentCollider;
class CircleCollider;
class TriangleCollider;

struct ColliderDebugDrawDesc 
{
	bool draw_aabb;

	ColliderDebugDrawDesc()
		: draw_aabb(false)
		, segment{}
		, box{}
	{}

	struct Segment
	{
		Segment()
			: line_color(0xFF0000)
			, line_alpha(255)
			, line_thickness(3)
		{}
		int line_color;
		int line_alpha;
		int line_thickness;
	};
	Segment segment;

	struct Box 
	{
		Box()
			: line_color(0xFF0000)
			, line_alpha(255)
			, line_thickness(3)
		{}
		int line_color;
		int line_alpha;
		int line_thickness;

	};
	Box box;

	struct Triangle 
	{
		Triangle()
			: line_color(0xFF0000)
			, line_alpha(255)
			, line_thickness(3)
		{}
		int line_color;
		int line_alpha;
		int line_thickness;
	};

	Triangle triangle;
};


class ColliderBase : public SceneComponent
{
protected:
	enum class ColliderShape : uint8_t
	{
		BOX, SEGMENT, CIRCLE, TRIANGLE
	};

public:
	ColliderBase()
		: is_active(true)
		, should_check_with_has_common_parent(false)
		, collision_type(CollisionType::OVERLAP)
		, collision_object_type(CollisionObjectType::WILDCARD)
		, hit_object_types(0)
		, is_pushable(false)
		, debug_collider_id(0)
		, _generate_hit_event(true)
	{}
	virtual ~ColliderBase() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Draw(const ScreenParams& screen_params) override;
	virtual void Finalize() override;
	virtual void OnParentActorHiddenChanged(const bool new_hidden) override;
	//~ End ComponentBase interface

	//~ Begin SceneComponent interface
	virtual void OnThisWorldTransformChanged();
	//~ End SceneComponent interface

	//~ Begin ColliderBase interface
public:
	virtual ColliderShape GetColliderShape() const = 0;
	/// <summary>
	/// コライダーの頂点の座標を取得する. out_vertex_positionsの隣り合う要素同士が辺を形成する
	/// </summary>
	virtual void GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const = 0;
	virtual void RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params) = 0;
	virtual void RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params) = 0;
	virtual void DrawDebugLines(const ScreenParams& screen_params, const ColliderDebugDrawDesc& desc = ColliderDebugDrawDesc{});
	// コライダーの中心のワールド座標を取得
	virtual Vector2D GetCenterWorldPosition() const = 0;
	virtual void GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const = 0;
private:
	void CheckHitWith(HitResult& out_result_for_self, const ColliderBase* other_collider) const;
	//~ End ColliderBase interface

public:
	bool IsOverlappingWith(const ColliderBase* other_collider) const;

	/// <summary>
	/// other_colliderとの衝突有無をチェックし, 衝突有の場合は衝突解消とその後の処理を行う.
	/// </summary>
	/// <param name="other_collider">自コライダーとの衝突をチェックするコライダー</param>
	void HandleCollisionWith(const ColliderBase* other_collider) const;

	CollisionObjectType GetCollisionObjectType() const { return collision_object_type; }
	CollisionType GetCollisionType() const { return collision_type; }

	// 押し戻し
	bool IsPushable() const { return is_pushable; }
	
	// コリジョンの有効・無効
	void Deactivate();
	void Activate();
	bool IsActive() const;

	/// <summary>
	/// ヒット時にActor::OnHitCollision()を呼ぶか否か. デフォルトはtrue
	/// </summary>
	/// <param name="new_generate_hit_event"></param>
	void SetGenerateHitEvent(const bool new_generate_hit_event) { _generate_hit_event = new_generate_hit_event; }

	/// <summary>
	/// 親アクターが同じコライダーとの衝突も判定するか否か. デフォルトはfalse
	/// </summary>
	/// <param name="new_should_check_with_has_common_parent"></param>
	void SetShouldCheckWithHasCommonParent(const bool new_should_check_with_has_common_parent);

	// 衝突対象
	bool IsHitCheckTarget(const CollisionObjectType target_object_type) const;
	static bool IsHitCheckTarget(const CollisionObjectType target_object_type, const CollisionObjectType_UnderlyingType hit_targets);
	void SetHitObjectTypes(const std::vector<CollisionObjectType>& new_hit_object_types);
	CollisionType GetCollisionTypeBetween(const ColliderBase* other) const;
	void AddHitTarget(const CollisionObjectType new_target);
	void AddHitTarget(const std::vector<CollisionObjectType>& new_targets);
	void RemoveHitTarget(const CollisionObjectType target_to_remove);
	void RemoveHitTarget(const std::vector<CollisionObjectType>& targets_to_remove);

	/// <summary>
	/// 指定した種類を除いた全種類のコライダーを衝突対象に設定
	/// </summary>
	/// <param name="target_exception"></param>
	void AddToHitTargetsOtherThan(const CollisionObjectType target_exception);

	int DEBUG_GetId() { return debug_collider_id; }
	void DEBUG_SetId(const int new_id) { debug_collider_id = new_id; }

protected:
	struct DebugColliderLineParams
	{
		constexpr static const int line_thickness = 3;
		constexpr static const int active_block_collider_color = 0xFF0000;
		constexpr static const int active_overlap_collider_color = 0x00FF00;
		constexpr static const int inactive_collider_color = 0x0000FF;
		constexpr static const int collider_alpha = 255;
		constexpr static const int aabb_color = 0xFFFFFF;
		constexpr static const int aabb_alpha = 255;
	};

	// 全形状のコライダーで共通の設定
	void SetColliderCommonParams(
		const CollisionType new_collision_type,
		const CollisionObjectType new_collision_object_type,
		const std::vector<CollisionObjectType>& new_hit_object_types,
		const bool new_pushability
	);

	// 衝突判定
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const BoxCollider* box2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const SegmentCollider* segment2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const CircleCollider* circle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const TriangleCollider* triangle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const SegmentCollider* segment2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const CircleCollider* circle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const TriangleCollider* triangle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const CircleCollider* circle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const TriangleCollider* triangle2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const TriangleCollider* triangle2) const;

	// 上で宣言されているオーバーロードを利用する.
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const BoxCollider* box2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const BoxCollider* box2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const BoxCollider* box2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const SegmentCollider* segment2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const SegmentCollider* segment2) const;
	void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const CircleCollider* circle2) const;

	template<typename QueryParamsType>
	bool ShouldCheckQueryHit(const QueryParamsType& query_params)
	{
		const bool is_parent_ignored = std::find(query_params.ignore_actors.begin(), query_params.ignore_actors.end(), GetOwnerActor()) != query_params.ignore_actors.end();
		const bool ret = 
			IsActive() &&
			IsHitCheckTarget(query_params.trace_object_type, hit_object_types) &&
			IsHitCheckTarget(collision_object_type, query_params.hit_object_types) &&
			!is_parent_ignored;
		return ret;
	}

private:
	bool ShouldCheckHitWith(const ColliderBase* other_collider) const;
	void HandleHitResult(const HitResult& hit_result_for_self) const;

	/// <summary>
	/// 押し戻し処理.
	/// </summary>
	/// <returns>押し戻しが行われたか</returns>
	bool HandlePushback(const HitResult& hit_result) const;

	/// <summary>
	/// 衝突結果を作成
	/// </summary>
	/// <param name="self_collider"></param>
	/// <param name="other_collider"></param>
	/// <param name="penetration_depth"></param>
	/// <returns></returns>
	HitResult MakeHitResult(const ColliderBase* self_collider, const ColliderBase* other_collider, const Vector2D& penetration_depth) const;

	bool is_active;
	bool _generate_hit_event;
	bool should_check_with_has_common_parent;	// 親アクターが同じコライダーとの衝突をチェックするか
	CollisionType collision_type;
	CollisionObjectType collision_object_type;
	unsigned int hit_object_types;
	bool is_pushable;

	// デバッグ線描画パラメータ
	DebugColliderLineParams debug_line_params;

	int debug_collider_id;
};