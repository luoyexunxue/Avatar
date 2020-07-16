//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CGraphicsManager.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CGuiEnvironment.h"
#include "CMatrix4.h"
#include "CVector3.h"
#include "CVector2.h"
#include "CStringUtil.h"
#include "CTimer.h"
#include "CLog.h"
#include <cmath>
#include <cstring>

#ifdef AVATAR_WINDOWS
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_LINUX
#include "thirdparty/glew/glew.h"
#include <GL/gl.h>
#endif
#ifdef AVATAR_ANDROID
#include <GLES3/gl3.h>
#define glClearDepth glClearDepthf
#endif

/**
* 构造函数
*/
CGraphicsManager::CGraphicsManager() {
	// OPENGL 初始状态
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
	// 清空颜色和深度
	glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	// 初始化内部参数
	m_iWindowSize[0] = 0;
	m_iWindowSize[1] = 0;
	m_bStereoMode = false;
	m_bShadowEnable = false;
	m_bFogEnable = false;
	m_bEnvironmentMapEnable = false;
	m_cBackground.SetValue(0.05f, 0.05f, 0.1f);
	m_pCamera = 0;
	m_pRenderTarget = 0;
	m_bRenderLeftEye = true;
	m_bRenderDepth = false;
	m_bRenderReflect = false;
	m_fFocusDistance = 2.0f;
	m_fFrameRate = 0.0f;
}

/**
* 析构函数
*/
CGraphicsManager::~CGraphicsManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CGraphicsManager* CGraphicsManager::m_pInstance = 0;

/**
* 实例销毁
*/
void CGraphicsManager::Destroy() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	pTextureMgr->Drop(pTextureMgr->GetTexture("__shadowmap__"));
	pTextureMgr->Drop(pTextureMgr->GetTexture("__shadowmap_blur__"));
	pTextureMgr->Drop(pTextureMgr->GetTexture("__irradiancemap__"));
	pTextureMgr->Drop(pTextureMgr->GetTexture("__environmentmap__"));
	pTextureMgr->Drop(pTextureMgr->GetTexture("__integrationmap__"));
	if (m_pCamera) delete m_pCamera;
	delete this;
}

/**
* 渲染一帧
*/
void CGraphicsManager::Render() {
	// 清除场景颜色和深度
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 绘制整个场景
	Draw();
	// 更新帧率信息
	float interval = CTimer::Reset("__graphicsmanager__");
	if (interval > 0.0f) {
		m_fFrameRate = 0.3f * (1.0f / interval) + 0.7f * m_fFrameRate;
	}
}

/**
* 设置相机
* @param camera 与图形渲染关联的相机
*/
void CGraphicsManager::SetCamera(CCamera* camera) {
	m_pCamera = camera;
	m_pCamera->Control(true);
}

/**
* 获取相机
* @return 与图形渲染关联的相机
*/
CCamera* CGraphicsManager::GetCamera() {
	return m_pCamera;
}

/**
* 设置窗口大小
* @param width 绘制窗口宽度
* @param height 绘制窗口高度
*/
void CGraphicsManager::SetWindowSize(int width, int height) {
	m_iWindowSize[0] = width;
	m_iWindowSize[1] = height;
	// 获取屏幕渲染大小
	int screenWidth = m_iWindowSize[0];
	int screenHeight = m_iWindowSize[1];
	if (m_pRenderTarget) {
		screenWidth = m_pRenderTarget->GetWidth();
		screenHeight = m_pRenderTarget->GetHeight();
	}
	CGuiEnvironment::GetInstance()->SetSize(screenWidth, screenHeight);
}

/**
* 获取窗口大小
* @param width 输出绘制窗口宽度
* @param height 输出绘制窗口高度
*/
void CGraphicsManager::GetWindowSize(int* width, int* height) {
	*width = m_iWindowSize[0];
	*height = m_iWindowSize[1];
}

/**
* 设置光源
* @param position 光源位置，当 position.w 为 0 时，为方向光，否则为点光源或聚光灯
* @param direction 光照方向，当 direction.w 大于 0 时，为聚光灯且 direction.w 为聚光角度范围
* @param color 光照颜色，当为点光源或聚光灯光源时，color.a 为光照范围
* @note 系统通过参数 position 和 direction 自动判断光源类型，支持平行光光源、点光源、聚光灯光源三种
*/
void CGraphicsManager::SetLight(const CVector3& position, const CVector3& direction, const CVector3& color) {
	// 单位化相关参数
	CVector3 normPos(position.m_fValue, 0.0f);
	CVector3 normDir(direction.m_fValue, 0.0f);
	normPos.Normalize();
	normDir.Normalize();
	// 根据光源类型对参数进行修正
	if (position.m_fValue[3] == 0.0f) {
		m_cLightPos.SetValue(normPos);
		m_cLightDir.SetValue(normPos);
		normDir.SetValue(normPos);
	} else {
		m_cLightPos.SetValue(position);
		m_cLightDir.SetValue(normDir.m_fValue, direction.m_fValue[3]);
		if (direction.m_fValue[3] > 0.0f) {
			normDir[3] = std::max(cosf(direction.m_fValue[3] * 0.5f), 0.0f);
		}
	}
	// 对每个含有 uLightPos 的着色器进行统一变量赋值
	vector<CShader*> shaderList;
	CEngine::GetShaderManager()->GetShaderList(shaderList);
	for (size_t i = 0; i < shaderList.size(); i++) {
		CShader* pShader = shaderList[i];
		if (pShader->IsUniform("uLightPos")) {
			pShader->UseShader();
			// 星球渲染必须使用方向光
			const string& shaderName = pShader->GetName();
			if (shaderName == "planetsky" || shaderName == "planetground") {
				pShader->SetUniform("uLightPos", normPos.m_fValue, 4, 1);
				continue;
			}
			pShader->SetUniform("uLightPos", m_cLightPos.m_fValue, 4, 1);
			pShader->SetUniform("uLightDir", normDir.m_fValue, 4, 1);
			pShader->SetUniform("uLightColor", color.m_fValue, 4, 1);
		}
	}
}

