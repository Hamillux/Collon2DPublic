#include "CollisionManager.h"
#include "Actor/Actor.h"
#include "Component/Collider/ColliderBase.h"
#include <algorithm>

/// <summary>
/// xのn乗
/// </summary>
constexpr int64_t powc(const int32_t x, const uint8_t n)
{
	if (n == 0)
	{
		return 1;
	}
	else
	{
		return x * powc(x, n - 1);
	}
}

void CollisionManager::Initialize()
{

}

void CollisionManager::Finalize()
{
	if (IsTreeConstructed())
	{
		DestructKdTree();
	}

	all_colliders.clear();
	all_colliders.shrink_to_fit();
}

void CollisionManager::OnColliderTransformed(ColliderBase* collider)
{
	if (!IsTreeConstructed())
	{
		return;
	}

	// 子ノードに包含されるコライダーは親ノードにも包含されるので,
	// 現在の所属ノードが子ノードを持つ場合, 子ノードの方が最適な可能性がある.
	// 所属ノードの再検索が不要なのは, 現在の所属ノードに包含され, 且つ子ノードを持たない場合.
	const KdNode* const current_node = collider_node_map.at(collider);
	if(!current_node->HasChildNode() && CheckNodeContainsCollider(collider, current_node))
	{
		return;
	}

	KdNode* new_node = FindNode(collider);
	
	// 新しいノードに移す
	RemoveFromBelongingNode(collider);
	AddColliderToNode(collider, new_node);
}

void CollisionManager::DrawPartition(const KdNode* node, const CameraParams& camera_params, const int color) const
{
	if (!node->HasChildNode())
	{
		return;
	}

	const Vector2D left_top_in_world = node->GetAreaLeftTop();
	const Vector2D right_bottom_in_world = node->GetAreaRightBottom();

	Vector2D line_from_in_world, line_to_in_world;
	if (node->split_axis == Axis::X)
	{
		line_from_in_world.x = line_to_in_world.x = node->split_position;
		line_from_in_world.y = left_top_in_world.y;
		line_to_in_world.y = right_bottom_in_world.y;
	}
	else if (node->split_axis == Axis::Y)
	{
		line_from_in_world.x = left_top_in_world.x;
		line_to_in_world.x = right_bottom_in_world.x;
		line_from_in_world.y = line_to_in_world.y = node->split_position;
	}

	const Vector2D line_from_in_screen(Vector2D::WorldToViewport(line_from_in_world, camera_params));
	const Vector2D line_to_in_screen(Vector2D::WorldToViewport(line_to_in_world, camera_params));

	DrawLine(line_from_in_screen.x, line_from_in_screen.y, line_to_in_screen.x, line_to_in_screen.y, color);

	// 
	auto To2CharString = [](uint8_t x)->tstring
	{
		if (x >= 100)
		{
			return _T("XX");
		}

		tstring x_str = to_tstring(x);
		if (x < 10)
		{
			return _T(" ") + x_str;
		}
		else if (x < 100)
		{
			return x_str;
		}
	};

	const tstring smaller_node_index_str(To2CharString(node->GetLeftChildIndex()));
	const tstring larger_node_index_str(To2CharString(node->GetRightChildIndex()));
	const tstring partition_label = _T("(") + smaller_node_index_str + _T(",") + larger_node_index_str + _T(")");
	constexpr uint8_t FONT_SIZE = 16;
	if (node->split_axis == Axis::X)
	{
		const int label_width = DrawStringHelper::GetDrawStringWidth(FONT_SIZE, partition_label.c_str(), strlenDx(partition_label.c_str()));
		Vector2D label_left_top(line_to_in_screen + Vector2D(-label_width / 2, 0));
		DrawString(label_left_top.x, label_left_top.y, partition_label.c_str(), color);
	}
	else if (node->split_axis == Axis::Y)
	{
		Vector2D label_left_top(line_to_in_screen + Vector2D(0, -FONT_SIZE/2));
		DrawString(label_left_top.x, label_left_top.y, partition_label.c_str(), color);
	}
}

