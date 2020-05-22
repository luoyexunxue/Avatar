//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* ���캯��
*/
CGraphicsManager::CGraphicsManager() {
	// OPENGL ��ʼ״̬
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
	// �����ɫ�����
	glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	// ��ʼ���ڲ�����
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
* ��������
*/
CGraphicsManager::~CGraphicsManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CGraphicsManager* CGraphicsManager::m_pInstance = 0;

/**
* ʵ������
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
* ��Ⱦһ֡
*/
void CGraphicsManager::Render() {
	// ���������ɫ�����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ������������
	Draw();
	// ����֡����Ϣ
	float interval = CTimer::Reset("__graphicsmanager__");
	if (interval > 0.0f) {
		m_fFrameRate = 0.3f * (1.0f / interval) + 0.7f * m_fFrameRate;
	}
}

/**
* �������
* @param camera ��ͼ����Ⱦ���������
*/
void CGraphicsManager::SetCamera(CCamera* camera) {
	m_pCamera = camera;
	m_pCamera->Control(true);
}

/**
* ��ȡ���
* @return ��ͼ����Ⱦ���������
*/
CCamera* CGraphicsManager::GetCamera() {
	return m_pCamera;
}

/**
* ���ô��ڴ�С
* @param width ���ƴ��ڿ��
* @param height ���ƴ��ڸ߶�
*/
void CGraphicsManager::SetWindowSize(int width, int height) {
	if (m_iWindowSize[0] != width || m_iWindowSize[1] != height) {
		m_iWindowSize[0] = width;
		m_iWindowSize[1] = height;
		CGuiEnvironment::GetInstance()->UpdateSize(width, height);
		CEngine::GetPostProcessManager()->UpdateSize(width, height);
	}
}

/**
* ��ȡ���ڴ�С
* @param width ������ƴ��ڿ��
* @param height ������ƴ��ڸ߶�
*/
void CGraphicsManager::GetWindowSize(int* width, int* height) {
	*width = m_iWindowSize[0];
	*height = m_iWindowSize[1];
}

/**
* ���ù�Դ
* @param pos ��Դλ�ã��� pos.w Ϊ 0 ʱ��Ϊ����⣬����Ϊ���Դ��۹��
* @param dir ���շ��򣬵� dir.w ���� 0 ʱ��Ϊ�۹���� dir.w Ϊ�۹�Ƕȷ�Χ
* @param color ������ɫ����Ϊ���Դ��۹�ƹ�Դʱ��color.a Ϊ���շ�Χ
* @note ϵͳͨ������ pos �� dir �Զ��жϹ�Դ���ͣ�֧��ƽ�й��Դ�����Դ���۹�ƹ�Դ����
*/
void CGraphicsManager::SetLight(const CVector3& pos, const CVector3& dir, const CVector3& color) {
	// ��λ����ز���
	CVector3 normPos(pos.m_fValue, 0.0f);
	CVector3 normDir(dir.m_fValue, 0.0f);
	normPos.Normalize();
	normDir.Normalize();
	// ���ݹ�Դ���ͶԲ�����������
	if (pos.m_fValue[3] == 0.0f) {
		m_cLightPos.SetValue(normPos);
		m_cLightDir.SetValue(normPos);
		normDir.SetValue(normPos);
	} else {
		m_cLightPos.SetValue(pos);
		m_cLightDir.SetValue(normDir.m_fValue, dir.m_fValue[3]);
		if (dir.m_fValue[3] > 0.0f) {
			normDir[3] = cosf(dir.m_fValue[3]);
			if (normDir[3] < 0.0872f) normDir[3] = 0.0872f;
		}
	}
	// ��ÿ������ uLightPos ����ɫ������ͳһ������ֵ
	vector<CShader*> shaderList;
	CEngine::GetShaderManager()->GetShaderList(shaderList);
	for (size_t i = 0; i < shaderList.size(); i++) {
		CShader* pShader = shaderList[i];
		if (pShader->IsUniform("uLightPos")) {
			pShader->UseShader();
			// ������Ⱦ����ʹ�÷����
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
* ��ȡ��Դλ��
* @return ��Դλ��
*/
CVector3 CGraphicsManager::GetLight() {
	return m_cLightPos;
}

/**
* �����Ƿ�������ʾ
* @param stereo ������Ⱦģʽ
* @note ����������Ⱦģʽ�������ҷ�����ʽ��Ⱦ����ͼ��
*/
void CGraphicsManager::SetStereoMode(bool stereo) {
	m_bStereoMode = stereo;
	if (!stereo) m_bRenderLeftEye = true;
}

/**
* ��ȡ�Ƿ�������ʾ
* @return ��ǰ�Ƿ�Ϊ������Ⱦģʽ
*/
bool CGraphicsManager::GetStereoMode() {
	return m_bStereoMode;
}

/**
* �����Ƿ���ʾ��Ӱ
* @param enable ��ʾ��Ӱ
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
* ��ȡ��Ӱ�Ƿ���ʾ
* @return ��ǰ�Ƿ���ʾ��Ӱ
*/
bool CGraphicsManager::GetShadowEnable() {
	return m_bShadowEnable;
}

/**
* ��������ʾ������
* @param enable �Ƿ���ʾ��
* @param start ��ʼ����
* @param end ��������
* @param color ��ɫ
* @note enable Ϊ false ʱ��start��end��color ��������
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
* ��ȡ���Ƿ���ʾ
* @return ��ǰ�Ƿ�������ʾ
*/
bool CGraphicsManager::GetFogEnable() {
	return m_bFogEnable;
}

/**
* �������û�������
* @param enable �Ƿ�����������
* @param cubemap ������������������
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
* ��ȡ�Ƿ�����������
* @return ��ǰ�Ƿ�����������
*/
bool CGraphicsManager::GetEnvironmentMapEnable() {
	return m_bEnvironmentMapEnable;
}

/**
* �����Ƿ�������Ȳ���
* @param enable ʹ����Ȳ���
*/
void CGraphicsManager::SetDepthTestEnable(bool enable) {
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

/**
* �����Ƿ��������ƫ��
* @param enable ʹ�����ƫ��
* @param offset ƫ����
* @note enable Ϊ false ʱ��offset ��������
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
* �����Ƿ�������ɫ���
* @param enable ʹ����ɫ���
*/
void CGraphicsManager::SetColorBlendEnable(bool enable) {
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

/**
* ������ȾĿ��ı�����ɫ
* @param color ������ɫ
*/
void CGraphicsManager::SetBackgroundColor(const CColor& color) {
	m_cBackground.SetValue(color);
	glClearColor(m_cBackground[0], m_cBackground[1], m_cBackground[2], m_cBackground[3]);
}

/**
* ���������Ӿ�����
* @param distance ��ע������������
*/
void CGraphicsManager::SetFocusDistance(float distance) {
	if (distance < 0.25f) distance = 0.25f;
	m_fFocusDistance = distance;
}

/**
* �ж��Ƿ��������Ⱦģʽ
* @return ��ǰ�Ƿ��������Ⱦ״̬
*/
bool CGraphicsManager::IsDepthRender() {
	return m_bRenderDepth;
}

/**
* �ж��Ƿ��ڷ���ͼ��Ⱦ
* @return ��ǰ�Ƿ��ڷ���ͼ��Ⱦ״̬
*/
bool CGraphicsManager::IsReflectRender() {
	return m_bRenderReflect;
}

/**
* �ж��Ƿ���������Ⱦ
* @return ��ǰ�Ƿ���������Ⱦ״̬
*/
bool CGraphicsManager::IsLeftEyeRender() {
	return m_bRenderLeftEye;
}

/**
* ��Ļ��ͼ
* @param file ͼƬ�ļ�����
* @param redraw �Ƿ���Ҫ������Ⱦ��������
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

	// ��Ⱦ��������
	if (redraw) Render();
	glFlush();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, pictureWidth, pictureHeight, format, GL_UNSIGNED_BYTE, file.contents);

	// ���½���������
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
* ��Ļ��ͼ���������ļ�
* @param fileName �����ͼ���ļ�����
* @param redraw �Ƿ���Ҫ������Ⱦ��������
*/
void CGraphicsManager::Screenshot(const string& fileName, bool redraw) {
	// ����ͼƬ����Ŀ¼�Ƿ����
	CFileManager* pFileMgr = CEngine::GetFileManager();
	string directory = CFileManager::GetDirectory(fileName);
	if (!pFileMgr->DirectoryExists(directory)) {
		if (!pFileMgr->DirectoryCreate(directory)) {
			CLog::Warn("Create directory '%s' failed", directory.c_str());
			return;
		}
	}
	// �洢ͼƬ�ļ�
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
* ��ȡ�����Ļ����
* @param point ��Ҫӳ��Ŀռ��
* @param screen ת�������Ļ���꣬screen.Z Ϊ���ֵ
*/
void CGraphicsManager::PointToScreen(const CVector3& point, CVector3& screen) {
	// ��ȡ�ӿڲ���
	int viewport[4];
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bRenderLeftEye ? 0 : viewport[2];
	Project(viewport, point.m_fValue, screen.m_fValue);
}

/**
* ͨ����������Ļ�������ʰȡ����
* @param x ��Ļ������
* @param y ��Ļ������
* @param ray ����õ���ʰȡ����
* @note ��Ļԭ��Ϊ���Ͻ�
*/
void CGraphicsManager::PickingRay(int x, int y, CRay& ray) {
	int viewport[4];
	float screen[3];
	float posStart[3];
	float posEnd[3];
	// ��ȡ�ӿڲ���
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bStereoMode && x > viewport[2] ? viewport[2] : 0;
	// ��ͶӰ����
	screen[0] = static_cast<float>(x);
	screen[1] = static_cast<float>(viewport[3] - y);
	screen[2] = 0;
	UnProject(viewport, screen, posStart);
	screen[2] = 1;
	UnProject(viewport, screen, posEnd);
	// �õ� Picking ����
	float direction[3] = {
		posEnd[0] - posStart[0],
		posEnd[1] - posStart[1],
		posEnd[2] - posStart[2]
	};
	ray.SetValue(posStart, direction);
}

/**
* ͨ����������Ļ�������ʰȡ������
* @param x ��Ļ������
* @param y ��Ļ������
* @param position ����õ��Ŀռ������
* @attention ��Ļԭ��Ϊ���Ͻǣ��÷�����֧�� OpenGL ES
*/
void CGraphicsManager::PickingPosition(int x, int y, CVector3& position) {
	int viewport[4];
	float screen[3];
	// ��ȡ�ӿڲ���
	viewport[1] = 0;
	viewport[2] = m_pCamera->GetViewWidth();
	viewport[3] = m_pCamera->GetViewHeight();
	viewport[0] = m_bStereoMode && x > viewport[2] ? viewport[2] : 0;
	screen[0] = static_cast<float>(x);
	screen[1] = static_cast<float>(viewport[3] - y);
	// ��ȡ���ֵ
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
	glReadPixels(static_cast<int>(screen[0]), static_cast<int>(screen[1]), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screen[2]);
#else
	screen[2] = 0;
	CLog::Warn("PickingPosition failed, OpenGL ES not support");
#endif
	UnProject(viewport, screen, position.m_fValue);
}

/**
* ���Ƶ�
* @param vertices ��������
* @param size �������
* @note ��Ĵ�С����ɫ������(uPointSize)
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
* ������
* @param vertices ��������
* @param size �������
* @param width �߿�
* @param strip ������true�������߶Σ�false��
*/
void CGraphicsManager::DrawLines(const CVertex* vertices, int size, float width, bool strip) {
	// ȷ�� size Ϊż���Ҵ��ڵ���2
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
* ����������
* @param vertices ��������
* @param size �������
* @param strip �����δ���true�����������μ��ϣ�false��
*/
void CGraphicsManager::DrawTriangles(const CVertex* vertices, int size, bool strip) {
	// ���������Ϊ���ı���
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
* �����ı���
* @param color �ı�����ɫ
* @param flipTexCoord ��ֱ��ת��������
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
* ����������
* @param color ��������ɫ
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
* ���õ�ǰ��ȾĿ��
* @param texture ��ȾĿ������Ϊ 0 ��ʾ��Ļ
* @param level ���� mipmap level
* @param fullView ָʾ��������ʾģʽ�²��������ӿ�
* @param clearColor ��� texture ����ɫ
* @param clearDepth ��� texture �������Ϣ
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
	// �����ӿ�
	if (m_bStereoMode && !fullView) {
		viewWidth >>= 1;
		glViewport(m_bRenderLeftEye ? 0 : viewWidth, 0, viewWidth, viewHeight);
	} else {
		glViewport(0, 0, viewWidth, viewHeight);
	}
	// �������Ⱦ���ͼ����Ҫ�����ֵ����Ϊ1.0
	if (clearColor) {
		const float* color = m_bRenderDepth ? CColor::Red.m_fValue : m_cBackground.m_fValue;
		glClearColor(color[0], color[1], color[2], color[3]);
	}
	// �Ƿ������ɫ����Ȼ�����
	if (clearColor && clearDepth) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else if (clearColor) glClear(GL_COLOR_BUFFER_BIT);
	else if (clearDepth) glClear(GL_DEPTH_BUFFER_BIT);
}

/**
* ��ȡ��ǰ��ȾĿ��
* @return ��ȾĿ������
*/
CTexture* CGraphicsManager::GetRenderTarget() {
	return m_pRenderTarget;
}

/**
* ��ȡ֡��
* @return ��ǰ֡��
*/
float CGraphicsManager::GetFrameRate() {
	return m_fFrameRate;
}

/**
* ��������ϵ����Ļ����ϵ�ı任
* @param viewport �ӿڲ��������Ͻ����꣬�ӿڸ߿�
* @param objCoord �ռ�����
* @param winPos ��Ļ����
*/
void CGraphicsManager::Project(const int viewport[4], const float objCoord[3], float winPos[3]) {
	// ������ͼͶӰ����
	CMatrix4 matrix = m_pCamera->GetProjMatrix() * m_pCamera->GetViewMatrix();
	// �任�õ���Ļ����ϵ����
	CVector3 vec3d = matrix * CVector3(objCoord);
	vec3d *= 1.0f / vec3d[3];
	winPos[0] = viewport[0] + (1.0f + vec3d[0]) * viewport[2] * 0.5f;
	winPos[1] = viewport[1] + (1.0f + vec3d[1]) * viewport[3] * 0.5f;
	winPos[2] = (1.0f + vec3d[2]) * 0.5f;
}

/**
* ��Ļ����ϵ����������ϵ�ı任
* @param viewport �ӿڲ��������Ͻ����꣬�ӿڸ߿�
* @param winPos ��Ļ����
* @param objCoord �ռ�����
*/
void CGraphicsManager::UnProject(const int viewport[4], const float winPos[3], float objCoord[3]) {
	// ����Ļ����ת��Ϊ OpenGL ��ʾ
	CVector3 vec3d;
	vec3d[0] = (winPos[0] - viewport[0]) / static_cast<float>(viewport[2]) * 2.0f - 1.0f;
	vec3d[1] = (winPos[1] - viewport[1]) / static_cast<float>(viewport[3]) * 2.0f - 1.0f;
	vec3d[2] = 2.0f * winPos[2] - 1.0f;
	// ������ͼͶӰ���������
	CMatrix4 matrix = m_pCamera->GetProjMatrix() * m_pCamera->GetViewMatrix();
	matrix.Invert();
	// �任�õ���������ϵ����
	vec3d = matrix * vec3d;
	vec3d *= 1.0f / vec3d[3];
	objCoord[0] = vec3d[0];
	objCoord[1] = vec3d[1];
	objCoord[2] = vec3d[2];
}

/**
* ���Ƴ���
*/
void CGraphicsManager::Draw() {
	// ��Ⱦ�����������Ӱ��ͼ�ͷ�����ͼ
	CTexture* pRenderTarget = m_pRenderTarget;
	CEngine::GetTextureManager()->Update();
	if (m_bShadowEnable) DrawShadowMap();
	DrawReflectMap();
	// ׼���������
	CPostProcessManager* pPostProcessMgr = CEngine::GetPostProcessManager();
	if (!pPostProcessMgr->PrepareFrame(pRenderTarget)) SetRenderTarget(pRenderTarget, 0, false, false, false);
	// ��ȡ��ȾĿ����
	int viewWidth = m_iWindowSize[0];
	int viewHeight = m_iWindowSize[1];
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
	// ����������ʾģʽ
	if (m_bStereoMode) {
		viewWidth >>= 1;
		m_pCamera->SetViewSize(viewWidth, viewHeight);
		m_pCamera->UpdateProjMatrix(false);

		// �����ۼ�� 63.6 mm
		CMatrix4& viewMat = m_pCamera->GetViewMatrix();
		CVector3 eyePos = m_pCamera->m_cPosition;
		CVector3 eyeDir = m_pCamera->m_cLookVector;
		CVector3 rightDir = CVector3(viewMat(0, 0), viewMat(0, 1), viewMat(0, 2)) * 0.0318f;

		// ��Ⱦ���۳���
		m_bRenderLeftEye = true;
		m_pCamera->m_cPosition = eyePos - rightDir;
		m_pCamera->m_cLookVector = (eyeDir * m_fFocusDistance + rightDir).Normalize();
		m_pCamera->UpdateViewMatrix();
		m_pCamera->UpdateFrustum();
		glViewport(0, 0, viewWidth, viewHeight);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		DrawScreen();

		// ��Ⱦ���۳���
		m_bRenderLeftEye = false;
		m_pCamera->m_cPosition = eyePos + rightDir;
		m_pCamera->m_cLookVector = (eyeDir * m_fFocusDistance - rightDir).Normalize();
		m_pCamera->UpdateViewMatrix();
		m_pCamera->UpdateFrustum();
		glViewport(viewWidth, 0, viewWidth, viewHeight);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
		DrawScreen();

		// �ָ����λ��
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
	// ���к���
	pPostProcessMgr->ApplyFrame();
	// ���� GUI Ԫ��
	CGuiEnvironment::GetInstance()->Render();
}

/**
* ������ĻԪ��
*/
void CGraphicsManager::DrawScreen() {
	// ������Ļԭ��ƫ��
	float offsetx = -0.5f * m_pCamera->GetViewWidth();
	float offsety = -0.5f * m_pCamera->GetViewHeight();
	CVector2 offset(offsetx, offsety);
	// ��������ͶӰ
	m_pCamera->UpdateProjMatrix(true);
	glDisable(GL_DEPTH_TEST);
	CEngine::GetSceneManager()->RenderScreen(m_pCamera->GetProjMatrix(), offset);
	// ���� LOGO
#ifdef AVATAR_ENABLE_LOGO
	CEngine::GetTextureManager()->GetTexture("logo")->UseTexture(0);
	CShader* pShader = CEngine::GetShaderManager()->GetCurrentShader();
	pShader->SetUniform("uOffset", offset.Add(32.0f).Scale(-1.0f));
	pShader->SetUniform("uModelMatrix", CMatrix4().Scale(32.0f, 32.0f, 1.0f));
	DrawQuadrilateral(CColor::White, true);
#endif
	// ����͸��ͶӰ
	m_pCamera->UpdateProjMatrix(false);
	glEnable(GL_DEPTH_TEST);
}

/**
* ������Ӱ��ͼ
*/
void CGraphicsManager::DrawShadowMap() {
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTexture* pShadowMap = pTextureMgr->GetTexture("__shadowmap__");
	CTexture* pShadowMapBlur = pTextureMgr->GetTexture("__shadowmap_blur__");
	CShader* pShaderBlur = pShaderMgr->GetShader("shadowmapfilter");
	// ��ʼ����Ӱ��ͼ��Դ
	if (!pShadowMap) {
		const int mapSize = 1024;
		pShadowMap = pTextureMgr->Create("__shadowmap__", mapSize, mapSize, true, true, false);
		pShadowMap->SetWrapModeClampToEdge(true, true);
		pShadowMapBlur = pTextureMgr->Create("__shadowmap_blur__", mapSize, mapSize, true, false, false);
		pShadowMapBlur->SetWrapModeClampToEdge(true, true);
		pShaderBlur->UseShader();
		pShaderBlur->SetUniform("uTextureSize", CVector2(mapSize, mapSize));
	}
	// �����Դ��ͼͶӰ����
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
	// ��Ⱦ��Ӱ��ͼ
	glDisable(GL_BLEND);
	m_bRenderDepth = true;
	CMatrix4 viewProj = projMat * viewMat;
	SetRenderTarget(pShadowMap, 0, true, true, true);
	CEngine::GetSceneManager()->RenderDepth(viewProj, "shadowmap");
	m_bRenderDepth = false;
	// ����Ӱ��ͼ����ģ��
	SetRenderTarget(pShadowMapBlur, 0, true, false, false);
	pShadowMap->UseTexture();
	pShaderBlur->UseShader();
	DrawQuadrilateral(CColor::White, false);
	glEnable(GL_BLEND);
	// ������Ӱ��ͼ����Ԫ
	pShadowMapBlur->UseTexture(8);
	glActiveTexture(GL_TEXTURE0);
	// ��Ӱ���������ƫ�ƣ���ǰ��������������
	const float biasMat[16] = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	};
	CMatrix4 shadowMat = CMatrix4(biasMat) * viewProj;
	// ���������ɫ������Ӱ����
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
* ����ȫ��ͼ
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
	// ȡ����ڵ�ǰ�����λ��6������
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
	// +X �����Ѿ�Ĭ�ϰ󶨲��������ɫ�����
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
* ��Ⱦ������ͼ
*/
void CGraphicsManager::DrawReflectMap() {
	vector<CSceneNode*> nodeList;
	CEngine::GetSceneManager()->GetNodeList("water", nodeList);
	if (nodeList.empty()) return;
	// �������������
	m_bRenderReflect = true;
	CMatrix4 projMat = m_pCamera->GetProjMatrix();
	CMatrix4 viewMat = m_pCamera->GetViewMatrix();
	CVector3 cameraPos = m_pCamera->m_cPosition;
	CVector3 cameraLook = m_pCamera->m_cLookVector;
	CVector3 cameraUp = m_pCamera->m_cUpVector;
	// ��Ⱦ����ˮ�浹Ӱ
	for (size_t i = 0; i < nodeList.size(); i++) {
		CSceneNode* node = nodeList[i];
		// ��ȡˮ��ƽ��
		CPlane reflectPlane(CVector3(0.0f, 0.0f, 1.0f), 0.0f);
		reflectPlane.Transform(node->m_cWorldMatrix);
		if (reflectPlane.Distance(cameraPos) < 0.0f) reflectPlane.FlipNormal();
		// ����ˮ�淴��ͼ�������
		CMatrix4 reflectMat = reflectPlane.GetReflectMatrix();
		m_pCamera->m_cPosition = reflectPlane.GetMirrorPoint(cameraPos);
		m_pCamera->m_cLookVector = reflectMat * CVector3(-viewMat(2, 0), -viewMat(2, 1), -viewMat(2, 2), 0.0f);
		m_pCamera->m_cUpVector = reflectMat * CVector3(viewMat(1, 0), viewMat(1, 1), viewMat(1, 2), 0.0f);
		m_pCamera->GetViewMatrix().LookAt(m_pCamera->m_cPosition, m_pCamera->m_cLookVector, m_pCamera->m_cUpVector);
		// ����б�ü�ͶӰ����
		reflectPlane.Transform(m_pCamera->GetViewMatrix());
		m_pCamera->GetProjMatrix().ObliqueNearPlaneClipping(m_pCamera->GetProjMatrix(), reflectPlane);
		m_pCamera->UpdateFrustum();
		// ��Ⱦ����������
		CTexture* reflectTexture = CEngine::GetTextureManager()->GetTexture(node->GetName() + "_reflect");
		SetRenderTarget(reflectTexture, 0, true, true, true);
		CEngine::GetSceneManager()->RenderScene(m_pCamera->GetViewMatrix(), m_pCamera->GetProjMatrix());
	}
	// �ָ����������
	m_bRenderReflect = false;
	m_pCamera->m_cPosition.SetValue(cameraPos);
	m_pCamera->m_cLookVector.SetValue(cameraLook);
	m_pCamera->m_cUpVector.SetValue(cameraUp);
	m_pCamera->GetProjMatrix().SetValue(projMat);
	m_pCamera->GetViewMatrix().SetValue(viewMat);
	m_pCamera->UpdateFrustum();
}