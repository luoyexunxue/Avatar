//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDynamicBvt.h"
#include <cmath>

/**
* 构造函数
*/
CDynamicBvt::CDynamicBvt() {
	m_pRoot = 0;
	m_iLeavesCount = 0;
}

/**
* 析构函数
*/
CDynamicBvt::~CDynamicBvt() {
	Clear();
}

/**
* 插入节点
*/
void CDynamicBvt::Insert(void* data, const CBoundingBox& volume) {
	// 创建节点
	SDbvtNode* node = new SDbvtNode(data, volume);
	node->children[0]	= 0;
	node->children[1]	= 0;
	m_iLeavesCount++;

	// 根节点为空，直接插入根节点
	if (!m_pRoot) {
		m_pRoot	= node;
		m_pRoot->parent = 0;
		return;
	}
	// 选择中心距离最近的叶节点
	SDbvtNode* sibling = m_pRoot;
	while (sibling->IsInternal()) {
		float distance1 = Proximity(volume, sibling->children[0]->volume);
		float distance2 = Proximity(volume, sibling->children[1]->volume);
		sibling = sibling->children[distance1 < distance2? 0: 1];
	}
	// 生成新的父节点，节点包围盒大小为新节点和邻节点之和
	SDbvtNode* parent = new SDbvtNode(0, node->volume + sibling->volume);
	parent->parent = sibling->parent;

	// 设置父节点子节点指针
	if (parent->parent) {
		parent->parent->children[sibling->ChildIndex()] =	parent;
		parent->children[0] = sibling; sibling->parent = parent;
		parent->children[1] = node; node->parent = parent;
		// 向上传导更新包围体
		while (parent->parent && !parent->parent->volume.IsContain(parent->volume)) {
			parent = parent->parent;
			parent->volume = parent->children[0]->volume + parent->children[1]->volume;
		}
	} else {
		m_pRoot = parent;
		parent->children[0] = sibling; sibling->parent = parent;
		parent->children[1] = node; node->parent = parent;
	}
}

/**
* 删除指定节点
*/
void CDynamicBvt::Delete(void* data) {
	// 查找叶节点
	SDbvtNode* leaf = GetLeaf(m_pRoot, data);
	if (!leaf) return;
	m_iLeavesCount--;

	// 叶节点为根节点情况
	if (leaf == m_pRoot) {
		m_pRoot = 0;
		delete leaf;
		return;
	}
	SDbvtNode* parent = leaf->parent;
	SDbvtNode* prev = parent->parent;
	SDbvtNode* sibling = parent->children[1 - leaf->ChildIndex()];
	if (prev) {
		prev->children[parent->ChildIndex()] = sibling;
		sibling->parent = prev;
		delete parent;
		// 向上传导更新包围体
		while (prev) {
			const CBoundingBox aabb = prev->children[0]->volume + prev->children[1]->volume;
			if (aabb == prev->volume) break;
			prev->volume = aabb;
			prev = prev->parent;
		}
	} else {
		m_pRoot = sibling;
		sibling->parent = 0;
		delete parent;
	}
	delete leaf;
}

/**
* 更新包围盒树
* 实质是先删除叶节点，然后在插入叶节点
*/
void CDynamicBvt::Update(void* data, const CBoundingBox& volume) {
	SDbvtNode* leaf = GetLeaf(m_pRoot, data);
	if (!leaf || leaf == m_pRoot) return;
	if (leaf->volume.IsContain(volume)) return;

	leaf->volume = volume;
	SDbvtNode* parent = leaf->parent;
	SDbvtNode* prev = parent->parent;
	SDbvtNode* sibling = parent->children[1 - leaf->ChildIndex()];
	if (prev) {
		prev->children[parent->ChildIndex()] = sibling;
		sibling->parent = prev;
		// 向上传导包围盒变化
		while (prev) {
			const CBoundingBox aabb = prev->children[0]->volume + prev->children[1]->volume;
			if (aabb == prev->volume) break;
			prev->volume = aabb;
			prev = prev->parent;
		}
		sibling = prev ? prev: m_pRoot;
	} else {
		m_pRoot = sibling;
		sibling->parent = 0;
	}
	// 选择距离最近的叶节点
	while (sibling->IsInternal()) {
		float distance1 = Proximity(volume, sibling->children[0]->volume);
		float distance2 = Proximity(volume, sibling->children[1]->volume);
		sibling = sibling->children[distance1 < distance2? 0: 1];
	}
	// 生成新的父节点
	parent->parent = sibling->parent;
	parent->volume = leaf->volume + sibling->volume;
	parent->data = 0;

	// 设置父节点子节点指针
	if (parent->parent) {
		parent->parent->children[sibling->ChildIndex()] =	parent;
		parent->children[0] = sibling; sibling->parent = parent;
		parent->children[1] = leaf; leaf->parent = parent;
		// 更新根节点包围体大小
		while (parent->parent && !parent->parent->volume.IsContain(parent->volume)) {
			parent = parent->parent;
			parent->volume = parent->children[0]->volume + parent->children[1]->volume;
		}
	} else {
		m_pRoot = parent;
		parent->children[0] = sibling; sibling->parent = parent;
		parent->children[1] = leaf; leaf->parent = parent;
	}
}