bool CollisionManager::CheckNodeContainsCollider(ColliderBase* const collider, const KdNode* const node) const
{
	// コライダーの外接AABB
	Vector2D aabb_left_top, aabb_right_bottom;
	collider->GetAABB(aabb_left_top, aabb_right_bottom);

	// ノードの領域にコライダーの外接AABBが内包されるかをチェックする
	bool collider_is_inside_node = GeometricUtility::DoesRectContainAnother(
		node->GetAreaLeftTop(),
		node->GetAreaRightBottom(),
		aabb_left_top,
		aabb_right_bottom
	);

	return collider_is_inside_node;
}

KdNode* CollisionManager::FindNode(ColliderBase* collider) const
{
	if (!IsTreeConstructed())
	{
		return nullptr;
	}

	// コライダーの外接AABB
	Vector2D aabb_left_top, aabb_right_bottom;
	collider->GetAABB(aabb_left_top, aabb_right_bottom);

	// ルートからスタートする
	KdNode* node = _root_node;
	
	while (node->HasChildNode())
	{
		// 左右子ノードどちらかに内包される場合は、さらにその子ノードについても調べる.
		// どちらの子ノードにも内包されない(分割軸と接触する)場合は、現在のノードに決定.

		const bool in_left_node = CheckNodeContainsCollider(collider, node->GetLeftChildNode());
		if (in_left_node)
		{
			node = node->GetLeftChildNode();
			continue;
		}

		// 右子ノードに含まれるか
		const bool in_right_node = CheckNodeContainsCollider(collider, node->GetRightChildNode());
		if (in_right_node)
		{
			node = node->GetRightChildNode();
			continue;
		}

		// 左右子ノードのセルどちらにも内包されない
		return node;
	}

	// ルートノードが子ノードを持たない
	return node;
}

KdNode* CollisionManager::FindNode(const Vector2D& query_point) const
{
	if (!IsTreeConstructed())
	{
		return nullptr;
	}

	if (!_root_node->HasChildNode())
	{
		return _root_node;
	}

	KdNode* node = _root_node;
	while (node->HasChildNode())
	{
		// 左子ノードに内包されるかチェック
		{
			const Vector2D left_top = node->GetLeftChildNode()->GetAreaLeftTop();
			const Vector2D right_bottom = node->GetLeftChildNode()->GetAreaRightBottom();
			const bool is_contained_in_left_node = GeometricUtility::DoesAARectContainPoint(query_point, left_top, right_bottom);
			if (is_contained_in_left_node)
			{
				node = node->GetLeftChildNode();
				continue;
			}
		}

		// 右子ノードに内包されるかチェック
		{
			const Vector2D left_top = node->GetRightChildNode()->GetAreaLeftTop();
			const Vector2D right_bottom = node->GetRightChildNode()->GetAreaRightBottom();
			const bool is_contained_in_right_node = GeometricUtility::DoesAARectContainPoint(query_point, left_top, right_bottom);
			if (is_contained_in_right_node)
			{
				node = node->GetRightChildNode();
				continue;
			}
		}

		// どちらの子ノードにも内包されない
		return node;
	}

	return nullptr;
}


void CollisionManager::ConstructKdTree(const Vector2D& root_cell_left_top, const Vector2D& root_cell_right_bottom)
{
	if (IsTreeConstructed())
	{
		// ツリーが構築済み
		return;
	}

	_root_node = new KdNode(0, 0, Axis::X, root_cell_left_top, root_cell_right_bottom);
	assert(_root_node);

	// キャッシュしておいたコライダーをルートノードに移す
	for (const auto& collider : all_colliders)
	{
		AddColliderToNode(collider, _root_node);
	}

	constexpr const uint32_t total_node_num = powc(2, MAX_TREE_DEPTH + 1) - 1;
	kd_nodes = std::vector<KdNode*>(total_node_num, nullptr);
	kd_nodes.at(0) = _root_node;

	SplitKdNodeRecursively(_root_node);
}