/**
* 获取光源位置
* @param position 输出光源位置
* @param direction 输出光源方向
* @see SetLight
*/
void CGraphicsManager::GetLight(CVector3& position, CVector3& direction) {
	position.SetValue(m_cLightPos);
	direction.SetValue(m_cLightDir);
}

/**
* 设置是否立体显示
* @param stereo 立体渲染模式
* @note 设置立体渲染模式后将以左右分屏方式渲染立体图像
*/
void CGraphicsManager::SetStereoMode(bool stereo) {
	m_bStereoMode = stereo;
	if (!stereo) m_bRenderLeftEye = true;
}

/**
* 获取是否立体显示
* @return 当前是否为立体渲染模式
*/
bool CGraphicsManager::GetStereoMode() {
	return m_bStereoMode;
}

/**
* 设置是否显示阴影
* @param enable 显示阴影
*/
void CGraphicsManager::SetShadowEnable(bool enable) {
	if (enable || m_bShadowEnable) {
		const char* shaderName[] = { "texturelight", "terrain", "light" };
		const int shaderCount = sizeof(shaderName) / sizeof(char*);
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		for (size_t i = 0; i < shaderCount; i++) {
			CShader* pShader = pShaderMgr->GetShader(shaderName[i]);
			pShader->UseShader();
			if (enable) {
				if (!m_bShadowEnable) pShaderMgr->Update(pShader, "ENABLE_SHADOW", "");
				pShader->SetUniform("uShadowMap", 8);
			} else {
				pShaderMgr->Update(pShader, "", "ENABLE_SHADOW");
			}
		}
		m_bShadowEnable = enable;
	}
}

/**
* 获取阴影是否显示
* @return 当前是否显示阴影
*/
bool CGraphicsManager::GetShadowEnable() {
	return m_bShadowEnable;
}

/**
* 设置雾显示及参数
* @param enable 是否显示雾
* @param start 开始距离
* @param end 结束距离
* @param color 颜色
* @note enable 为 false 时，start，end，color 将被忽略
*/
void CGraphicsManager::SetFogEnable(bool enable, float start, float end, const CColor& color) {
	if (enable || m_bFogEnable) {
		const char* shaderName[] = {
			"texture", "texturelight", "water", "terrain", "light", "fresnel", "skybox", "flame"
		};
		const int shaderCount = sizeof(shaderName) / sizeof(char*);
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		CVector3 fogColor(color.m_fValue);
		for (size_t i = 0; i < shaderCount; i++) {
			CShader* pShader = pShaderMgr->GetShader(shaderName[i]);
			pShader->UseShader();
			if (enable) {
				if (!m_bFogEnable) pShaderMgr->Update(pShader, "ENABLE_FOG", "");
				pShader->SetUniform("uFogColor", fogColor);
				pShader->SetUniform("uFogParam", CVector2(start, end));
			} else {
				pShaderMgr->Update(pShader, "", "ENABLE_FOG");
			}
		}
		m_bFogEnable = enable;
	}
}

/**
* 获取雾是否显示
* @return 当前是否开启雾显示
*/
bool CGraphicsManager::GetFogEnable() {
	return m_bFogEnable;
}

