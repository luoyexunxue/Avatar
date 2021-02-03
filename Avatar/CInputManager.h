//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CINPUTMANAGER_H_
#define _CINPUTMANAGER_H_
#include "AvatarConfig.h"

/**
* @brief 输入管理器
*/
class AVATAR_EXPORT CInputManager {
public:
	//! 获取管理器实例
	static CInputManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CInputManager();
		return m_pInstance;
	}
	//! 销毁
	void Destroy();

public:
	//! 输入定义
	struct SInput {
		bool bPause;
		bool bQuit;
		bool bFire;
		bool bJump;
		bool bMove;
		bool bTurn;
		bool bPosition;
		bool bOrientation;
		bool bGravity;

		int	iFunction;
		int iInputKey;
		int iInputX;
		int iInputY;
		int iWidth;
		int iHeight;

		float fMove[3];
		float fTurn[3];
		float fPosition[3];
		float fOrientation[4];
		float fGravity[3];

		SInput() {
			bPause = false;
			bQuit = false;
			bFire = false;
			bJump = false;
			bMove = false;
			bTurn = false;
			bPosition = false;
			bOrientation = false;
			bGravity = false;
			iFunction = 0;
			iInputKey = 0;
			iInputX = 0;
			iInputY = 0;
			iWidth = 0;
			iHeight = 0;
			fMove[0] = 0.0f;
			fMove[1] = 0.0f;
			fMove[2] = 0.0f;
			fTurn[0] = 0.0f;
			fTurn[1] = 0.0f;
			fTurn[2] = 0.0f;
			fPosition[0] = 0.0f;
			fPosition[1] = 0.0f;
			fPosition[2] = 0.0f;
			fOrientation[0] = 0.0f;
			fOrientation[1] = 0.0f;
			fOrientation[2] = 0.0f;
			fOrientation[3] = 1.0f;
			fGravity[0] = 0.0f;
			fGravity[1] = 0.0f;
			fGravity[2] = 0.0f;
		}
	};

public:
	//! 获取输入
	struct SInput* GetInput();
	//! 输入更新
	void Update();

	//! 移动
	void Move(float right, float forth, float up);
	//! 角度
	void Turn(float yaw, float pitch, float roll);
	//! 位置
	void Position(float x, float y, float z);
	//! 方位
	void Orientation(float x, float y, float z, float w);

	//! 开火
	void Fire();
	//! 跳跃
	void Jump();
	//! 开始
	void Start();
	//! 暂停
	void Pause();
	//! 退出
	void Quit();

	//! 功能选择
	void Function(int func);
	//! 窗口大小改变
	void Resize(int width, int height);

	//! 鼠标输入
	void MouseInput(int x, int y, int button, int delta);
	//! 键盘输入
	void KeyboardInput(int key);
	//! 重力输入
	void GravityInput(float x, float y, float z);

	//! 禁用控制输入
	void DisableInput(bool mouse, bool keyboard, bool gravity);
	//! 鼠标按键映射
	void MapMouseButton(int left, int right, int middle);
	//! 鼠标控制速度
	void MouseSensitivity(bool set, float& move, float& turn);
	//! 重力输入系数
	void GravityScale(float sx, float sy, float sz);

private:
	CInputManager();
	~CInputManager();

private:
	//! 当前输入
	SInput* m_pInput;
	//! 输入缓冲
	SInput* m_pBufferedInput;

	//! 禁用鼠标输入
	bool m_bDisableMouse;
	//! 禁用键盘输入
	bool m_bDisableKeyboard;
	//! 禁用重力输入
	bool m_bDisableGravity;

	//! 鼠标上次的按键
	int m_iMouseLastButton;
	//! 鼠标上次的位置
	int m_iMouseLastPos[2];
	//! 鼠标按下的位置
	int m_iMouseClickPos[2];
	//! 鼠标按键映射
	int m_iMouseButtonMap[4];
	//! 鼠标控制速度
	float m_fMouseInputScale[2];
	//! 重力输入系数
	float m_fGravityScale[3];
	//! 输入管理器实例
	static CInputManager* m_pInstance;
 };

#endif