//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CGRAPHICSMANAGER_H_
#define _CGRAPHICSMANAGER_H_
#include "AvatarConfig.h"
#include "CCamera.h"
#include "CVector3.h"
#include "CVertex.h"
#include "CColor.h"
#include "CRay.h"
#include "CBoundingBox.h"
#include "CTexture.h"
#include "CFileManager.h"
#include <string>
using std::string;

/**
* @brief 图形渲染管理器
*/
class AVATAR_EXPORT CGraphicsManager {
public:
	//! 获取管理器实例
	static CGraphicsManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CGraphicsManager();
		return m_pInstance;
	}
	//! 实例销毁
	void Destroy();

	//! 渲染一帧
	void Render();

	//! 设置相机
	void SetCamera(CCamera* camera);
	//! 获取相机
	CCamera* GetCamera();
	//! 设置窗口大小
	void SetWindowSize(int width, int height);
	//! 获取窗口大小
	void GetWindowSize(int* width, int* height);
	//! 设置光源
	void SetLight(const CVector3& pos, const CVector3& dir, const CVector3& color);
	//! 获取光源位置
	CVector3 GetLight();
	//! 设置是否立体显示
	void SetStereoMode(bool stereo);
	//! 获取是否立体显示
	bool GetStereoMode();
	//! 设置是否显示阴影
	void SetShadowEnable(bool enable);
	//! 获取阴影是否显示
	bool GetShadowEnable();
	//! 设置雾显示及参数
	void SetFogEnable(bool enable, float start, float end, const CColor& color);
	//! 获取雾是否显示
	bool GetFogEnable();
	//! 设置启用环境光照
	void SetEnvironmentMapEnable(bool enable, const string& cubemap);
	//! 获取是否开启环境光照
	bool GetEnvironmentMapEnable();

	//! 设置是否启用深度测试
	void SetDepthTestEnable(bool enable);
	//! 设置是否启用深度偏移
	void SetDepthOffsetEnable(bool enable, float offset);
	//! 设置是否启用颜色混合
	void SetColorBlendEnable(bool enable);
	//! 设置初始背景颜色
	void SetBackgroundColor(const CColor& color);
	//! 设置立体视觉焦距
	void SetFocusDistance(float distance);

	//! 判断是否处于深度渲染模式
	bool IsDepthRender();
	//! 判断是否处于反射图渲染
	bool IsReflectRender();
	//! 判断是否处于左眼渲染
	bool IsLeftEyeRender();

	//! 屏幕截图
	void Screenshot(CFileManager::CImageFile& file, bool redraw);
	//! 屏幕截图并保存至文件
	void Screenshot(const string& fileName, bool redraw);
	//! 获取点的屏幕坐标
	void PointToScreen(const CVector3& point, CVector3& screen);
	//! 计算拾取射线
	void PickingRay(int x, int y, CRay& ray);
	//! 获取拾取点坐标
	void PickingPosition(int x, int y, CVector3& position);

	//! 绘制点
	void DrawPoints(const CVertex* vertices, int size);
	//! 绘制线
	void DrawLines(const CVertex* vertices, int size, float width, bool strip);
	//! 绘制三角形
	void DrawTriangles(const CVertex* vertices, int size, bool strip);
	//! 绘制四边形
	void DrawQuadrilateral(const CColor& color, bool flipTexCoord);
	//! 绘制立方体
	void DrawCube(const CColor& color);

	//! 设置当前渲染目标
	void SetRenderTarget(CTexture* texture, int level, bool fullView, bool clearColor, bool clearDepth);
	//! 获取当前渲染目标
	CTexture* GetRenderTarget();

	//! 获取帧率
	float GetFrameRate();

private:
	CGraphicsManager();
	~CGraphicsManager();

	//! 世界坐标系到屏幕坐标系的变换
	void Project(const int viewport[4], const float objCoord[3], float winPos[3]);
	//! 屏幕坐标系到世界坐标系的变换
	void UnProject(const int viewport[4], const float winPos[3], float objCoord[3]);

	//! 绘制场景
	void Draw();
	//! 绘制屏幕元素
	void DrawScreen();
	//! 绘制阴影贴图
	void DrawShadowMap();
	//! 绘制全景图
	void DrawCubeMap();
	//! 渲染反射贴图
	void DrawReflectMap();

private:
	//! 窗口宽高
	int m_iWindowSize[2];
	//! 立体模式
	bool m_bStereoMode;
	//! 阴影显示
	bool m_bShadowEnable;
	//! 雾显示
	bool m_bFogEnable;
	//! 环境光照
	bool m_bEnvironmentMapEnable;
	//! 背景颜色
	CColor m_cBackground;
	//! 全局光源位置
	CVector3 m_cLightPos;
	//! 全局光照方向
	CVector3 m_cLightDir;
	//! 相机
	CCamera* m_pCamera;
	//! 当前渲染目标
	CTexture* m_pRenderTarget;

	//! 渲染左眼（立体模式）
	bool m_bRenderLeftEye;
	//! 深度图渲染
	bool m_bRenderDepth;
	//! 反射图渲染
	bool m_bRenderReflect;
	//! 焦距
	float m_fFocusDistance;
	//! 帧率
	float m_fFrameRate;
	//! 实例
	static CGraphicsManager* m_pInstance;
};

#endif