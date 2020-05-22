//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CDYNAMICBVT_H_
#define _CDYNAMICBVT_H_
#include "CBoundingBox.h"
#include <vector>
using std::vector;

/**
* @brief ��̬��Χ����
*
* �ο� Bullet Դ�� btDbvt �е� CollideTT ����
*/
class CDynamicBvt {
public:
	//! �ص�����
	struct SCollidePair {
		//! �û�����A
		void* userData1;
		//! �û�����B
		void* userData2;
	};

public:
	//! Ĭ�Ϲ��캯��
	CDynamicBvt();
	//! Ĭ����������
	~CDynamicBvt();

	//! ����ڵ�
	void Insert(void* data, const CBoundingBox& volume);
	//! ɾ��ָ���ڵ�
	void Delete(void* data);
	//! ʹ���µİ�Χ�и��½ڵ�
	void Update(void* data, const CBoundingBox& volume);
	//! ɾ�����нڵ�
	void Clear();

	//! ��ȡ����������
	int GetMaxDepth();
	//! ��ȡ��Χ���ص������ݶ�
	int CollisionPair(vector<SCollidePair>& collidePair);

private:
	//! �ڵ㶨��
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

	//! �ǵݹ������ջԪ�ض���
	typedef struct _SNodePair {
		const SDbvtNode* a;
		const SDbvtNode* b;
		_SNodePair(): a(0), b(0) {}
		_SNodePair(const SDbvtNode* na, const SDbvtNode* nb): a(na), b(nb) {}
	} SNodePair;

private:
	//! ����������Χ�����ĵĽӽ���
	inline float Proximity(const CBoundingBox& a, const CBoundingBox& b);
	//! �ݹ��ȡ�ڵ��������
	int GetMaxDepth(SDbvtNode* node, int depth);
	//! �ݹ��ȡ����ָ�����ݵ�Ҷ�ڵ�
	SDbvtNode* GetLeaf(SDbvtNode* root, void* data);
	//! �ݹ�ɾ��ָ���ڵ�
	void DeleteNode(SDbvtNode* node);

private:
	//! ���ڵ�
	SDbvtNode* m_pRoot;
	//! Ҷ�ڵ����
	int m_iLeavesCount;
	//! ��ײ���ǵݹ����ջ
	vector<SNodePair> m_vecStack;
};

#endif