#pragma once

#include "Core.h"
#include "Component/Collider/HitResult.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class ColliderBase;
class Actor;
struct KdNode;

enum class Axis { X, Y };

/// <summary>
/// コリジョン管理クラス
/// <para>コリジョンの検出, 衝突判定を行う</para>
/// </summary>
class CollisionManager
{
public:
	// k-dツリーの木の最大深さ. ルートは深さ0.
	static constexpr int MAX_TREE_DEPTH = 4;

	// k-dツリーのノードの数の最大値. コライダー数が少ない場合はこの数に満たないこともある.
	static constexpr int MAX_NUM_NODES = (1 << (MAX_TREE_DEPTH + 1)) - 1;

	// ノードを分割するのに最低限必要なコライダー数. 深さが最大に達していなくても所属コライダー数がこの数を下回ったらそれ以上分割しない.
	static constexpr const int MIN_COLLIDERS_PER_NODE = 4;

	static CollisionManager& GetInstance()
	{
		static CollisionManager instance;
		return instance;
	}
private:
	CollisionManager()
		: _root_node(nullptr)
	{}
	~CollisionManager() {}

	// コピーとムーブの禁止
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;
	CollisionManager(CollisionManager&&) = delete;
	CollisionManager& operator=(CollisionManager&&) = delete;

public:
	void Initialize();

	/// <summary>
	/// ツリーが構築済みの場合は破壊し, コライダーの参照を破棄する
	/// </summary>
	void Finalize();

	/// <summary>
	/// 領域を指定してKDツリーを構築する. 領域外のコライダーはルートノードに属すると判定される
	/// </summary>
	/// <param name="area_left_top">KD分割する領域の左上座標</param>
	/// <param name="area_right_bottom">KD分割する領域の右下座標</param>
	void ConstructKdTree(const Vector2D& area_left_top, const Vector2D& area_right_bottom);

	// kdツリーの破壊
	void DestructKdTree();

	/// <summary>
	/// 現在と同じ領域でKDツリーを再構築する.
	/// </summary>
	void ReConstructKdTree();

	/// <summary>
	/// 領域を変更してKDツリーを再構築する.
	/// </summary>
	/// <param name="area_left_top"></param>
	/// <param name="area_right_bottom"></param>
	void ReConstructKdTree(const Vector2D& new_area_left_top, const Vector2D& new_area_right_bottom);

	/// <summary>
	/// ツリーが構築済みか否か
	/// </summary>
	/// <returns></returns>
	bool IsTreeConstructed() const { return _root_node; }

	/// <summary>
	/// 衝突判定と, 衝突結果の処理. シーンのTickから呼ぶ.
	/// </summary>
	void HandleCollisions();

	/// <summary>
	/// コライダーが生成されたときに行う処理.
	/// <para>生成されたコライダーへの参照を保存し, コライダーが所属するノードを検索する</para>
	/// </summary>
	/// <param name="new_collider"></param>
	void OnNewColliderInitialized(ColliderBase* new_collider);

	/// <summary>
	/// コライダーをノードから削除し, collider_node_mapのマッピング情報も削除する
	/// </summary>
	/// <param name="collider">削除対象</param>
	void OnColliderFinalize(ColliderBase* const collider);

	/// <summary>
	/// コライダーが回転・移動した際に呼ばれる.
	/// 所属中のノードが最適なノードでない場合, 所属ノードを再検索する.
	/// </summary>
	/// <param name="collider">回転・移動したコライダー</param>
	void OnColliderTransformed(ColliderBase* collider);

	void DrawPartition(const KdNode* node, const CameraParams& camera_params, const int line_color) const;

	/// <summary>
	/// 指定された深さの各ノードのセルを描画する
	/// </summary>
	/// <param name="camera_params"></param>
	/// <param name="nodes_depth">描画するノードの深さ</param>
	void DrawCells(const CameraParams& camera_params, const uint8_t nodes_depth = 0) const;

	/// <summary>
	/// target_colliderと重なっているコライダーを取得する
	/// </summary>
	/// <param name="out_overlapping_colliders">target_colliderと重なっているコライダーリスト</param>
	/// <param name="target_collider">CollisionTypeがOverlapのコライダー</param>
	void GetOverlappingColliders(std::vector<ColliderBase*>& out_overlapping_colliders, ColliderBase* const target_collider) const;

	/// <summary>
	/// 線分と重なっているノードをすべて取得
	/// </summary>
	/// <param name="out_intersected_nodes">結果格納先</param>
	/// <param name="node">対称ノード. このノードが結果に含まれ, 且つ子ノードを持つ場合は, 子ノードについても再帰的に探索する</param>
	/// <param name="segment">線分</param>
	/// <returns></returns>
	void FindOverlappingNodesRecursively(std::vector<KdNode*>& out_intersected_nodes, KdNode* node, const FSegment& segment) const;