void CollisionManager::SplitKdNodeRecursively(KdNode* parent_node)
{
	assert(parent_node->depth <= MAX_TREE_DEPTH);

	if (!ShouldSplitNode(parent_node))
	{
		return;
	}

	// 分割軸. 分割線はこの軸に垂直.
	const Axis parent_split_axis = parent_node->split_axis;

	// ノードに属するコライダー
	std::vector<ColliderBase*>& colliders_in_parent = parent_node->GetCollidersInNode();

	// 分割軸に沿ってコライダーリストをソートする
	SortCollidersAlongAxis(colliders_in_parent, parent_split_axis);
	const size_t i_middle_collider = colliders_in_parent.size() / 2;
	
	// parent_nodeの中央のコライダーの 左(X分割の場合) or 上(Y分割の場合) に分割線を引く. --> 中央コライダーは右子ノードに属する
	Vector2D middle_collider_left_top, dummy;
	colliders_in_parent.at(i_middle_collider)->GetAABB(middle_collider_left_top, dummy);
	if (parent_split_axis == Axis::X)
	{
		// X軸で分割する場合、中央コライダーの左に分割線を引く
		const float middle_collider_left = middle_collider_left_top.x;
		parent_node->split_position = middle_collider_left - 1.f;
	}
	else
	{
		// Y軸で分割する場合、中央コライダーの上に分割線を引く
		const float middle_collider_top = middle_collider_left_top.y;
		parent_node->split_position = middle_collider_top - 1.f;
	}

	// 子ノードの深さと分割軸. 親子で分割軸は逆
	const int child_depth = parent_node->depth + 1;
	const Axis child_split_axis = (parent_split_axis == Axis::X) ? Axis::Y : Axis::X;

	// 左右子ノードの左上, 右下の座標
	Vector2D left_node_left_top, left_node_right_bottom, right_node_left_top, right_node_right_bottom;

	if (parent_split_axis == Axis::X)
	{
		const float top = parent_node->cell_left_top.y;
		const float bottom = parent_node->cell_right_bottom.y;

		// 左子ノードは分割線の左側
		left_node_left_top = parent_node->cell_left_top;
		left_node_right_bottom = Vector2D(parent_node->split_position, bottom);

		// 右子ノードは分割線の右側
		right_node_left_top = Vector2D(parent_node->split_position, top);
		right_node_right_bottom = parent_node->cell_right_bottom;
	}
	else
	{
		// Y軸分割
		const float left = parent_node->cell_left_top.x;
		const float right = parent_node->cell_right_bottom.x;

		// 左子ノードは分割線の上側
		left_node_left_top = parent_node->cell_left_top;
		left_node_right_bottom = Vector2D(right, parent_node->split_position);

		// 右子ノードは分割線の下側
		right_node_left_top = Vector2D(left, parent_node->split_position);
		right_node_right_bottom = parent_node->cell_right_bottom;
	}

	// 左子ノードを生成
	parent_node->left_node = new KdNode(
		child_depth, 
		parent_node->GetLeftChildIndex(), 
		child_split_axis,
		left_node_left_top, left_node_right_bottom
	);
	KdNode* const left_child = parent_node->GetLeftChildNode();
	kd_nodes.at(parent_node->GetLeftChildIndex()) = left_child;

	// 右子ノードを生成
	parent_node->right_node = new KdNode(
		child_depth,
		parent_node->GetRightChildIndex(),
		child_split_axis,
		right_node_left_top, right_node_right_bottom
	);
	KdNode* const right_child = parent_node->GetRightChildNode();
	kd_nodes.at(parent_node->GetRightChildIndex()) = right_child;
	
	// 親ノードが持つコライダーを左右子ノードに分配
	DistributeColliders(parent_node);

	// NOTE: 分割の必要性は関数先頭でチェックされる
	SplitKdNodeRecursively(left_child);
	SplitKdNodeRecursively(right_child);
}

