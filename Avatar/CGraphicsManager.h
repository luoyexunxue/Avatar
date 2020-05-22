//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
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
* @brief ͼ����Ⱦ������
*/
class AVATAR_EXPORT CGraphicsManager {
public:
	//! ��ȡ������ʵ��
	static CGraphicsManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CGraphicsManager();
		return m_pInstance;
	}
	//! ʵ������
	void Destroy();

	//! ��Ⱦһ֡
	void Render();

	//! �������
	void SetCamera(CCamera* camera);
	//! ��ȡ���
	CCamera* GetCamera();
	//! ���ô��ڴ�С
	void SetWindowSize(int width, int height);
	//! ��ȡ���ڴ�С
	void GetWindowSize(int* width, int* height);
	//! ���ù�Դ
	void SetLight(const CVector3& pos, const CVector3& dir, const CVector3& color);
	//! ��ȡ��Դλ��
	CVector3 GetLight();
	//! �����Ƿ�������ʾ
	void SetStereoMode(bool stereo);
	//! ��ȡ�Ƿ�������ʾ
	bool GetStereoMode();
	//! �����Ƿ���ʾ��Ӱ
	void SetShadowEnable(bool enable);
	//! ��ȡ��Ӱ�Ƿ���ʾ
	bool GetShadowEnable();
	//! ��������ʾ������
	void SetFogEnable(bool enable, float start, float end, const CColor& color);
	//! ��ȡ���Ƿ���ʾ
	bool GetFogEnable();
	//! �������û�������
	void SetEnvironmentMapEnable(bool enable, const string& cubemap);
	//! ��ȡ�Ƿ�����������
	bool GetEnvironmentMapEnable();

	//! �����Ƿ�������Ȳ���
	void SetDepthTestEnable(bool enable);
	//! �����Ƿ��������ƫ��
	void SetDepthOffsetEnable(bool enable, float offset);
	//! �����Ƿ�������ɫ���
	void SetColorBlendEnable(bool enable);
	//! ���ó�ʼ������ɫ
	void SetBackgroundColor(const CColor& color);
	//! ���������Ӿ�����
	void SetFocusDistance(float distance);

	//! �ж��Ƿ��������Ⱦģʽ
	bool IsDepthRender();
	//! �ж��Ƿ��ڷ���ͼ��Ⱦ
	bool IsReflectRender();
	//! �ж��Ƿ���������Ⱦ
	bool IsLeftEyeRender();

	//! ��Ļ��ͼ
	void Screenshot(CFileManager::CImageFile& file, bool redraw);
	//! ��Ļ��ͼ���������ļ�
	void Screenshot(const string& fileName, bool redraw);
	//! ��ȡ�����Ļ����
	void PointToScreen(const CVector3& point, CVector3& screen);
	//! ����ʰȡ����
	void PickingRay(int x, int y, CRay& ray);
	//! ��ȡʰȡ������
	void PickingPosition(int x, int y, CVector3& position);

	//! ���Ƶ�
	void DrawPoints(const CVertex* vertices, int size);
	//! ������
	void DrawLines(const CVertex* vertices, int size, float width, bool strip);
	//! ����������
	void DrawTriangles(const CVertex* vertices, int size, bool strip);
	//! �����ı���
	void DrawQuadrilateral(const CColor& color, bool flipTexCoord);
	//! ����������
	void DrawCube(const CColor& color);

	//! ���õ�ǰ��ȾĿ��
	void SetRenderTarget(CTexture* texture, int level, bool fullView, bool clearColor, bool clearDepth);
	//! ��ȡ��ǰ��ȾĿ��
	CTexture* GetRenderTarget();

	//! ��ȡ֡��
	float GetFrameRate();

private:
	CGraphicsManager();
	~CGraphicsManager();

	//! ��������ϵ����Ļ����ϵ�ı任
	void Project(const int viewport[4], const float objCoord[3], float winPos[3]);
	//! ��Ļ����ϵ����������ϵ�ı任
	void UnProject(const int viewport[4], const float winPos[3], float objCoord[3]);

	//! ���Ƴ���
	void Draw();
	//! ������ĻԪ��
	void DrawScreen();
	//! ������Ӱ��ͼ
	void DrawShadowMap();
	//! ����ȫ��ͼ
	void DrawCubeMap();
	//! ��Ⱦ������ͼ
	void DrawReflectMap();

private:
	//! ���ڿ��
	int m_iWindowSize[2];
	//! ����ģʽ
	bool m_bStereoMode;
	//! ��Ӱ��ʾ
	bool m_bShadowEnable;
	//! ����ʾ
	bool m_bFogEnable;
	//! ��������
	bool m_bEnvironmentMapEnable;
	//! ������ɫ
	CColor m_cBackground;
	//! ȫ�ֹ�Դλ��
	CVector3 m_cLightPos;
	//! ȫ�ֹ��շ���
	CVector3 m_cLightDir;
	//! ���
	CCamera* m_pCamera;
	//! ��ǰ��ȾĿ��
	CTexture* m_pRenderTarget;

	//! ��Ⱦ���ۣ�����ģʽ��
	bool m_bRenderLeftEye;
	//! ���ͼ��Ⱦ
	bool m_bRenderDepth;
	//! ����ͼ��Ⱦ
	bool m_bRenderReflect;
	//! ����
	float m_fFocusDistance;
	//! ֡��
	float m_fFrameRate;
	//! ʵ��
	static CGraphicsManager* m_pInstance;
};

#endif