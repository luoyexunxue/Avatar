//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CInputManager.h"
#include "CGuiEnvironment.h"
#include <cstring>

/**
* ���캯��
*/
CInputManager::CInputManager() {
	m_pInput = new SInput();
	m_pBufferedInput = new SInput();
	m_iMouseLastButton = 0;
	m_iMouseLastPos[0] = -1;
	m_iMouseLastPos[1] = -1;
	m_iMouseClickPos[0] = -1;
	m_iMouseClickPos[1] = -1;
	m_iMouseButtonMap[0] = 0;
	m_iMouseButtonMap[1] = 1;
	m_iMouseButtonMap[2] = 2;
	m_iMouseButtonMap[3] = 3;
	m_fMouseInputScale[0] = 0.01f;
	m_fMouseInputScale[1] = 0.005f;
	m_bDisableMouse = false;
	m_bDisableKeyboard = false;
	m_bDisableGravity = true;
	m_fGravityScale[0] = 1.0f;
	m_fGravityScale[1] = 1.0f;
	m_fGravityScale[2] = 1.0f;
}

/**
* ��������
*/
CInputManager::~CInputManager() {
	m_pInstance = 0;
}

/**
* ����ʵ��
*/
CInputManager* CInputManager::m_pInstance = 0;

/**
* ʵ������
*/
void CInputManager::Destroy() {
	delete m_pInput;
	delete m_pBufferedInput;
	delete this;
}

/**
* ��ȡ����
*/
CInputManager::SInput* CInputManager::GetInput() {
	return m_pInput;
}

/**
* �������
*/
void CInputManager::Update() {
	// ����������븴�Ƶ���ǰ��������
	memcpy(m_pInput, m_pBufferedInput, sizeof(SInput));
	m_pInput->bMove = m_pInput->fRightLeft != 0.0f || m_pInput->fForthBack != 0.0f || m_pInput->fUpDown != 0.0f;
	m_pInput->bTurn = m_pInput->fYaw != 0.0f || m_pInput->fPitch != 0.0f || m_pInput->fRoll != 0.0f;
	// �������뻺��
	m_pBufferedInput->bFire = false;
	m_pBufferedInput->bJump = false;
	m_pBufferedInput->bMove = false;
	m_pBufferedInput->bTurn = false;
	m_pBufferedInput->bPosition = false;
	m_pBufferedInput->bOrientation = false;
	m_pBufferedInput->bGravity = false;
	m_pBufferedInput->fRightLeft = 0;
	m_pBufferedInput->fForthBack = 0;
	m_pBufferedInput->fUpDown = 0;
	m_pBufferedInput->fYaw = 0;
	m_pBufferedInput->fPitch = 0;
	m_pBufferedInput->fRoll = 0;
	m_pBufferedInput->iFunction = 0;
	m_pBufferedInput->iInputKey = 0;
	m_pBufferedInput->iWidth = 0;
	m_pBufferedInput->iHeight = 0;
}

/**
* �����ƶ�
*/
void CInputManager::RightLeft(float step) {
	m_pBufferedInput->fRightLeft += step;
}

/**
* ǰ���ƶ�
*/
void CInputManager::ForthBack(float step) {
	m_pBufferedInput->fForthBack += step;
}

/**
* �����ƶ�
*/
void CInputManager::UpDown(float step) {
	m_pBufferedInput->fUpDown += step;
}

/**
* ��λ������
*/
void CInputManager::Yaw(float angle) {
	m_pBufferedInput->fYaw += angle;
}

/**
* ����������
*/
void CInputManager::Pitch(float angle) {
	m_pBufferedInput->fPitch += angle;
}

/**
* ����������
*/
void CInputManager::Roll(float angle) {
	m_pBufferedInput->fRoll += angle;
}

/**
* λ������
*/
void CInputManager::Position(float x, float y, float z) {
	m_pBufferedInput->fPosition[0] = x;
	m_pBufferedInput->fPosition[1] = y;
	m_pBufferedInput->fPosition[2] = z;
	m_pBufferedInput->bPosition = true;
}

/**
* ��λ���루��Ԫ����
*/
void CInputManager::Orientation(float x, float y, float z, float w) {
	m_pBufferedInput->fOrientation[0] = x;
	m_pBufferedInput->fOrientation[1] = y;
	m_pBufferedInput->fOrientation[2] = z;
	m_pBufferedInput->fOrientation[3] = w;
	m_pBufferedInput->bOrientation = true;
}

/**
* ����
*/
void CInputManager::Fire() {
	m_pBufferedInput->bFire = true;
}

/**
* ��Ծ
*/
void CInputManager::Jump() {
	m_pBufferedInput->bJump = true;
}

/**
* ��ʼ
*/
void CInputManager::Start() {
	m_pBufferedInput->bPause = false;
}

/**
* ��ͣ
*/
void CInputManager::Pause() {
	m_pBufferedInput->bPause = true;
}

/**
* �˳�
*/
void CInputManager::Quit() {
	m_pBufferedInput->bQuit = true;
}

/**
* ��һ��״̬
*/
void CInputManager::NextState() {
	m_pBufferedInput->iState++;
}

/**
* ��һ��״̬
*/
void CInputManager::PrevState() {
	m_pBufferedInput->iState--;
}

/**
* ��λ״̬
*/
void CInputManager::ResetState() {
	m_pBufferedInput->iState = 0;
}