void CollisionManager::DistributeColliders(KdNode* const parent_node)
{
	KdNode* const left_node = parent_node->GetLeftChildNode();
	KdNode* const right_node = parent_node->GetRightChildNode();
	if (!parent_node || !left_node || !right_node)
	{
		return;
	}

	const std::vector<ColliderBase*>& colliders_in_parent = parent_node->GetCollidersInNode();

	// コライダーが子ノードに内包される場合は親ノードから子ノードに移し、
	// 分割軸と接触する場合は親ノードに残す
	auto it_collider = colliders_in_parent.begin();
	while (it_collider != colliders_in_parent.end())
	{
		ColliderBase* const collider = *it_collider;

		Vector2D aabb_left_top, aabb_right_bottom;
		collider->GetAABB(aabb_left_top, aabb_right_bottom);

		// 左子ノードに内包される
		if(CheckNodeContainsCollider(collider, left_node))
		{
			// 親ノードから左子ノードにコライダーを移動
			RemoveFromBelongingNode(it_collider);
			AddColliderToNode(collider, left_node);
			continue;
		}

		// 右子ノードに内包される
		if(CheckNodeContainsCollider(collider, right_node))
		{
			// 親ノードから右子ノードにコライダーを移動
			RemoveFromBelongingNode(it_collider);
			AddColliderToNode(collider, right_node);
			continue;
		}

		// *iteratorは左右子ノードどちらにも属さない
		++it_collider;
	}
}

void CollisionManager::DestructKdTree()
{
	if (!IsTreeConstructed())
	{
		return;
	}

	DestroyKdNodesRecursively(_root_node);
	kd_nodes.clear();
	kd_nodes.shrink_to_fit();
	collider_node_map.clear();
}

void CollisionManager::ReConstructKdTree()
{
	if (!IsTreeConstructed())
	{
		return;
	}
	const Vector2D area_left_top = _root_node->GetAreaLeftTop();
	const Vector2D area_right_bottom = _root_node->GetAreaRightBottom();
	DestructKdTree();
	ConstructKdTree(area_left_top, area_right_bottom);
	return;
}

void CollisionManager::ReConstructKdTree(const Vector2D& new_area_left_top, const Vector2D& new_area_right_bottom)
{
	if (!IsTreeConstructed())
	{
		return;
	}

	DestructKdTree();
	ConstructKdTree(new_area_left_top, new_area_right_bottom);
}


void CollisionManager::HandleCollisions()
{
	for (auto& node : kd_nodes)
	{
		if (!node)
		{
			continue;
		}
		HandleCollisionsBetweenNodes(node, node);
	}
}

void CollisionManager::HandleCollisionsBetweenNodes(const KdNode* fixed_node, const KdNode* root_of_targets)
{
	if (!fixed_node || !root_of_targets) return;

	if (root_of_targets->left_node && root_of_targets->right_node)
	{
		HandleCollisionsBetweenNodes(fixed_node, root_of_targets->left_node);
		HandleCollisionsBetweenNodes(fixed_node, root_of_targets->right_node);
	}

	if (fixed_node == root_of_targets)
	{
		// 1ノードに属するコライダーの全組み合わせ
		std::vector<ColliderBase*> fixed_colliders = fixed_node->colliders;
		for (auto iterator = fixed_colliders.begin(); iterator != fixed_colliders.end(); ++iterator)
		{
			for (auto sub_iterator = iterator + 1; sub_iterator != fixed_colliders.end(); ++sub_iterator)
			{
				(*iterator)->HandleCollisionWith(*sub_iterator);
			}
		}
	}
	else
	{
		// 異なる2ノードに属するコライダーの全組み合わせ
		std::vector<ColliderBase*> fixed_colliders = fixed_node->colliders;
		std::vector<ColliderBase*> target_colliders = root_of_targets->colliders;
		for (auto& collider1 : fixed_colliders)
		{
			for (auto& collider2 : target_colliders)
			{
				collider1->HandleCollisionWith(collider2);
			}
		}
	}
}