	/// <summary>
	/// 軸に平行な矩形と重なっているノードをすべて取得
	/// </summary>
	/// <param name="out_intersected_nodes">結果格納先</param>
	/// <param name="node">対称ノード. このノードが結果に含まれ, 且つ子ノードを持つ場合は, 子ノードについても再帰的に探索する</param>
	/// <param name="rect">軸に平行な矩形</param>
	void FindOverlappingNodesRecursively(std::vector<KdNode*>& out_intersected_nodes, KdNode* node, const FRectAA& rect) const;

	/// <summary>
	/// 線分がノードのエリアに重なっているか
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="segment">線分</param>
	bool IsOverlapping(const KdNode* const node, const FSegment& segment) const;

	/// <summary>
	/// 軸に平行な矩形がノードのエリアに重なっているか
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="rect">軸に平行な矩形</param>
	bool IsOverlapping(const KdNode* const node, const FRectAA& rect) const;

	int DEBUG_GetNodeIndex(ColliderBase* collider);

	//クエリ系
	void SingleLineTrace(QueryResult_SingleLineTrace& out_query_result, const CollisionQueryParams_SingleLineTrace& query_params);
	void MultiAARectTrace(QueryResult_MultiAARectTrace& out_query_result, const CollisionQueryParams_RectAA& query_params = CollisionQueryParams_RectAA{});


private:

	/// <summary>
	/// ノードが分割条件を満たすか
	/// <para>分割条件: ノードの深さがMAX_TREE_DEPTH未満 かつ 所属コライダー数がMIN_COLLIDERS_PER_NODE以上</para>
	/// </summary>
	static bool ShouldSplitNode(const KdNode* const node);

	/// <summary>
	/// ノードに所属コライダーを追加する
	/// </summary>
	void AddColliderToNode(ColliderBase* const collider, KdNode* const node);

	/// <summary>
	/// kdノードを再帰的に分割する.
	/// <para>
	/// 分割は ノードの深さがMAX_TREE_DEPTHに達する または ノードに属するコライダー数がMIN_COLLIDERS_PER_NODE以下になる まで行われる
	/// </para>
	/// </summary>
	/// <param name="parent_node">分割のルートノード</param>
	void SplitKdNodeRecursively(KdNode* parent_node);

	/// <summary>
	/// 左右の子ノードに親ノードのコライダーを移動させる.
	/// どちらの子ノードにも包含されないコライダーは親ノードに残る
	/// </summary>
	void DistributeColliders(KdNode* const parent_node);

	/// <summary>
	/// 破壊ルートノードの子孫ノードをdeleteした後, 破壊ルートノードをdeleteする DestructKdTree()から呼ばれる
	/// </summary>
	/// <param name="destruction_root_node">破壊ルートノード</param>
	void DestroyKdNodesRecursively(KdNode*& destruction_root_node);
	
	/// <summary>
	/// 1つの固定ノードと、別のノード及びその子孫ノードの間で, 属するコライダーの全組で衝突判定を行う.
	/// </summary>
	/// <param name="fixed_node">固定ノード</param>
	/// <param name="root_of_targets">衝突判定の対象となるノードのルート. このノード自体と, その子孫ノードが固定ノードとの衝突判定の対象となる。</param>
	void HandleCollisionsBetweenNodes(const KdNode* fixed_node, const KdNode* root_of_targets);
	
	/// <summary>
	/// コライダーがノードに包含されるかをチェックする
	/// </summary>
	/// <param name="collider">コライダー</param>
	/// <param name="node">ノード</param>
	/// <returns>nodeがcolliderを包含するか</returns>
	bool CheckNodeContainsCollider(ColliderBase* const collider, const KdNode* const node) const;

	/// <summary>
	/// コライダーが所属する最適なノードを検索する
	/// <para>*最適なノードは, コライダーを包含するノードの中で最も深いもの</para>
	/// </summary>
	/// <param name="collider">検索対象コライダー</param>
	/// <returns>コライダーが属する最適なノード</returns>
	KdNode* FindNode(ColliderBase* collider) const;

	/// <summary>
	/// クエリポイントが所属する最も深いノードを検索する
	/// </summary>
	/// <param name="query_point"></param>
	/// <returns></returns>
	KdNode* FindNode(const Vector2D& query_point) const;

	/// <summary>
	/// ノードからコライダーを削除
	/// </summary>
	/// <param name="collider">削除対象</param>
	void RemoveFromBelongingNode(ColliderBase* collider);

