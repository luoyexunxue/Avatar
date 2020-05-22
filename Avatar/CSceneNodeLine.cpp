//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeLine.h"
#include "CEngine.h"

/**
* ���캯��
*/
CSceneNodeLine::CSceneNodeLine(const string& name, const CColor& color, float width)
	: CSceneNode("line", name) {
	m_cColor.SetValue(color);
	m_fLineWidth = width;
	m_bShowPoints = false;
	m_bDisableDepth = false;
	m_bScreenSpace = false;
	m_bSegmentMode = false;
	m_pShader = 0;
}

/**
* ��ʼ�������ڵ�
*/
bool CSceneNodeLine::Init() {
	return true;
}

/**
* ���ٳ����ڵ�
*/
void CSceneNodeLine::Destroy() {
	CEngine::GetShaderManager()->Drop(m_pShader);
}

/**
* ��Ⱦ�����ڵ�
*/
void CSceneNodeLine::Render() {
	if (m_vecVertices.empty()) return;
	CShader* shader = m_pShader;
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	if (shader) {
		CCamera* pCamera = pGraphicsMgr->GetCamera();
		shader->UseShader();
		shader->SetUniform("uProjMatrix", pCamera->GetProjMatrix());
		shader->SetUniform("uViewMatrix", pCamera->GetViewMatrix());
		shader->SetUniform("uCameraPos", pCamera->m_cPosition);
		shader->SetUniform("uModelMatrix", m_cWorldMatrix);
	} else shader = CEngine::GetShaderManager()->GetCurrentShader();
	// ���ݱ�־����ѡ���Ի���
	if (m_bShowPoints) {
		shader->SetUniform("uPointSize", m_fPointSize);
		if (m_bDisableDepth) pGraphicsMgr->SetDepthTestEnable(false);
		pGraphicsMgr->DrawPoints(&m_vecVertices[0], m_vecVertices.size());
		if (m_bDisableDepth) pGraphicsMgr->SetDepthTestEnable(true);
	} else {
		if (m_bDisableDepth) pGraphicsMgr->SetDepthTestEnable(false);
		pGraphicsMgr->DrawLines(&m_vecVertices[0], m_vecVertices.size(), m_fLineWidth, !m_bSegmentMode);
		if (m_bDisableDepth) pGraphicsMgr->SetDepthTestEnable(true);
	}
}

/**
* ���³����ڵ�
*/
void CSceneNodeLine::Update(float dt) {
	if (m_bScreenSpace) {
		CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
		float screen_w = static_cast<float>(pCamera->GetViewWidth());
		float screen_h = static_cast<float>(pCamera->GetViewHeight());
		// MVP = Proj * View * (View-1 * Proj-1 * OrthoProj * Offset)
		CMatrix4 mvp;
		mvp.Ortho(screen_w, screen_h, -1.0f, 1.0f);
		mvp(0, 3) = -0.5f * mvp(0, 0) * screen_w;
		mvp(1, 3) = -0.5f * mvp(1, 1) * screen_h;
		CMatrix4 viewProj = pCamera->GetProjMatrix() * pCamera->GetViewMatrix();
		m_cWorldMatrix = viewProj.Invert() * mvp;
	}
}

/**
* ��Ӷ���
*/
void CSceneNodeLine::AddPoint(const CVector3& position) {
	CVertex vertex;
	vertex.SetPosition(position);
	vertex.SetColor(m_cColor);
	if (m_bScreenSpace) vertex.m_fPosition[2] = 0.0f;
	if (!m_vecVertices.empty()) {
		CVertex& last = m_vecVertices.back();
		CVector3 len = position - CVector3(last.m_fPosition);
		CVector2 uv = CVector2(last.m_fTexCoord[0] + len.Length());
		vertex.SetTexCoord(uv);
	}
	m_vecVertices.push_back(vertex);
}

/**
* ��ն���
*/
void CSceneNodeLine::ClearPoint() {
	m_vecVertices.clear();
}

