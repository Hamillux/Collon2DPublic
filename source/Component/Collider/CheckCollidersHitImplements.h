#pragma once

#include "Component/Collider/HitResult.h"

class BoxCollider;
class SegmentCollider;
class CircleCollider;
class TriangleCollider;

// 衝突判定
void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const BoxCollider* box2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const SegmentCollider* segment2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const CircleCollider* circle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const BoxCollider* box1, const TriangleCollider* triangle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const SegmentCollider* segment2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const CircleCollider* circle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const TriangleCollider* triangle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const CircleCollider* circle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const TriangleCollider* triangle2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const TriangleCollider* triangle2);

void CheckCollidersHitImpl(HitResult& out_result_for_1, const SegmentCollider* segment1, const BoxCollider* box2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const BoxCollider* box2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const BoxCollider* box2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const CircleCollider* circle1, const SegmentCollider* segment2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const SegmentCollider* segment2);
void CheckCollidersHitImpl(HitResult& out_result_for_1, const TriangleCollider* triangle1, const CircleCollider* circle2);