/**
* 设置启用环境光照
* @param enable 是否开启环境光照
* @param cubemap 环境立方体纹理名称
*/
void CGraphicsManager::SetEnvironmentMapEnable(bool enable, const string& cubemap) {
	if (enable || m_bEnvironmentMapEnable) {
		CTextureManager* pTextureMgr = CEngine::GetTextureManager();
		CShaderManager* pShaderMgr = CEngine::GetShaderManager();
		CTexture* pTexture = pTextureMgr->GetTexture(cubemap);
		CTexture* pIrradianceMap = pTextureMgr->GetTexture("__irradiancemap__");
		CTexture* pEnvironmentMap = pTextureMgr->GetTexture("__environmentmap__");
		CTexture* pBRDFIntegrationMap = pTextureMgr->GetTexture("__integrationmap__");
		if (!pTexture || !pTexture->IsValid() || !pTexture->IsCubeMap()) enable = false;
		if (enable) {
			if (!pIrradianceMap) pIrradianceMap = pTextureMgr->Create("__irradiancemap__", 32, false, false, false);
			if (!pEnvironmentMap) pEnvironmentMap = pTextureMgr->Create("__environmentmap__", 128, false, false, true);
			if (!pBRDFIntegrationMap) pBRDFIntegrationMap = pTextureMgr->Create("__integrationmap__", 512, 512, false, false, false);
			CTexture* pRenderTarget = m_pRenderTarget;
			SetRenderTarget(pIrradianceMap, 0, true, false, false);
			pTextureMgr->Update(pIrradianceMap, cubemap, "ibl_irradiancemap", 0);
			SetRenderTarget(pEnvironmentMap, 0, true, false, false);
			CShader* pShader = pShaderMgr->GetShader("ibl_environmentmap");
			pShader->UseShader();
			for (int i = 0; i < 5; i++) {
				pShader->SetUniform("uRoughness", (float)i / 4.0f);
				pTextureMgr->Update(pEnvironmentMap, cubemap, pShader->GetName(), i);
			}
			SetRenderTarget(pBRDFIntegrationMap, 0, true, false, false);
			pTextureMgr->Update(pBRDFIntegrationMap, "", "ibl_integrationmap", 0);
			SetRenderTarget(pRenderTarget, 0, true, false, false);
		}
		const char* shaderName[] = { "texturelight", "light" };
		const int shaderCount = sizeof(shaderName) / sizeof(char*);
		for (size_t i = 0; i < shaderCount; i++) {
			CShader* pShader = pShaderMgr->GetShader(shaderName[i]);
			pShader->UseShader();
			if (enable) {
				if (!m_bEnvironmentMapEnable) pShaderMgr->Update(pShader, "ENABLE_ENVIRONMENT", "");
				pShader->SetUniform("uIrradianceMap", 9);
				pShader->SetUniform("uEnvironmentMap", 10);
				pShader->SetUniform("uBRDFIntegrationMap", 11);
				pIrradianceMap->UseTexture(9);
				pEnvironmentMap->UseTexture(10);
				pBRDFIntegrationMap->UseTexture(11);
				glActiveTexture(GL_TEXTURE0);
			} else {
				pShaderMgr->Update(pShader, "", "ENABLE_ENVIRONMENT");
			}
		}
		m_bEnvironmentMapEnable = enable;
	}
}

/**
* 获取是否开启环境光照
* @return 当前是否开启环境光照
*/
bool CGraphicsManager::GetEnvironmentMapEnable() {
	return m_bEnvironmentMapEnable;
}

