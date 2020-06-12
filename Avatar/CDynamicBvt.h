//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDYNAMICBVT_H_
#define _CDYNAMICBVT_H_
#include "CBoundingBox.h"
#include <vector>
using std::vector;

/**
* @brief 动态包围盒树
*
* 参考 Bullet 源码 btDbvt 中的 CollideTT 方法
*/
class CDynamicBvt {
public:
	//! 重叠数据
	struct SCollidePair {
		//! 用户数据A
		void* userData1;
		//! 用户数据B
		void* userData2;
	};

public:
	//! 默认构造函数
	CDynamicBvt();
	//! 默认析构函数
	~CDynamicBvt();

	//! 插入节点
	void Insert(void* data, const CBoundingBox& volume);
	//! 删除指定节点
	void Delete(void* data);
	//! 使用新的包围盒更新节点
	void Update(void* data, const CBoundingBox& volume);
	//! 删除所有节点
	void Clear();

	//! 获取树的最大深度
	int GetMaxDepth();
	//! 获取包围盒重叠的数据对
	int CollisionPair(vector<SCollidePair>& collidePair);

private:
	//! 节点定义
	typedef struct _SDbvtNode {
		const void* data;
		CBoundingBox volume;
		_SDbvtNode* parent;
		_SDbvtNode* children[2];
		_SDbvtNode(const void* d, const CBoundingBox& v): data(d), volume(v) {}
		inline bool	IsLeaf() const { return children[0] == 0; }
		inline bool	IsInternal() const { return children[0] != 0; }
		inline int ChildIndex() const { return parent->children[0] == this ? 0: 1; }
	} SDbvtNode;

	//! 非递归遍历树栈元素定义
	typedef struct _SNodePair {
		const SDbvtNode* a;
		const SDbvtNode* b;
		_SNodePair(): a(0), b(0) {}
		_SNodePair(const SDbvtNode* na, const SDbvtNode* nb): a(na), b(nb) {}
	} SNodePair;

private:
	//! 计算两个包围盒中心的接近度
	inline float Proximity(const CBoundingBox& a, const CBoundingBox& b);
	//! 递归获取节点的最大深度
	int GetMaxDepth(SDbvtNode* node, int depth);
	//! 递归获取包含指定数据的叶节点
	SDbvtNode* GetLeaf(SDbvtNode* root, void* data);
	//! 递归删除指定节点
	void DeleteNode(SDbvtNode* node);

private:
	//! 根节点
	SDbvtNode* m_pRoot;
	//! 叶节点个数
	int m_iLeavesCount;
	//! 碰撞检测非递归遍历栈
	vector<SNodePair> m_vecStack;
};

#endif