/**
* ʹ�� B ��������ƽ��
*/
void CSceneNodeLine::SmoothLine(float ds) {
	if (m_vecVertices.size() < 2) return;
	if (ds <= 0.0f) ds = 0.001f;

	CVector3 point[4];
	point[0] = m_vecVertices[0].m_fPosition;
	point[1] = m_vecVertices[0].m_fPosition;
	point[2] = m_vecVertices[0].m_fPosition;
	point[3] = m_vecVertices[0].m_fPosition;
	// �����б�������ֵ
	vector<CVector3> vecSpline;
	for (size_t i = 0; i < m_vecVertices.size() + 3; i++) {
		size_t cur = i < m_vecVertices.size() ? i : m_vecVertices.size() - 1;
		point[0] = point[1];
		point[1] = point[2];
		point[2] = point[3];
		point[3] = m_vecVertices[cur].m_fPosition;
		BSpline(point, static_cast<int>((point[2] - point[1]).Length() / ds), vecSpline);
	}
	// ����������б�
	CVertex vertex;
	vertex.SetPosition(vecSpline[0]);
	vertex.SetColor(m_cColor);
	m_vecVertices.clear();
	m_vecVertices.push_back(vertex);
	for (size_t i = 1; i < vecSpline.size(); i++) {
		vertex.SetPosition(vecSpline[i]);
		vertex.SetColor(m_cColor);
		CVector3 len = vecSpline[i] - vecSpline[i - 1];
		CVector2 uv = CVector2(m_vecVertices.back().m_fTexCoord[0] + len.Length());
		vertex.SetTexCoord(uv);
		m_vecVertices.push_back(vertex);
	}
}

/**
* ����ʾ����
*/
void CSceneNodeLine::ShowPoints(bool show, float pointSize) {
	m_bShowPoints = show;
	m_fPointSize = pointSize;
}

/**
* ������Ȳ���
*/
void CSceneNodeLine::DisableDepth(bool disable) {
	m_bDisableDepth = disable;
}

/**
* ʹ����Ļ�������
* @attention ����xy���굥λΪ����
*/
void CSceneNodeLine::ScreenSpace(bool enable) {
	m_bScreenSpace = enable;
}

/**
* �߶�ģʽ����
*/
void CSceneNodeLine::Segment(bool enable) {
	m_bSegmentMode = enable;
}

/**
* �����Զ�����ɫ��
*/
void CSceneNodeLine::SetShader(const string& shader) {
	m_pShader = CEngine::GetShaderManager()->GetShader(shader);
	m_pShader->AddReference();
}

/**
* ��ȡ�������
*/
int CSceneNodeLine::GetPointCount() {
	return m_vecVertices.size();
}

/**
* ��ȡָ������
*/
CVertex* CSceneNodeLine::GetPoint(unsigned int index) {
	if (index >= m_vecVertices.size()) return 0;
	return &m_vecVertices[index];
}

/**
* ���� B �������߲�ֵ
*/
void CSceneNodeLine::BSpline(const CVector3 pt[4], int divisions, vector<CVector3>& spline) {
	float basis[4][3];
	// ��ά����
	for (int i = 0; i < 3; i++) {
		basis[0][i] = (-pt[0].m_fValue[i] + 3.0f * pt[1].m_fValue[i] - 3.0f * pt[2].m_fValue[i] + pt[3].m_fValue[i]) / 6.0f;
		basis[1][i] = (3.0f * pt[0].m_fValue[i] - 6.0f * pt[1].m_fValue[i] + 3.0f * pt[2].m_fValue[i]) / 6.0f;
		basis[2][i] = (-3.0f * pt[0].m_fValue[i] + 3.0f * pt[2].m_fValue[i]) / 6.0f;
		basis[3][i] = (pt[0].m_fValue[i] + 4.0f * pt[1].m_fValue[i] + pt[2].m_fValue[i]) / 6.0f;
	}
	const float step = 1.0f / divisions;
	spline.push_back(CVector3(basis[3]));
	for (int i = 0; i < divisions; i++) {
		float t = i * step;
		float x = (basis[2][0] + t * (basis[1][0] + t * basis[0][0])) * t + basis[3][0];
		float y = (basis[2][1] + t * (basis[1][1] + t * basis[0][1])) * t + basis[3][1];
		float z = (basis[2][2] + t * (basis[1][2] + t * basis[0][2])) * t + basis[3][2];
		spline.push_back(CVector3(x, y, z));
	}
}