/**
* 设置是否启用深度测试
* @param enable 使能深度测试
*/
void CGraphicsManager::SetDepthTestEnable(bool enable) {
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

/**
* 设置是否启用深度偏移
* @param enable 使能深度偏移
* @param offset 偏移量
* @note enable 为 false 时，offset 将被忽略
*/
void CGraphicsManager::SetDepthOffsetEnable(bool enable, float offset) {
	if (enable) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(offset, offset);
	} else {
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

/**
* 设置是否启用颜色混合
* @param enable 使能颜色混合
*/
void CGraphicsManager::SetColorBlendEnable(bool enable) {
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

/**
* 设置渲染目标的背景颜色
* @param color 背景颜色
*/
void CGraphicsManager::SetBackgroundColor(const CColor& color) {
	m_cBackground.SetValue(color);
	glClearColor(m_cBackground[0], m_cBackground[1], m_cBackground[2], m_cBackground[3]);
}

/**
* 设置立体视觉焦距
* @param distance 关注点距离相机距离
*/
void CGraphicsManager::SetFocusDistance(float distance) {
	if (distance < 0.25f) distance = 0.25f;
	m_fFocusDistance = distance;
}

/**
* 判断是否处于深度渲染模式
* @return 当前是否处于深度渲染状态
*/
bool CGraphicsManager::IsDepthRender() {
	return m_bRenderDepth;
}

/**
* 判断是否处于反射图渲染
* @return 当前是否处于反射图渲染状态
*/
bool CGraphicsManager::IsReflectRender() {
	return m_bRenderReflect;
}

/**
* 判断是否处于左眼渲染
* @return 当前是否处于左眼渲染状态
*/
bool CGraphicsManager::IsLeftEyeRender() {
	return m_bRenderLeftEye;
}

/**
* 屏幕截图
* @param file 图片文件对象
* @param redraw 是否需要重新渲染整个场景
*/
void CGraphicsManager::Screenshot(CFileManager::CImageFile& file, bool redraw) {
	const int pictureWidth = m_pRenderTarget? m_pRenderTarget->GetWidth(): m_iWindowSize[0];
	const int pictureHeight = m_pRenderTarget? m_pRenderTarget->GetHeight(): m_iWindowSize[1];

	if (file.contents) delete[] file.contents;
	file.channels = file.type == CFileManager::JPG ? 3 : 4;
	file.width = pictureWidth;
	file.height = pictureHeight;
	file.size = file.channels * pictureWidth * pictureHeight;
	file.contents = new unsigned char[file.size];
	GLenum format = file.channels == 3 ? GL_RGB : GL_RGBA;

	// 渲染整个场景
	if (redraw) Render();
	glFlush();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, pictureWidth, pictureHeight, format, GL_UNSIGNED_BYTE, file.contents);

	// 上下交换像素行
	size_t rowBytes = pictureWidth * file.channels;
	unsigned char* row = new unsigned char[rowBytes];
	for (int i = 0; i < (pictureHeight >> 1); i++) {
		int index1 = rowBytes * i;
		int index2 = rowBytes * (pictureHeight - i - 1);
		memcpy(row, &file.contents[index1], rowBytes);
		memcpy(&file.contents[index1], &file.contents[index2], rowBytes);
		memcpy(&file.contents[index2], row, rowBytes);
	}
	delete[] row;
}

/**
* 屏幕截图并保存至文件
* @param fileName 保存截图的文件名称
* @param redraw 是否需要重新渲染整个场景
*/
void CGraphicsManager::Screenshot(const string& fileName, bool redraw) {
	// 截屏图片保存目录是否存在
	CFileManager* pFileMgr = CEngine::GetFileManager();
	string directory = CFileManager::GetDirectory(fileName);
	if (!pFileMgr->DirectoryExists(directory)) {
		if (!pFileMgr->DirectoryCreate(directory)) {
			CLog::Warn("Create directory '%s' failed", directory.c_str());
			return;
		}
	}
	// 存储图片文件
	CFileManager::FileType fileType = CFileManager::BIN;
	string ext = CStringUtil::UpperCase(CFileManager::GetExtension(fileName));
	if (ext == "BMP") fileType = CFileManager::BMP;
	else if (ext == "TGA") fileType = CFileManager::TGA;
	else if (ext == "PNG") fileType = CFileManager::PNG;
	else if (ext == "JPG" || ext == "JPEG") fileType = CFileManager::JPG;
	CFileManager::CImageFile file(fileType);
	Screenshot(file, redraw);
	if (pFileMgr->WriteFile(&file, fileName) > 0) {
		CLog::Info("Saved image file '%s'", fileName.c_str());
	}
}

/**
* 获取点的屏幕坐标
* @param point 需要映射的空间点
* @param screen 转换后的屏幕坐标，screen.Z 为深度值
*/
void CGraphicsManager::PointToScreen(const CVector3& point, CVector3& screen) {
	// 获取视口参数
	int viewport[4];
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bRenderLeftEye ? 0 : viewport[2];
	Project(viewport, point.m_fValue, screen.m_fValue);
}

/**
* 通过给定的屏幕坐标计算拾取射线
* @param x 屏幕横坐标
* @param y 屏幕纵坐标
* @param ray 计算得到的拾取射线
* @note 屏幕原点为左上角
*/
void CGraphicsManager::PickingRay(int x, int y, CRay& ray) {
	int viewport[4];
	float screen[3];
	float posStart[3];
	float posEnd[3];
	// 获取视口参数
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bStereoMode && x > viewport[2] ? viewport[2] : 0;
	// 反投影计算
	screen[0] = static_cast<float>(x);
	screen[1] = static_cast<float>(viewport[3] - y);
	screen[2] = 0;
	UnProject(viewport, screen, posStart);
	screen[2] = 1;
	UnProject(viewport, screen, posEnd);
	// 得到 Picking 向量
	float direction[3] = {
		posEnd[0] - posStart[0],
		posEnd[1] - posStart[1],
		posEnd[2] - posStart[2]
	};
	ray.SetValue(posStart, direction);
}

/**
* 通过给定的屏幕坐标计算拾取点坐标
* @param x 屏幕横坐标
* @param y 屏幕纵坐标
* @param position 计算得到的空间点坐标
* @attention 屏幕原点为左上角，该方法不支持 OpenGL ES
*/
void CGraphicsManager::PickingPosition(int x, int y, CVector3& position) {
	int viewport[4];
	float screen[3];
	// 获取视口参数
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bStereoMode && x > viewport[2] ? viewport[2] : 0;
	screen[0] = static_cast<float>(x);
	screen[1] = static_cast<float>(viewport[3] - y);
	// 读取深度值
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
	glReadPixels(static_cast<int>(screen[0]), static_cast<int>(screen[1]), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screen[2]);
#else
	screen[2] = 0;
	CLog::Warn("PickingPosition failed, OpenGL ES not support");
#endif
	UnProject(viewport, screen, position.m_fValue);
}

/**
* 绘制点
* @param vertices 顶点数组
* @param size 顶点个数
* @note 点的大小由着色器设置(uPointSize)
*/
void CGraphicsManager::DrawPoints(const CVertex* vertices, int size) {
	size_t pointer = reinterpret_cast<size_t>(vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 10));
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	SetDepthOffsetEnable(true, -1.0f);
	glDrawArrays(GL_POINTS, 0, size);
	SetDepthOffsetEnable(false, 0.0f);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

/**
* 绘制线
* @param vertices 顶点数组
* @param size 顶点个数
* @param width 线宽
* @param strip 线条（true）还是线段（false）
*/
void CGraphicsManager::DrawLines(const CVertex* vertices, int size, float width, bool strip) {
	// 确保 size 为偶数且大于等于2
	if (!strip) size -= size % 2;
	if (size < 2) return;
	size_t pointer = reinterpret_cast<size_t>(vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 4));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 10));
	glDisableVertexAttribArray(2);
	glLineWidth(width);
	SetDepthOffsetEnable(true, -1.0f);
	glDrawArrays(strip? GL_LINE_STRIP: GL_LINES, 0, size);
	SetDepthOffsetEnable(false, 0.0f);
	glEnableVertexAttribArray(2);
}

