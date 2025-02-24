#include "CheckCollidersHitImplements.h"

#include "Component/Collider/BoxCollider.h"
#include "Component/Collider/SegmentCollider.h"
// #include "Component/Collider/CircleCollider.h"
#include "Component/Collider/TriangleCollider.h"

void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const BoxCollider* box2)
{
	FRect rect_1 = FRect(box1->GetWorldPosition(), box1->GetBoxExtent().x, box1->GetBoxExtent().y, box1->GetWorldRotation());
	FRect rect_2 = FRect(box2->GetWorldPosition(), box2->GetBoxExtent().x, box2->GetBoxExtent().y, box2->GetWorldRotation());
	Vector2D penetration_depth{};
	const bool has_hit = GeometricUtility::DoesRectOverlapWithAnother(rect_1, rect_2, &penetration_depth);
	if (!has_hit)
	{
		out_result_for_1.has_hit = false;
		return;
	}

	out_result_for_1 = MakeHitResult(box1, box2, penetration_depth);
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const SegmentCollider* segment2)
{
	const FRect rect = FRect(box1->GetWorldPosition(), box1->GetBoxExtent().x, box1->GetBoxExtent().y, box1->GetWorldRotation());
	const FSegment segment = FSegment(segment2->CalcSegmentEndPosition(SegmentEnd::LEFT), segment2->CalcSegmentEndPosition(SegmentEnd::RIGHT));
	Vector2D penetration_depth{};
	const bool has_hit = GeometricUtility::DoesRectOverlapWithSegment(rect, segment, penetration_depth);
	if (!has_hit)
	{
		out_result_for_1.has_hit = false;
		return;
	}

	out_result_for_1 = MakeHitResult(box1, segment2, penetration_depth);
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const CircleCollider* circle2)
{
	// TODO: 矩形コライダーと円形コライダーの衝突判定
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const TriangleCollider* triangle2)
{
	const FRect geometric_rect1 = box1->GetGeometricRect();
	const FTriangle geometric_triangle2 = triangle2->GetTriangle();
	Vector2D penetration_depth{};
	const bool has_hit = GeometricUtility::DoesRectOverlapWithTriangle(geometric_rect1, geometric_triangle2, &penetration_depth);
	if (!has_hit)
	{
		out_result_for_1.has_hit = false;
		return;
	}

	out_result_for_1 = MakeHitResult(box1, triangle2, penetration_depth);
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const SegmentCollider* segment2)
{
	// TODO: 線分コライダーと線分コライダーの衝突判定
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const CircleCollider* circle2)
{
	// TODO: 線分コライダーと円形コライダーの衝突判定
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const TriangleCollider* triangle2)
{
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const CircleCollider* circle2)
{
	// TODO: 円形コライダーと円形コライダーの衝突判定
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const TriangleCollider* triangle2)
{
	throw std::runtime_error("Not implemented yet");
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const TriangleCollider* triangle2)
{
	throw std::runtime_error("Not implemented yet");
	Vector2D penetration_depth;
	const bool has_hit = GeometricUtility::DoesTriangleOverlapWIthAnother(triangle1->GetTriangle(), triangle2->GetTriangle(), &penetration_depth);
	if (!has_hit)
	{
		out_result_for_1.has_hit = false;
		return;
	}

	MakeHitResult(triangle1, triangle2, penetration_depth);
}


void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const BoxCollider* box2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, box2, segment1);
	out_result_for_1 = result_for_2.GetInverted();
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const BoxCollider* box2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, box2, circle1);
	out_result_for_1 = result_for_2.GetInverted();
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const BoxCollider* box2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, box2, triangle1);
	out_result_for_1 = result_for_2.GetInverted();
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const SegmentCollider* segment2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, segment2, circle1);
	out_result_for_1 = result_for_2.GetInverted();
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const SegmentCollider* segment2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, segment2, triangle1);
	out_result_for_1 = result_for_2.GetInverted();
}

void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const CircleCollider* circle2)
{
	HitResult result_for_2;
	CheckCollidersHitImpl(result_for_2, circle2, triangle1);
	out_result_for_1 = result_for_2.GetInverted();
}