/**
* 删除所有节点
*/
void CDynamicBvt::Clear() {
	if (m_pRoot) {
		DeleteNode(m_pRoot);
		m_iLeavesCount = 0;
	}
}

/**
* 获取树的最大深度
*/
int CDynamicBvt::GetMaxDepth() {
	if (m_pRoot) return GetMaxDepth(m_pRoot, 1);
	return 0;
}

/**
* 获取包围盒重叠的数据对
*/
int CDynamicBvt::CollisionPair(vector<SCollidePair>& collidePair) {
	collidePair.clear();
	// 节点数低于两个
	if (m_iLeavesCount < 2) return 0;
	m_vecStack.reserve(m_iLeavesCount * 2 - 1);
	m_vecStack.resize(0);
	m_vecStack.push_back(SNodePair(m_pRoot, m_pRoot));

	// 遍历包围层次树
	while (m_vecStack.size()) {
		SNodePair element = m_vecStack.back();
		m_vecStack.pop_back();
		if (element.a == element.b) {
			if (element.a->IsInternal()) {
				m_vecStack.push_back(SNodePair(element.a->children[0], element.a->children[0]));
				m_vecStack.push_back(SNodePair(element.a->children[1], element.a->children[1]));
				m_vecStack.push_back(SNodePair(element.a->children[0], element.a->children[1]));
			}
			continue;
		}
		if (element.a->volume.IsOverlap(element.b->volume)) {
			if (element.a->IsInternal()) {
				if (element.b->IsInternal()) {
					m_vecStack.push_back(SNodePair(element.a->children[0], element.b->children[0]));
					m_vecStack.push_back(SNodePair(element.a->children[1], element.b->children[0]));
					m_vecStack.push_back(SNodePair(element.a->children[0], element.b->children[1]));
					m_vecStack.push_back(SNodePair(element.a->children[1], element.b->children[1]));
				} else {
					m_vecStack.push_back(SNodePair(element.a->children[0], element.b));
					m_vecStack.push_back(SNodePair(element.a->children[1], element.b));
				}
			} else {
				if (element.b->IsInternal()) {
					m_vecStack.push_back(SNodePair(element.a, element.b->children[0]));
					m_vecStack.push_back(SNodePair(element.a, element.b->children[1]));
				} else {
					SCollidePair pair;
					pair.userData1 = const_cast<void*>(element.a->data);
					pair.userData2 = const_cast<void*>(element.b->data);
					collidePair.push_back(pair);
				}
			}
		}
	}
	return collidePair.size();
}

/**
* 计算两个包围盒中心的接近度，曼哈顿距离 * 2
*/
float CDynamicBvt::Proximity(const CBoundingBox& a, const CBoundingBox& b) {
	float dx = a.m_cMin.m_fValue[0] + a.m_cMax.m_fValue[0] - b.m_cMin.m_fValue[0] - b.m_cMax.m_fValue[0];
	float dy = a.m_cMin.m_fValue[1] + a.m_cMax.m_fValue[1] - b.m_cMin.m_fValue[1] - b.m_cMax.m_fValue[1];
	float dz = a.m_cMin.m_fValue[2] + a.m_cMax.m_fValue[2] - b.m_cMin.m_fValue[2] - b.m_cMax.m_fValue[2];
	return fabs(dx) + fabs(dy)+ fabs(dz);
}

/**
* 获取节点的最大深度
*/
int CDynamicBvt::GetMaxDepth(SDbvtNode* node, int depth) {
	if (node->IsInternal()) {
		int depth1 = GetMaxDepth(node->children[0], depth + 1);
		int depth2 = GetMaxDepth(node->children[1], depth + 1);
		depth = depth2 > depth1? depth2: depth1;
	}
	return depth;
}

/**
* 获取包含指定数据的叶节点
*/
CDynamicBvt::SDbvtNode* CDynamicBvt::GetLeaf(SDbvtNode* root, void* data) {
	if (!root) return 0;
	if (root->IsInternal()) {
		SDbvtNode* leaf1 = GetLeaf(root->children[0], data);
		if (leaf1) return leaf1;
		SDbvtNode* leaf2 = GetLeaf(root->children[1], data);
		if (leaf2) return leaf2;
	} else if (root->data == data) {
		return root;
	}
	return 0;
}

/**
* 递归删除指定节点
*/
void CDynamicBvt::DeleteNode(SDbvtNode* node) {
	if (node->IsInternal()) {
		DeleteNode(node->children[0]);
		DeleteNode(node->children[1]);
		node->children[0] = 0;
		node->children[1] = 0;
	}
	if (node == m_pRoot) m_pRoot = 0;
	delete node;
}