/**
* 绘制三角形
* @param vertices 顶点数组
* @param size 顶点个数
* @param strip 三角形带（true）还是三角形集合（false）
*/
void CGraphicsManager::DrawTriangles(const CVertex* vertices, int size, bool strip) {
	// 顶点个数需为三的倍数
	if (!strip) size -= size % 3;
	if (size < 3) return;
	size_t pointer = reinterpret_cast<size_t>(vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 4));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 6));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CVertex), (GLvoid*)(pointer + sizeof(float) * 10));
	glDrawArrays(strip? GL_TRIANGLE_STRIP: GL_TRIANGLES, 0, size);
}

/**
* 绘制四边形
* @param color 四边形颜色
* @param flipTexCoord 垂直翻转纹理坐标
*/
void CGraphicsManager::DrawQuadrilateral(const CColor& color, bool flipTexCoord) {
	float vertices[] = {
		-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
	};
	if (flipTexCoord) {
		vertices[5] = vertices[25] = 0.0f;
		vertices[15] = vertices[35] = 1.0f;
	}
	color.FillBuffer(vertices + 6, 40, 10);
	size_t pointer = reinterpret_cast<size_t>(vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(pointer));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(pointer + sizeof(float) * 4));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (GLvoid*)(pointer + sizeof(float) * 6));
	glDisableVertexAttribArray(2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnableVertexAttribArray(2);
}

/**
* 绘制立方体
* @param color 立方体颜色
*/
void CGraphicsManager::DrawCube(const CColor& color) {
	float vertices[] = {
		-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
	};
	unsigned short indices[] = {
		2, 1, 5, 2, 5, 6, 0, 3, 7, 0, 7, 4, 3, 2, 6, 3, 6, 7,
		1, 0, 4, 1, 4, 5, 7, 6, 5, 7, 5, 4, 0, 1, 2, 0, 2, 3
	};
	color.FillBuffer(vertices + 4, 64, 8);
	size_t pointer = reinterpret_cast<size_t>(vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid*)(pointer));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (GLvoid*)(pointer + sizeof(float) * 4));
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indices);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

/**
* 设置当前渲染目标
* @param texture 渲染目标纹理，为 0 表示屏幕
* @param level 纹理 mipmap level
* @param fullView 指示在立体显示模式下不分左右视口
* @param clearColor 清空 texture 的颜色
* @param clearDepth 清空 texture 的深度信息
*/
void CGraphicsManager::SetRenderTarget(CTexture* texture, int level, bool fullView, bool clearColor, bool clearDepth) {
	int viewWidth = m_iWindowSize[0];
	int viewHeight = m_iWindowSize[1];
	CEngine::GetTextureManager()->Drop(m_pRenderTarget);
	if (texture && texture->IsRenderTarget()) {
		viewWidth = texture->GetWidth();
		viewHeight = texture->GetHeight();
		m_pRenderTarget = texture->AddReference();
		glBindFramebuffer(GL_FRAMEBUFFER, texture->GetFramebuffer());
		if (level > 0 || (texture->IsCubeMap() && (clearColor || clearDepth))) {
			viewWidth >>= level;
			viewHeight >>= level;
			GLenum target = texture->IsCubeMap() ? GL_TEXTURE_CUBE_MAP_POSITIVE_X : GL_TEXTURE_2D;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, texture->GetTextureId(), level);
		}
	} else {
		m_pRenderTarget = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	// 设置视口
	if (m_bStereoMode && !fullView) {
		viewWidth >>= 1;
		glViewport(m_bRenderLeftEye ? 0 : viewWidth, 0, viewWidth, viewHeight);
	} else {
		glViewport(0, 0, viewWidth, viewHeight);
	}
	// 如果是渲染深度图，需要将深度值设置为1.0
	if (clearColor) {
		const float* color = m_bRenderDepth ? CColor::Red.m_fValue : m_cBackground.m_fValue;
		glClearColor(color[0], color[1], color[2], color[3]);
	}
	// 是否清空颜色和深度缓冲区
	if (clearColor && clearDepth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clearColor) glClear(GL_COLOR_BUFFER_BIT);
	else if (clearDepth) glClear(GL_DEPTH_BUFFER_BIT);
}

/**
* 获取当前渲染目标
* @return 渲染目标纹理
*/
CTexture* CGraphicsManager::GetRenderTarget() {
	return m_pRenderTarget;
}

/**
* 获取帧率
* @return 当前帧率
*/
float CGraphicsManager::GetFrameRate() {
	return m_fFrameRate;
}

