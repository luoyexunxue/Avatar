//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODELINE_H_
#define _CSCENENODELINE_H_
#include "CSceneNode.h"
#include <vector>
using std::vector;

/**
* @brief 点线绘制场景节点
*/
class AVATAR_EXPORT CSceneNodeLine: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeLine(const string& name, const CColor& color, float width);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);

	//! 添加顶点
	void AddPoint(const CVector3& position);
	//! 清空顶点
	void ClearPoint();
	//! 使用 B 样条曲线平滑
	void SmoothLine(float ds);
	//! 仅显示顶点
	void ShowPoints(bool show, float pointSize);
	//! 禁用深度测试
	void DisableDepth(bool disable);
	//! 以屏幕坐标绘制
	void ScreenSpace(bool enable);
	//! 线段模式
	void Segment(bool enable);
	//! 设置自定义着色器
	void SetShader(const string& shader);
	//! 获取顶点个数
	int GetPointCount();
	//! 获取指定顶点
	CVertex* GetPoint(unsigned int index);

private:
	//! 三次 B 样条曲线插值
	void BSpline(const CVector3 pt[4], int divisions, vector<CVector3>& spline);

private:
	//! 默认颜色
	CColor m_cColor;
	//! 线显示宽度
	float m_fLineWidth;
	//! 点显示大小
	float m_fPointSize;
	//! 是否仅显示顶点
	bool m_bShowPoints;
	//! 是否禁用深度测试
	bool m_bDisableDepth;
	//! 屏幕坐标系
	bool m_bScreenSpace;
	//! 线段模式
	bool m_bSegmentMode;
	//! 自定义着色器
	CShader* m_pShader;
	//! 顶点数组
	vector<CVertex> m_vecVertices;
};

#endif