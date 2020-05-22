//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODELINE_H_
#define _CSCENENODELINE_H_
#include "CSceneNode.h"
#include <vector>
using std::vector;

/**
* @brief ���߻��Ƴ����ڵ�
*/
class AVATAR_EXPORT CSceneNodeLine: public CSceneNode {
public:
	//! ���췽��
	CSceneNodeLine(const string& name, const CColor& color, float width);

	//! ��ʼ�������ڵ�
	virtual bool Init();
	//! ���ٳ����ڵ�
	virtual void Destroy();
	//! ��Ⱦ�����ڵ�
	virtual void Render();
	//! ���³����ڵ�
	virtual void Update(float dt);

	//! ��Ӷ���
	void AddPoint(const CVector3& position);
	//! ��ն���
	void ClearPoint();
	//! ʹ�� B ��������ƽ��
	void SmoothLine(float ds);
	//! ����ʾ����
	void ShowPoints(bool show, float pointSize);
	//! ������Ȳ���
	void DisableDepth(bool disable);
	//! ����Ļ�������
	void ScreenSpace(bool enable);
	//! �߶�ģʽ
	void Segment(bool enable);
	//! �����Զ�����ɫ��
	void SetShader(const string& shader);
	//! ��ȡ�������
	int GetPointCount();
	//! ��ȡָ������
	CVertex* GetPoint(unsigned int index);

private:
	//! ���� B �������߲�ֵ
	void BSpline(const CVector3 pt[4], int divisions, vector<CVector3>& spline);

private:
	//! Ĭ����ɫ
	CColor m_cColor;
	//! ����ʾ���
	float m_fLineWidth;
	//! ����ʾ��С
	float m_fPointSize;
	//! �Ƿ����ʾ����
	bool m_bShowPoints;
	//! �Ƿ������Ȳ���
	bool m_bDisableDepth;
	//! ��Ļ����ϵ
	bool m_bScreenSpace;
	//! �߶�ģʽ
	bool m_bSegmentMode;
	//! �Զ�����ɫ��
	CShader* m_pShader;
	//! ��������
	vector<CVertex> m_vecVertices;
};

#endif