/**
* 世界坐标系到屏幕坐标系的变换
* @param viewport 视口参数（左上角坐标，视口高宽）
* @param objCoord 空间坐标
* @param winPos 屏幕坐标
*/
void CGraphicsManager::Project(const int viewport[4], const float objCoord[3], float winPos[3]) {
	// 计算视图投影矩阵
	CMatrix4 matrix = m_pCamera->GetProjMatrix() * m_pCamera->GetViewMatrix();
	// 变换得到屏幕坐标系坐标
	CVector3 vec3d = matrix * CVector3(objCoord);
	vec3d *= 1.0f / vec3d[3];
	winPos[0] = viewport[0] + (1.0f + vec3d[0]) * viewport[2] * 0.5f;
	winPos[1] = viewport[1] + (1.0f + vec3d[1]) * viewport[3] * 0.5f;
	winPos[2] = (1.0f + vec3d[2]) * 0.5f;
}

/**
* 屏幕坐标系到世界坐标系的变换
* @param viewport 视口参数（左上角坐标，视口高宽）
* @param winPos 屏幕坐标
* @param objCoord 空间坐标
*/
void CGraphicsManager::UnProject(const int viewport[4], const float winPos[3], float objCoord[3]) {
	// 将屏幕坐标转换为 OpenGL 表示
	CVector3 vec3d;
	vec3d[0] = (winPos[0] - viewport[0]) / static_cast<float>(viewport[2]) * 2.0f - 1.0f;
	vec3d[1] = (winPos[1] - viewport[1]) / static_cast<float>(viewport[3]) * 2.0f - 1.0f;
	vec3d[2] = 2.0f * winPos[2] - 1.0f;
	// 计算视图投影矩阵逆矩阵
	CMatrix4 matrix = m_pCamera->GetProjMatrix() * m_pCamera->GetViewMatrix();
	matrix.Invert();
	// 变换得到世界坐标系坐标
	vec3d = matrix * vec3d;
	vec3d *= 1.0f / vec3d[3];
	objCoord[0] = vec3d[0];
	objCoord[1] = vec3d[1];
	objCoord[2] = vec3d[2];
}

/**
* 绘制场景
*/
void CGraphicsManager::Draw() {
	// 获取绘制屏幕大小
	int screenWidth = m_pRenderTarget ? m_pRenderTarget->GetWidth() : m_iWindowSize[0];
	int screenHeight = m_pRenderTarget ? m_pRenderTarget->GetHeight() : m_iWindowSize[1];
	CTexture* pRenderTarget = m_pRenderTarget;
	// 渲染过程纹理和阴影贴图和反射贴图
	CEngine::GetTextureManager()->Update();
	if (m_bShadowEnable) DrawShadowMap();
	DrawReflectMap();
	// 准备后处理过程
	CPostProcessManager* pPostProcessMgr = CEngine::GetPostProcessManager();
	if (!pPostProcessMgr->PrepareFrame(pRenderTarget, screenWidth, screenHeight)) {
		SetRenderTarget(pRenderTarget, 0, false, false, false);
	}
	// 获取渲染目标宽高
	int viewWidth = screenWidth;
	int viewHeight = screenHeight;
	if (m_pRenderTarget) {
		if (m_pRenderTarget->IsCubeMap()) {
			DrawCubeMap();
			pPostProcessMgr->ApplyFrame();
			CGuiEnvironment::GetInstance()->Render();
			return;
		}
		viewWidth = m_pRenderTarget->GetWidth();
		viewHeight = m_pRenderTarget->GetHeight();
	}
	// 左右立体显示模式
	if (m_bStereoMode) {
		viewWidth >>= 1;
		m_pCamera->SetViewSize(viewWidth, viewHeight);
		m_pCamera->UpdateProjMatrix(false);

		// 设置眼间距 63.6 mm
		CMatrix4& viewMat = m_pCamera->GetViewMatrix();
		CVector3 eyePos = m_pCamera->m_cPosition;
		CVector3 eyeDir = m_pCamera->m_cLookVector;
		CVector3 rightDir = CVector3(viewMat(0, 0), viewMat(0, 1), viewMat(0, 2)) * 0.0318f;

		// 渲染左眼场景
		m_bRenderLeftEye = true;
		m_pCamera->m_cPosition = eyePos - rightDir;
		m_pCamera->m_cLookVector = (eyeDir * m_fFocusDistance + rightDir).Normalize();
		m_pCamera->UpdateViewMatrix();
		m_pCamera->UpdateFrustum();
		glViewport(0, 0, viewWidth, viewHeight);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		DrawScreen();

		// 渲染右眼场景
		m_bRenderLeftEye = false;
		m_pCamera->m_cPosition = eyePos + rightDir;
		m_pCamera->m_cLookVector = (eyeDir * m_fFocusDistance - rightDir).Normalize();
		m_pCamera->UpdateViewMatrix();
		m_pCamera->UpdateFrustum();
		glViewport(viewWidth, 0, viewWidth, viewHeight);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		DrawScreen();

		// 恢复相机位置
		m_pCamera->m_cPosition = eyePos;
		m_pCamera->m_cLookVector = eyeDir;
		glViewport(0, 0, viewWidth << 1, viewHeight);
	} else {
		m_pCamera->SetViewSize(viewWidth, viewHeight);
		m_pCamera->UpdateProjMatrix(false);
		glViewport(0, 0, viewWidth, viewHeight);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		DrawScreen();
	}
	// 进行后处理
	pPostProcessMgr->ApplyFrame();
	// 绘制 GUI 元素
	CGuiEnvironment::GetInstance()->Render();
	// 始终渲染到窗口
	if (pRenderTarget) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, pRenderTarget->GetFramebuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, viewWidth, viewHeight, 0, 0, m_iWindowSize[0], m_iWindowSize[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, pRenderTarget->GetFramebuffer());
	}
}