/**
* ����ѡ��
* @param func ���ܼ� 1,2,3����
*/
void CInputManager::Function(int func) {
	m_pBufferedInput->iFunction = func;
}

/**
* ���ڴ�С�ı�
*/
void CInputManager::Resize(int width, int height) {
	m_pBufferedInput->iWidth = width;
	m_pBufferedInput->iHeight = height;
}

/**
* �������
* @param x ��Ļ���� X ֵ
* @param y ��Ļ���� Y ֵ
* @param button 0 �ް�����1 ������£�2 �Ҽ����£�3 �м�����
* @param delta ���ֹ���ֵ
*/
void CInputManager::MouseInput(int x, int y, int button, int delta) {
	m_pBufferedInput->iInputX = x;
	m_pBufferedInput->iInputY = y;
	int dx = x - m_iMouseLastPos[0];
	int dy = y - m_iMouseLastPos[1];
	m_iMouseLastPos[0] = x;
	m_iMouseLastPos[1] = y;

	// ����Ƿ�Ϊ GUI ����
	if (!CGuiEnvironment::GetInstance()->MouseEvent(x, y, button, delta) && !m_bDisableMouse) {
		button = m_iMouseButtonMap[button];
		m_pBufferedInput->fForthBack += delta * m_fMouseInputScale[0] * 100.0f;
		if (dx != 0 || dy != 0) {
			m_iMouseClickPos[0] = -1;
			m_iMouseClickPos[1] = -1;
			if (button == 1) {
				m_pBufferedInput->fYaw += dx * m_fMouseInputScale[1];
				m_pBufferedInput->fPitch += dy * m_fMouseInputScale[1];
			} else if (button == 2) {
				m_pBufferedInput->fRightLeft -= dx * m_fMouseInputScale[0];
				m_pBufferedInput->fUpDown += dy * m_fMouseInputScale[0];
			}
		} else {
			if (button == 1 && m_iMouseLastButton == 0) {
				m_iMouseClickPos[0] = x;
				m_iMouseClickPos[1] = y;
			} else if (button == 0 && m_iMouseLastButton == 1) {
				bool clicked = x == m_iMouseClickPos[0] && y == m_iMouseClickPos[1];
				m_pBufferedInput->bFire = clicked;
			}
		}
		m_iMouseLastButton = button;
	}
}

/**
* ��������
* @param key ����ֵ
*/
void CInputManager::KeyboardInput(int key) {
	// ����Ƿ�Ϊ GUI ����
	if (!CGuiEnvironment::GetInstance()->KeyboardEvent(key) && !m_bDisableKeyboard) {
		m_pBufferedInput->iInputKey = key;
		// ��ĸת��Ϊ��д
		if (key > 0x60 && key < 0x7B) key -= 0x20;
		switch (key) {
		case 0x1B: m_pBufferedInput->bQuit = true; break;
		case ' ': m_pBufferedInput->bJump = true; break;
		case 'F': m_pBufferedInput->bFire = true; break;
		case 'P': m_pBufferedInput->bPause = !m_pBufferedInput->bPause; break;
		case '+': m_pBufferedInput->iState++; break;
		case '-': m_pBufferedInput->iState--; break;
		default: break;
		}
	}
}

/**
* ��������
* @param x ���� X ����
* @param y ���� Y ����
* @param z ���� Z ����
*/
void CInputManager::GravityInput(float x, float y, float z) {
	if (!m_bDisableGravity) {
		m_pBufferedInput->fGravity[0] = x * m_fGravityScale[0];
		m_pBufferedInput->fGravity[1] = y * m_fGravityScale[1];
		m_pBufferedInput->fGravity[2] = z * m_fGravityScale[2];
		m_pBufferedInput->bGravity = true;
	}
}

/**
* ���ÿ�������
* @param mouse �Ƿ�����������
* @param keyboard �Ƿ���ü�������
* @param gravity �Ƿ������������
*/
void CInputManager::DisableInput(bool mouse, bool keyboard, bool gravity) {
	m_bDisableMouse = mouse;
	m_bDisableKeyboard = keyboard;
	m_bDisableGravity = gravity;
}

/**
* ��갴��ӳ��
* @param left ������ӳ��ֵ��Ĭ��Ϊ 1
* @param right ����Ҽ�ӳ��ֵ��Ĭ��Ϊ 2
* @param middle ����м�ӳ��ֵ��Ĭ��Ϊ 3
*/
void CInputManager::MapMouseButton(int left, int right, int middle) {
	m_iMouseButtonMap[1] = left;
	m_iMouseButtonMap[2] = right;
	m_iMouseButtonMap[3] = middle;
}

/**
* ��ȡ���������������Ʊ�����С
* @param set �������������
* @param move �ƶ��ٶ�
* @param turn ת���ٶ�
*/
void CInputManager::MouseSensitivity(bool set, float& move, float& turn) {
	if (set) {
		m_fMouseInputScale[0] = move * 0.01f;
		m_fMouseInputScale[1] = turn * 0.005f;
	} else {
		move = m_fMouseInputScale[0];
		turn = m_fMouseInputScale[1];
	}
}

/**
* ������������ϵ��
* @param sx X����ϵ��
* @param sy Y����ϵ��
* @param sz Z����ϵ��
*/
void CInputManager::GravityScale(float sx, float sy, float sz) {
	m_fGravityScale[0] = sx;
	m_fGravityScale[1] = sy;
	m_fGravityScale[2] = sz;
}