void CollisionManager::OnNewColliderInitialized(ColliderBase* new_collider)
{
	if (!new_collider)
	{
		return;
	}

	all_colliders.push_back(new_collider);

	if (IsTreeConstructed())
	{
		// ツリーから新コライダーを内包するノードを探す
		KdNode* const new_collider_node = FindNode(new_collider);
		AddColliderToNode(new_collider, new_collider_node);
	}
}

void CollisionManager::DestroyKdNodesRecursively(KdNode*& current_node)
{
	if (current_node->left_node != nullptr)
	{
		DestroyKdNodesRecursively(current_node->left_node);
	}
	if (current_node->right_node != nullptr)
	{
		DestroyKdNodesRecursively(current_node->right_node);
	}

	delete current_node;
	current_node = nullptr;
}

void CollisionManager::OnColliderFinalize(ColliderBase* collider)
{

	// 破壊されるコライダーを所属ノードから削除
	RemoveFromBelongingNode(collider);

	auto it_collider = std::find(all_colliders.begin(), all_colliders.end(), collider);
	if(it_collider != all_colliders.end())
	{
		all_colliders.erase(it_collider);
	}
}

void CollisionManager::DrawCells(const CameraParams& camera_params, const uint8_t node_depth) const
{
	if (node_depth > MAX_TREE_DEPTH)
	{
		return;
	}
	const int line_thickness = ceil(camera_params.screen_scale);
	constexpr const int color = 0x00FFFF;
	
	if (_root_node)
	{
		// ルートノードセルの描画
		int cell_left, cell_top, cell_right, cell_bottom;
		(Vector2D::WorldToViewport(_root_node->cell_left_top, camera_params)).ToIntRound(cell_left, cell_top);
		(Vector2D::WorldToViewport(_root_node->cell_right_bottom, camera_params)).ToIntRound(cell_right, cell_bottom);
		DrawBoxAA(cell_left, cell_top, cell_right, cell_bottom, color, false, line_thickness);
		
		DrawCircle(cell_left, cell_top, 3, color, true);
		const tstring text_left_top = _T("RootNode-LeftTop");
		const int draw_str_left = cell_left - GetDrawStringWidth(text_left_top.c_str(), strlenDx(text_left_top.c_str())) / 2;
		const int draw_str_top = cell_top - GetFontSize();
		DrawString(draw_str_left, draw_str_top, text_left_top.c_str(), color);

		const tstring text_right_bottom = _T("RootNode-RightBottom");
		const int draw_str_right = cell_right - GetDrawStringWidth(text_right_bottom.c_str(), strlenDx(text_right_bottom.c_str())) / 2;
		const int draw_str_bottom = cell_bottom + GetFontSize();
		DrawString(draw_str_right, draw_str_bottom, text_right_bottom.c_str(), color);
	}

	for (const auto& node : kd_nodes)
	{
		if(node)
		{
			DrawPartition(node, camera_params, color);
		}
	}

	return;
	
	// 指定された深さのノードの内, ツリーで最も左,最も右にあるノード
	const uint8_t left_most_node_number = pow(2, node_depth) - 1;
	const uint8_t right_most_node_number = pow(2, node_depth + 1) - 2;

	for (int i = 0; i <= right_most_node_number; i++)
	{
		const KdNode* const node = kd_nodes.at(i);
		if (node)
		{
			int x1, y1, x2, y2;

			(Vector2D::WorldToViewport(node->cell_left_top, camera_params)).ToIntRound(x1, y1);
			(Vector2D::WorldToViewport(node->cell_right_bottom, camera_params)).ToIntRound(x2, y2);
			DrawBoxAA(x1, y1, x2, y2, color, false, line_thickness);
		}
	}
}

void GetOverlappingCollidersImpl(std::vector<ColliderBase*>& out_overlapping_colliders, const KdNode* const node, ColliderBase* const target_collider, const bool check_descendant_nodes = false)
{
	if (check_descendant_nodes && node->HasChildNode())
	{
		GetOverlappingCollidersImpl(out_overlapping_colliders, node->GetLeftChildNode(), target_collider, true);
		GetOverlappingCollidersImpl(out_overlapping_colliders, node->GetRightChildNode(), target_collider, true);
	}

	const std::vector<ColliderBase*>& colliders_in_node = node->colliders;
	for (const auto& other_collider : colliders_in_node)
	{
		if (other_collider == target_collider)
		{
			continue;
		}
		if (target_collider->IsOverlappingWith(other_collider))
		{
			out_overlapping_colliders.push_back(other_collider);
		}
	}
}