/**
* 绘制屏幕元素
*/
void CGraphicsManager::DrawScreen() {
	// 设置屏幕原点偏移
	float offsetx = -0.5f * m_pCamera->GetViewWidth();
	float offsety = -0.5f * m_pCamera->GetViewHeight();
	CVector2 offset(offsetx, offsety);
	// 设置正交投影
	m_pCamera->UpdateProjMatrix(true);
	glDisable(GL_DEPTH_TEST);
	CEngine::GetSceneManager()->RenderScreen(m_pCamera->GetProjMatrix(), offset);
	// 绘制 LOGO
#ifdef AVATAR_ENABLE_LOGO
	CEngine::GetTextureManager()->GetTexture("logo")->UseTexture(0);
	CShader* pShader = CEngine::GetShaderManager()->GetCurrentShader();
	pShader->SetUniform("uOffset", offset.Add(32.0f).Scale(-1.0f));
	pShader->SetUniform("uModelMatrix", CMatrix4().Scale(32.0f, 32.0f, 1.0f));
	DrawQuadrilateral(CColor::White, true);
#endif
	// 设置透视投影
	m_pCamera->UpdateProjMatrix(false);
	glEnable(GL_DEPTH_TEST);
}

/**
* 绘制阴影贴图
*/
void CGraphicsManager::DrawShadowMap() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTexture* pShadowMap = pTextureMgr->GetTexture("__shadowmap__");
	CTexture* pShadowMapBlur = pTextureMgr->GetTexture("__shadowmap_blur__");
	CShader* pShaderBlur = pShaderMgr->GetShader("shadowmapfilter");
	// 初始化阴影贴图资源
	if (!pShadowMap) {
		const int mapSize = 1024;
		pShadowMap = pTextureMgr->Create("__shadowmap__", mapSize, mapSize, true, true, false);
		pShadowMap->SetWrapModeClampToEdge(true, true);
		pShadowMapBlur = pTextureMgr->Create("__shadowmap_blur__", mapSize, mapSize, true, false, false);
		pShadowMapBlur->SetWrapModeClampToEdge(true, true);
		pShaderBlur->UseShader();
		pShaderBlur->SetUniform("uTextureSize", CVector2(mapSize, mapSize));
	}
	// 计算光源视图投影矩阵
	CMatrix4 viewMat;
	CMatrix4 projMat;
	if (m_cLightPos.m_fValue[3] == 0.0f) {
		CVector3 pos = m_cLightPos * -50.0f;
		CVector3 dir = m_cLightPos;
		viewMat.LookAt(pos, dir, dir.Tangent().Normalize());
		projMat.Ortho(30.0f, 30.0f, 0, 100.0f);
	} else if (m_cLightDir.m_fValue[3] > 0.0f) {
		CVector3 pos = m_cLightPos;
		CVector3 dir = m_cLightDir;
		viewMat.LookAt(pos, dir, dir.Tangent().Normalize());
		projMat.Perspective(m_cLightDir[3] * 57.29578f, 1.0f, 0.5f, 1000.0f);
	} else {
		CVector3 pos = m_cLightPos;
		CVector3 dir = (m_cLightPos * -1.0f).Normalize();
		viewMat.LookAt(pos, dir, dir.Tangent().Normalize());
		projMat.Perspective(120.0f, 1.0f, 0.5f, 1000.0f);
	}
	// 渲染阴影贴图
	glDisable(GL_BLEND);
	m_bRenderDepth = true;
	CMatrix4 viewProj = projMat * viewMat;
	SetRenderTarget(pShadowMap, 0, true, true, true);
	CEngine::GetSceneManager()->RenderDepth(viewProj, "shadowmap");
	m_bRenderDepth = false;
	// 对阴影贴图进行模糊
	SetRenderTarget(pShadowMapBlur, 0, true, false, false);
	pShadowMap->UseTexture();
	pShaderBlur->UseShader();
	DrawQuadrilateral(CColor::White, false);
	glEnable(GL_BLEND);
	// 设置阴影贴图纹理单元
	pShadowMapBlur->UseTexture(8);
	glActiveTexture(GL_TEXTURE0);
	// 阴影矩阵添加上偏移，提前对纹理坐标修正
	const float biasMat[16] = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	};
	CMatrix4 shadowMat = CMatrix4(biasMat) * viewProj;
	// 设置相关着色器的阴影矩阵
	vector<CShader*> shaderList;
	pShaderMgr->GetShaderList(shaderList);
	for (size_t i = 0; i < shaderList.size(); i++) {
		CShader* pShader = shaderList[i];
		if (pShader->IsUniform("uShadowMatrix")) {
			pShader->UseShader();
			pShader->SetUniform("uShadowMatrix", shadowMat);
		}
	}
}

