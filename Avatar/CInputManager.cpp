//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CInputManager.h"
#include "CGuiEnvironment.h"
#include <cstring>

/**
* 构造函数
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
* 析构函数
*/
CInputManager::~CInputManager() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CInputManager* CInputManager::m_pInstance = 0;

/**
* 实例销毁
*/
void CInputManager::Destroy() {
	delete m_pInput;
	delete m_pBufferedInput;
	delete this;
}

/**
* 获取输入
*/
CInputManager::SInput* CInputManager::GetInput() {
	return m_pInput;
}

/**
* 输入更新
*/
void CInputManager::Update() {
	// 将缓冲的输入复制到当前输入体中
	memcpy(m_pInput, m_pBufferedInput, sizeof(SInput));
	m_pInput->bMove = m_pInput->fRightLeft != 0.0f || m_pInput->fForthBack != 0.0f || m_pInput->fUpDown != 0.0f;
	m_pInput->bTurn = m_pInput->fYaw != 0.0f || m_pInput->fPitch != 0.0f || m_pInput->fRoll != 0.0f;
	// 重置输入缓冲
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
* 右左移动
*/
void CInputManager::RightLeft(float step) {
	m_pBufferedInput->fRightLeft += step;
}

/**
* 前后移动
*/
void CInputManager::ForthBack(float step) {
	m_pBufferedInput->fForthBack += step;
}

/**
* 上下移动
*/
void CInputManager::UpDown(float step) {
	m_pBufferedInput->fUpDown += step;
}

/**
* 方位角输入
*/
void CInputManager::Yaw(float angle) {
	m_pBufferedInput->fYaw += angle;
}

/**
* 俯仰角输入
*/
void CInputManager::Pitch(float angle) {
	m_pBufferedInput->fPitch += angle;
}

/**
* 翻滚角输入
*/
void CInputManager::Roll(float angle) {
	m_pBufferedInput->fRoll += angle;
}

/**
* 位置输入
*/
void CInputManager::Position(float x, float y, float z) {
	m_pBufferedInput->fPosition[0] = x;
	m_pBufferedInput->fPosition[1] = y;
	m_pBufferedInput->fPosition[2] = z;
	m_pBufferedInput->bPosition = true;
}

/**
* 方位输入（四元数）
*/
void CInputManager::Orientation(float x, float y, float z, float w) {
	m_pBufferedInput->fOrientation[0] = x;
	m_pBufferedInput->fOrientation[1] = y;
	m_pBufferedInput->fOrientation[2] = z;
	m_pBufferedInput->fOrientation[3] = w;
	m_pBufferedInput->bOrientation = true;
}

/**
* 开火
*/
void CInputManager::Fire() {
	m_pBufferedInput->bFire = true;
}

/**
* 跳跃
*/
void CInputManager::Jump() {
	m_pBufferedInput->bJump = true;
}

/**
* 开始
*/
void CInputManager::Start() {
	m_pBufferedInput->bPause = false;
}

/**
* 暂停
*/
void CInputManager::Pause() {
	m_pBufferedInput->bPause = true;
}

/**
* 退出
*/
void CInputManager::Quit() {
	m_pBufferedInput->bQuit = true;
}

/**
* 下一个状态
*/
void CInputManager::NextState() {
	m_pBufferedInput->iState++;
}

/**
* 上一个状态
*/
void CInputManager::PrevState() {
	m_pBufferedInput->iState--;
}

/**
* 复位状态
*/
void CInputManager::ResetState() {
	m_pBufferedInput->iState = 0;
}

/**
* 功能选择
* @param func 功能键 1,2,3……
*/
void CInputManager::Function(int func) {
	m_pBufferedInput->iFunction = func;
}

/**
* 窗口大小改变
*/
void CInputManager::Resize(int width, int height) {
	m_pBufferedInput->iWidth = width;
	m_pBufferedInput->iHeight = height;
}

/**
* 鼠标输入
* @param x 屏幕坐标 X 值
* @param y 屏幕坐标 Y 值
* @param button 0 无按键，1 左键按下，2 右键按下，3 中键按下
* @param delta 滚轮滚动值
*/
void CInputManager::MouseInput(int x, int y, int button, int delta) {
	m_pBufferedInput->iInputX = x;
	m_pBufferedInput->iInputY = y;
	int dx = x - m_iMouseLastPos[0];
	int dy = y - m_iMouseLastPos[1];
	m_iMouseLastPos[0] = x;
	m_iMouseLastPos[1] = y;

	// 检查是否为 GUI 输入
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
* 键盘输入
* @param key 按键值
*/
void CInputManager::KeyboardInput(int key) {
	// 检查是否为 GUI 输入
	if (!CGuiEnvironment::GetInstance()->KeyboardEvent(key) && !m_bDisableKeyboard) {
		m_pBufferedInput->iInputKey = key;
		// 字母转换为大写
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
* 重力输入
* @param x 重力 X 分量
* @param y 重力 Y 分量
* @param z 重力 Z 分量
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
* 禁用控制输入
* @param mouse 是否禁用鼠标输入
* @param keyboard 是否禁用键盘输入
* @param gravity 是否禁用重力输入
*/
void CInputManager::DisableInput(bool mouse, bool keyboard, bool gravity) {
	m_bDisableMouse = mouse;
	m_bDisableKeyboard = keyboard;
	m_bDisableGravity = gravity;
}

/**
* 鼠标按键映射
* @param left 鼠标左键映射值，默认为 1
* @param right 鼠标右键映射值，默认为 2
* @param middle 鼠标中键映射值，默认为 3
*/
void CInputManager::MapMouseButton(int left, int right, int middle) {
	m_iMouseButtonMap[1] = left;
	m_iMouseButtonMap[2] = right;
	m_iMouseButtonMap[3] = middle;
}

/**
* 获取或设置鼠标输入控制比例大小
* @param set 设置鼠标灵敏度
* @param move 移动速度
* @param turn 转动速度
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
* 设置重力输入系数
* @param sx X分量系数
* @param sy Y分量系数
* @param sz Z分量系数
*/
void CInputManager::GravityScale(float sx, float sy, float sz) {
	m_fGravityScale[0] = sx;
	m_fGravityScale[1] = sy;
	m_fGravityScale[2] = sz;
}