	/// <summary>
	/// ノードからコライダーを削除
	/// </summary>
	/// <param name="it_collider">削除対象を指すイテレータ. 関数内で削除した次のコライダーを指すように更新される</param>
	void RemoveFromBelongingNode(std::vector<ColliderBase*>::const_iterator& it_collider);

	/// <summary>
	/// <para>axisがX軸の場合、外接AABBの左側のX座標が昇順(左から右)になるようにソートする</para>
	/// <para>axisがY軸の場合、外接AABBの上側のY座標が昇順(上から下)になるようにソートする</para>
	/// </summary>
	/// <param name="colliders">ソート対象のコライダーリスト</param>
	/// <param name="axis">ソートの基準となる軸</param>
	void SortCollidersAlongAxis(std::vector<ColliderBase*>& colliders, const Axis axis) const;

	/// <summary>
	/// KDノードのリスト
	/// <para>親ノードのインデックスをi_parentとすると</para>
	/// <para>左子ノード: 2*i_parent + 1</para>
	/// <para>右子ノード: 2*i_parent + 2</para>
	/// </summary>
	std::vector<KdNode*> kd_nodes;

	/// <summary>
	/// KDツリーのルートノード
	/// </summary>
	KdNode* _root_node;

	/// <summary>
	/// コライダーとKDノードのマッピング情報
	/// </summary>
	std::unordered_map<ColliderBase*, KdNode*> collider_node_map;

	/// <summary>
	///	存在するすべてのコライダー
	/// </summary>
	std::vector<ColliderBase*> all_colliders;
};

/// <summary>
/// 左子ノードは分割された親ノードの X(Y)が小さい側で, 右子ノードは大きい側
/// <para>コライダーのAABBが左右子ノードのエリアいずれかに内包される場合, そのコライダーは子ノードに所属する</para>
/// <para>AABBが子ノードの境界線に接触する場合は, 親ノードに所属する</para>
/// <para>あるコライダーが衝突し得るのは, 所属するノードと, その子孫および祖先ノード に所属するコライダー</para>
/// </summary>
struct KdNode
{
	/// <summary>
	/// 
	/// </summary>
	/// <param name="depth_in">ノードの深さ. ルートは0</param>
	/// <param name="node_index_in">ノード番号. 左子は2*[親インデックス]+1 or 右子は2*[親インデックス]+2</param>
	/// <param name="split_axis_in">分割軸</param>
	/// <param name="left_top">左上の座標</param>
	/// <param name="right_bottom">右下の座標</param>
	KdNode(const uint8_t depth_in, const uint8_t node_index_in, Axis split_axis_in, Vector2D left_top_in, Vector2D right_bottom_in)
		: left_node(nullptr)
		, right_node(nullptr)
		, depth(depth_in)
		, node_index(node_index_in)
		, split_axis(split_axis_in)
		, cell_left_top(left_top_in)
		, cell_right_bottom(right_bottom_in)
		, split_position(0.f)
	{
	}

	std::vector<ColliderBase*>& GetCollidersInNode() { return colliders; }
	void AddCollider(ColliderBase* collider) { colliders.push_back(collider); }

	/// <summary>
	/// コライダーリストからコライダーを削除する
	/// </summary>
	/// <param name="collider">削除対象コライダー</param>
	/// <returns>削除したコライダーの次のコライダーのイテレータ</returns>
	std::vector<ColliderBase*>::iterator EraseCollider(ColliderBase* collider) {
		auto iterator = std::find(colliders.begin(), colliders.end(), collider);
		if (iterator != colliders.end())
		{
			return colliders.erase(iterator);
		}
	}
	uint8_t GetNodeIndex() const { return node_index; }
	bool HasChildNode() const { return left_node && right_node; }
	uint8_t GetLeftChildIndex() const { return 2 * node_index + 1; }
	KdNode* const GetLeftChildNode() const { return left_node; }
	uint8_t GetRightChildIndex() const { return 2 * node_index + 2; }
	KdNode* const GetRightChildNode() const { return right_node; }

	/// <summary>
	///  ノードに所属するコライダー数を取得
	/// </summary>
	size_t GetCollidersNum() const { return colliders.size(); }

	Vector2D GetAreaLeftTop() const { return cell_left_top; }
	Vector2D GetAreaRightBottom() const { return cell_right_bottom; }

	std::vector<ColliderBase*> colliders;
	KdNode* left_node;
	KdNode* right_node;
	uint8_t depth;
	uint8_t node_index;
	Vector2D cell_left_top;
	Vector2D cell_right_bottom;
	Axis split_axis;
	float split_position;
};