/**
* 绘制全景图
*/
void CGraphicsManager::DrawCubeMap() {
	float saved_fov = m_pCamera->GetFieldOfView();
	int viewWidth = m_pRenderTarget->GetWidth();
	int viewHeight = m_pRenderTarget->GetHeight();
	m_pCamera->SetFieldOfView(90.0f);
	m_pCamera->SetViewSize(viewWidth, viewHeight);
	m_pCamera->UpdateProjMatrix(false);
	glViewport(0, 0, viewWidth, viewHeight);
	CMatrix4& mat = m_pCamera->GetViewMatrix();
	// 取相对于当前相机方位的6个方向
	const float look[6][3] = {
		{ mat[0], mat[4], mat[8] }, { -mat[0], -mat[4], -mat[8] },
		{ -mat[2], -mat[6], -mat[10] }, { mat[2], mat[6], mat[10] },
		{ mat[1], mat[5], mat[9] }, { -mat[1], -mat[5], -mat[9] }
	};
	const float up[6][3] = {
		{ mat[2], mat[6], mat[10] }, { mat[2], mat[6], mat[10] },
		{ mat[1], mat[5], mat[9] }, { -mat[1], -mat[5], -mat[9] },
		{ mat[2], mat[6], mat[10] }, { mat[2], mat[6], mat[10] }
	};
	CSceneManager* pSceneMgr = CEngine::GetSceneManager();
	// +X 方向已经默认绑定并清空了颜色和深度
	m_pCamera->GetViewMatrix().LookAt(m_pCamera->m_cPosition, look[0], up[0]);
	m_pCamera->UpdateFrustum();
	pSceneMgr->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	for (int i = 1; i < 6; i++) {
		GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, face, m_pRenderTarget->GetTextureId(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_pCamera->GetViewMatrix().LookAt(m_pCamera->m_cPosition, look[i], up[i]);
		m_pCamera->UpdateFrustum();
		pSceneMgr->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	}
	m_pCamera->SetFieldOfView(saved_fov);
	m_pCamera->UpdateViewMatrix();
}

/**
* 渲染反射贴图
*/
void CGraphicsManager::DrawReflectMap() {
	vector<CSceneNode*> nodeList;
	CEngine::GetSceneManager()->GetNodeList("water", nodeList);
	if (nodeList.empty()) return;
	// 保存摄像机参数
	m_bRenderReflect = true;
	CMatrix4 projMat = m_pCamera->GetProjMatrix();
	CMatrix4 viewMat = m_pCamera->GetViewMatrix();
	CVector3 cameraPos = m_pCamera->m_cPosition;
	CVector3 cameraLook = m_pCamera->m_cLookVector;
	CVector3 cameraUp = m_pCamera->m_cUpVector;
	// 渲染所有水面倒影
	for (size_t i = 0; i < nodeList.size(); i++) {
		CSceneNode* node = nodeList[i];
		// 获取水面平面
		CPlane reflectPlane(CVector3(0.0f, 0.0f, 1.0f), 0.0f);
		reflectPlane.Transform(node->m_cWorldMatrix);
		if (reflectPlane.Distance(cameraPos) < 0.0f) reflectPlane.FlipNormal();
		// 设置水面反射图相机参数
		CMatrix4 reflectMat = reflectPlane.GetReflectMatrix();
		m_pCamera->m_cPosition = reflectPlane.GetMirrorPoint(cameraPos);
		m_pCamera->m_cLookVector = reflectMat * CVector3(-viewMat(2, 0), -viewMat(2, 1), -viewMat(2, 2), 0.0f);
		m_pCamera->m_cUpVector = reflectMat * CVector3(viewMat(1, 0), viewMat(1, 1), viewMat(1, 2), 0.0f);
		m_pCamera->GetViewMatrix().LookAt(m_pCamera->m_cPosition, m_pCamera->m_cLookVector, m_pCamera->m_cUpVector);
		// 设置斜裁剪投影矩阵
		reflectPlane.Transform(m_pCamera->GetViewMatrix());
		m_pCamera->GetProjMatrix().ObliqueNearPlaneClipping(m_pCamera->GetProjMatrix(), reflectPlane);
		m_pCamera->UpdateFrustum();
		// 渲染场景到纹理
		CTexture* reflectTexture = CEngine::GetTextureManager()->GetTexture(node->GetName() + "_reflect");
		SetRenderTarget(reflectTexture, 0, true, true, true);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	}
	// 恢复摄像机参数
	m_bRenderReflect = false;
	m_pCamera->m_cPosition.SetValue(cameraPos);
	m_pCamera->m_cLookVector.SetValue(cameraLook);
	m_pCamera->m_cUpVector.SetValue(cameraUp);
	m_pCamera->GetProjMatrix().SetValue(projMat);
	m_pCamera->GetViewMatrix().SetValue(viewMat);
	m_pCamera->UpdateFrustum();
}