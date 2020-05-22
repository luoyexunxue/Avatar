//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeText.h"
#include "CGeometryCreator.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include <cstring>
#include <cmath>

/**
* ���캯��
*/
CSceneNodeText::CSceneNodeText(const string& name, const wchar_t* text, const CColor& color,
	const CColor& outline, const string& font, int fontSize) : CSceneNode("text", name) {
	wcsncpy(m_strText, text, 128);
	m_cColor.SetValue(color);
	m_cOutline.SetValue(outline);
	m_strFont = font;
	m_iFontSize = fontSize;
	m_pMesh = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeText::Init() {
	m_pMesh = CGeometryCreator::CreatePlane(1.0f, 1.0f, 3);
	UpdateText();
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeText::Destroy() {
	delete m_pMesh;
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeText::Render() {
	m_pMesh->Render();
}

/**
* ���³����ڵ�
*/
void CSceneNodeText::Update(float dt) {
	m_cModelMatrix.MakeTransform(m_cScale, CMatrix4::Identity, m_cPosition);
	if (!m_pParent) m_cWorldMatrix = m_cModelMatrix;
	else m_cWorldMatrix = m_pParent->m_cWorldMatrix * m_cModelMatrix;
	// MVP = Proj * View * (View-1 * Model)
	CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
	CMatrix4 viewMat = pCamera->GetViewMatrix();
	CMatrix4 mvMat = viewMat * m_cWorldMatrix;
	float fov = pCamera->GetFieldOfView() * (3.141593f / 360.0f);
	float scale = -2.0f * mvMat(2, 3) * tanf(fov) / pCamera->GetViewWidth();
	// ȥ����ת���ŷ���
	mvMat(0, 0) = m_cScale[0] * scale;
	mvMat(1, 1) = m_cScale[1] * scale;
	mvMat(2, 2) = m_cScale[2] * scale;
	mvMat(3, 3) = 1.0f;
	mvMat(0, 1) = 0.0f;
	mvMat(1, 0) = 0.0f;
	mvMat(0, 2) = 0.0f;
	mvMat(2, 0) = 0.0f;
	mvMat(1, 2) = 0.0f;
	mvMat(2, 1) = 0.0f;
	m_cWorldMatrix = viewMat.Invert() * mvMat;
}

/**
* �����ı�
*/
void CSceneNodeText::SetText(const wchar_t* text) {
	wcsncpy(m_strText, text, 128);
	UpdateText();
}

/**
* ��������
*/
void CSceneNodeText::SetFont(const string& font, int fontSize) {
	m_strFont = font;
	m_iFontSize = fontSize;
	UpdateText();
}

/**
* �����ı���ɫ
*/
void CSceneNodeText::SetColor(const CColor& color, const CColor& outline) {
	m_cColor.SetValue(color);
	m_cOutline.SetValue(outline);
	UpdateText();
}

/**
* �����ı�
*/
void CSceneNodeText::UpdateText() {
	// ��ȡ�ı���С
	int width = 0;
	int height = 0;
	CFontManager* pFontMgr = CEngine::GetFontManager();
	pFontMgr->UseFont(m_strFont);
	pFontMgr->SetSize(m_iFontSize);
	pFontMgr->TextSize(m_strText, &width, &height);
	// �����ı�
	CFontManager::CTextImage image(width, height);
	pFontMgr->DrawText(m_strText, &image, CFontManager::TOPLEFT, false);
	int extend = static_cast<int>(m_iFontSize * 0.1f);
	int textureWidth = width + extend * 2;
	int textureHeight = height + extend * 2;
	unsigned char color[4];
	unsigned char* buffer = new unsigned char[textureWidth * textureHeight * 4];
	memset(buffer, 0, textureWidth * textureHeight * 4);
	// ����8�Σ��õ�����
	m_cOutline.GetValue(color);
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			if (x == 1 && y == 1) continue;
			for (int h = 0; h < height; h++) {
				for (int w = 0; w < width; w++) {
					unsigned int index = ((h + y * extend) * textureWidth + (w + x * extend)) << 2;
					unsigned char pixel = image.data[h * width + w];
					buffer[index + 0] = (pixel * color[0] + (0xFF - pixel) * buffer[index + 0]) >> 8;
					buffer[index + 1] = (pixel * color[1] + (0xFF - pixel) * buffer[index + 1]) >> 8;
					buffer[index + 2] = (pixel * color[2] + (0xFF - pixel) * buffer[index + 2]) >> 8;
					buffer[index + 3] = (pixel * color[3] + (0xFF - pixel) * buffer[index + 3]) >> 8;
				}
			}
		}
	}
	// �ı����
	m_cColor.GetValue(color);
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			unsigned int index = ((h + extend) * textureWidth + (w + extend)) << 2;
			unsigned char pixel = image.data[h * width + w];
			buffer[index + 0] = (pixel * color[0] + (0xFF - pixel) * buffer[index + 0]) >> 8;
			buffer[index + 1] = (pixel * color[1] + (0xFF - pixel) * buffer[index + 1]) >> 8;
			buffer[index + 2] = (pixel * color[2] + (0xFF - pixel) * buffer[index + 2]) >> 8;
			buffer[index + 3] = (pixel * color[3] + (0xFF - pixel) * buffer[index + 3]) >> 8;
		}
	}
	m_pMesh->GetMaterial()->SetTexture("", textureWidth, textureHeight, 4, buffer);
	m_cScale.SetValue(static_cast<float>(textureWidth), static_cast<float>(textureHeight), 1.0f);
	delete[] buffer;
}