void CollisionManager::GetOverlappingColliders(std::vector<ColliderBase*>& out_overlapping_colliders, ColliderBase* const target_collider) const
{
	if (target_collider->GetCollisionType() != CollisionType::OVERLAP)
	{
		return;
	}

	const KdNode* target_node = collider_node_map.at(target_collider);

	// 1. target_colliderが所属するノードと, その子孫ノード
	GetOverlappingCollidersImpl(out_overlapping_colliders, target_node, target_collider, true);

	// 2. 祖先ノード
	uint8_t target_node_index = target_node->GetNodeIndex();
	// target_nodeがルートノードでなければ祖先ノードについて調べる
	if (target_node_index != 0)
	{
		uint8_t parent_node_index = (target_node_index - 1) / 2;
		while (true)
		{
			GetOverlappingCollidersImpl(out_overlapping_colliders, kd_nodes.at(parent_node_index), target_collider, false);

			if (parent_node_index == 0)
			{
				// ルートに到達したらループを抜ける
				break;
			}
			else
			{
				// 親の親についても調べる
				parent_node_index = (parent_node_index - 1) / 2;
			}
		}
	}
}

void CollisionManager::FindOverlappingNodesRecursively(std::vector<KdNode*>& out_intersected_nodes, KdNode* node, const FSegment& segment) const
{
	if (!node)
	{
		return;
	}
	
	if (IsOverlapping(node, segment))
	{
		out_intersected_nodes.push_back(node);

		if (node->HasChildNode()) 
		{
			FindOverlappingNodesRecursively(out_intersected_nodes, node->GetLeftChildNode(), segment);
			FindOverlappingNodesRecursively(out_intersected_nodes, node->GetRightChildNode(), segment);
		}
	}
}

void CollisionManager::FindOverlappingNodesRecursively(std::vector<KdNode*>& out_intersected_nodes, KdNode* node, const FRectAA& rect) const
{
	if (!IsTreeConstructed() ||  !node)
	{
		return;
	}

	if (IsOverlapping(node, rect))
	{
		out_intersected_nodes.push_back(node);

		if (node->HasChildNode())
		{
			FindOverlappingNodesRecursively(out_intersected_nodes, node->GetLeftChildNode(), rect);
			FindOverlappingNodesRecursively(out_intersected_nodes, node->GetRightChildNode(), rect);
		}
	}
}

bool CollisionManager::IsOverlapping(const KdNode* const node, const FSegment& segment) const
{
	const FRectAA rect{ node->GetAreaLeftTop(), node->GetAreaRightBottom() };

	const bool is_segment_contained = GeometricUtility::DoesAARectContainSegment(rect, segment);
	if (is_segment_contained)
	{
		return true;
	}
	
	const int num_intersections = GeometricUtility::GetSegmentAARectIntersections(segment, rect);
	return num_intersections > 0;
}

bool CollisionManager::IsOverlapping(const KdNode* const node, const FRectAA& rect) const
{
	const FRectAA node_rect{ node->GetAreaLeftTop(), node->GetAreaRightBottom() };
	return GeometricUtility::DoesAARectOverlapWithAnother(node_rect, rect);
}

int CollisionManager::DEBUG_GetNodeIndex(ColliderBase* collider)
{
	auto it = collider_node_map.find(collider);
	if (it == collider_node_map.end())
	{
		return -1;
	}

	return it->second->node_index;
}


