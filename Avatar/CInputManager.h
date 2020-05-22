//================================================
// Copyright (c) 2016 ���ʷ�. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CINPUTMANAGER_H_
#define _CINPUTMANAGER_H_
#include "AvatarConfig.h"

/**
* @brief ���������
*/
class AVATAR_EXPORT CInputManager {
public:
	//! ��ȡ������ʵ��
	static CInputManager* GetInstance() {
		if (m_pInstance) return m_pInstance;
		m_pInstance = new CInputManager();
		return m_pInstance;
	}
	//! ����
	void Destroy();

public:
	//! ���붨��
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

		int iState;
		int	iFunction;
		int iInputKey;
		int iInputX;
		int iInputY;
		int iWidth;
		int iHeight;

		float fRightLeft;
		float fForthBack;
		float fUpDown;
		float fYaw;
		float fPitch;
		float fRoll;
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
			iState = 0;
			iFunction = 0;
			iInputKey = 0;
			iInputX = 0;
			iInputY = 0;
			iWidth = 0;
			iHeight = 0;
			fRightLeft = 0.0f;
			fForthBack = 0.0f;
			fUpDown = 0.0f;
			fYaw = 0.0f;
			fPitch = 0.0f;
			fRoll = 0.0f;
			fPosition[0] = 0.0f;
			fPosition[1] = 0.0f;
			fPosition[2] = 0.0f;
			fOrientation[0] = 0.0f;
			fOrientation[1] = 0.0f;
			fOrientation[2] = 0.0f;
			fOrientation[3] = 0.0f;
			fGravity[0] = 0.0f;
			fGravity[1] = 0.0f;
			fGravity[2] = 0.0f;
		}
	};

public:
	//! ��ȡ����
	struct SInput* GetInput();
	//! �������
	void Update();

	//! �����ƶ�
	void RightLeft(float step);
	//! ǰ���ƶ�
	void ForthBack(float step);
	//! �����ƶ�
	void UpDown(float step);

	//! ˮƽ�Ƕ�
	void Yaw(float angle);
	//! �����Ƕ�
	void Pitch(float angle);
	//! �����Ƕ�
	void Roll(float angle);

	//! λ��
	void Position(float x, float y, float z);
	//! ��λ
	void Orientation(float x, float y, float z, float w);

	//! ����
	void Fire();
	//! ��Ծ
	void Jump();
	//! ��ʼ
	void Start();
	//! ��ͣ
	void Pause();
	//! �˳�
	void Quit();

	//! ��һ��״̬
	void NextState();
	//! ��һ��״̬
	void PrevState();
	//! ��λ״̬
	void ResetState();
	//! ����ѡ��
	void Function(int func);
	//! ���ڴ�С�ı�
	void Resize(int width, int height);

	//! �������
	void MouseInput(int x, int y, int button, int delta);
	//! ��������
	void KeyboardInput(int key);
	//! ��������
	void GravityInput(float x, float y, float z);

	//! ���ÿ�������
	void DisableInput(bool mouse, bool keyboard, bool gravity);
	//! ��갴��ӳ��
	void MapMouseButton(int left, int right, int middle);
	//! �������ٶ�
	void MouseSensitivity(bool set, float& move, float& turn);
	//! ��������ϵ��
	void GravityScale(float sx, float sy, float sz);

private:
	CInputManager();
	~CInputManager();

private:
	//! ��ǰ����
	SInput* m_pInput;
	//! ���뻺��
	SInput* m_pBufferedInput;

	//! �����������
	bool m_bDisableMouse;
	//! ���ü�������
	bool m_bDisableKeyboard;
	//! ������������
	bool m_bDisableGravity;

	//! ����ϴεİ���
	int m_iMouseLastButton;
	//! ����ϴε�λ��
	int m_iMouseLastPos[2];
	//! ��갴�µ�λ��
	int m_iMouseClickPos[2];
	//! ��갴��ӳ��
	int m_iMouseButtonMap[4];
	//! �������ٶ�
	float m_fMouseInputScale[2];
	//! ��������ϵ��
	float m_fGravityScale[3];
	//! ���������ʵ��
	static CInputManager* m_pInstance;
 };

#endif