void CollisionManager::SingleLineTrace(QueryResult_SingleLineTrace& out_query_result, const CollisionQueryParams_SingleLineTrace& query_params)
{
	if (!IsTreeConstructed())
	{
		return;
	}

	out_query_result = QueryResult_SingleLineTrace{};

	// ラインと交差するノードを取得
	std::vector<KdNode*> intersected_nodes = {};
	FindOverlappingNodesRecursively(intersected_nodes, _root_node, query_params.segment);

	// 取得したノード内のコライダーとラインの交差をチェック
	std::vector<QueryResult_SingleLineTrace> results = {};
	for (auto& node : intersected_nodes)
	{
		for (auto& collider : node->colliders)
		{
			QueryResult_SingleLineTrace result{};
			collider->RespondToSingleLineTrace(result, query_params);
			if (result.has_hit)
			{
				results.push_back(result);

				if (result.hit_location.IsZeroVector())
				{
					int x = 0;
				}
			}
		}
	}

	if (results.empty())
	{
		out_query_result.has_hit = false;
		return;
	}
	else
	{
		// 結果をソート
		std::sort(results.begin(), results.end(), [&query_params](const QueryResult_SingleLineTrace& a, const QueryResult_SingleLineTrace& b)
			{
				const Vector2D& loc_a = a.hit_location;
				const Vector2D& loc_b = b.hit_location;
				return (loc_a - query_params.segment.start).LengthSquared() < (loc_b - query_params.segment.start).LengthSquared();
			});

		// ラインの始点から最も近いコライダーの結果を採用
		out_query_result = results.at(0);
	}
}

void CollisionManager::MultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params)
{
	if (!IsTreeConstructed())
	{
		return;
	}

	query_result = QueryResult_MultiAARectTrace{};

	// クエリの矩形と重なっているノードを取得
	std::vector<KdNode*> overlapping_nodes = {};
	FindOverlappingNodesRecursively(overlapping_nodes, _root_node, query_params.rect);

	for (auto& node : overlapping_nodes)
	{
		for (auto& collider : node->colliders)
		{
			collider->RespondToMultiAARectTrace(query_result, query_params);
		}
	}

	query_result.has_hit = !query_result.hit_colliders.empty();
}

bool CollisionManager::ShouldSplitNode(const KdNode* const node)
{
	return node->GetCollidersNum() >= MIN_COLLIDERS_PER_NODE && node->depth < MAX_TREE_DEPTH;
}

void CollisionManager::AddColliderToNode(ColliderBase* const collider, KdNode* const node)
{
	node->AddCollider(collider);

	//マッピング情報を登録
	collider_node_map[collider] = node;
}

void CollisionManager::RemoveFromBelongingNode(ColliderBase* collider)
{
	// ツリーが破壊済みであれば何もする必要がない
	if (!IsTreeConstructed())
	{
		return;
	}

	KdNode* const belonging_node = collider_node_map.at(collider);
	collider_node_map.erase(collider);
	belonging_node->EraseCollider(collider);
}

void CollisionManager::RemoveFromBelongingNode(std::vector<ColliderBase*>::const_iterator& it_collider)
{
	if (!IsTreeConstructed())
	{
		return;
	}

	KdNode* const belonging_node = collider_node_map.at(*it_collider);
	collider_node_map.erase(*it_collider);
	it_collider = belonging_node->EraseCollider(*it_collider);
}

void CollisionManager::SortCollidersAlongAxis(std::vector<ColliderBase*>& colliders, const Axis axis) const
{
	// axisがX軸の場合、外接AABBの左側のX座標を基準に昇順ソートする (左から右)
	// axisがY軸の場合、外接AABBの上側のY座標を基準に昇順ソートする (上から下)
	std::sort(colliders.begin(), colliders.end(),
		[axis](ColliderBase* collider1, ColliderBase* collider2)
	{
		Vector2D aabb1_left_top, aabb2_left_top, dummy;
		collider1->GetAABB(aabb1_left_top, dummy);
		collider2->GetAABB(aabb2_left_top, dummy);
		if (axis == Axis::X)
		{
			return aabb1_left_top.x < aabb2_left_top.x;
		}
		
		if (axis == Axis::Y)
		{
			return aabb1_left_top.y < aabb2_left_top.y;
		}
	});
}
