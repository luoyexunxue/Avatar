//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDeviceWindows.h"
#include "AvatarConfig.h"

#ifdef AVATAR_WINDOWS
#include "CEngine.h"
#include "CLog.h"
#include "thirdparty/glew/glew.h"
#include "thirdparty/glew/wglew.h"
#include <windowsx.h>
#include <cstdlib>
#include <imm.h>

#pragma comment(lib, "imm32")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "thirdparty/glew/glew32.lib")

// 方向键按键记录
static bool KeyPressed[5];
// Windows 消息回调函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/**
* 构造函数
*/
CDeviceWindows::CDeviceWindows() {
	m_pContext = new SContext;
	KeyPressed[0] = false;
	KeyPressed[1] = false;
	KeyPressed[2] = false;
	KeyPressed[3] = false;
	KeyPressed[4] = false;
}

/**
* 析构函数
*/
CDeviceWindows::~CDeviceWindows() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CDeviceWindows* CDeviceWindows::m_pInstance = 0;

/**
* 创建窗口
*/
bool CDeviceWindows::Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) {
	// 从已有的窗口句柄创建
	if (title.length() > 1 && title.at(0) == '$') {
		RECT windowRect;
		m_pContext->hwnd = (HWND)strtoul(title.substr(1).c_str(), 0, 0);
		m_pContext->created = false;
		GetWindowRect(m_pContext->hwnd, &windowRect);
		width = windowRect.right - windowRect.left;
		height = windowRect.bottom - windowRect.top;
	}
	if (!m_pContext->hwnd) {
		// 注册窗口
		m_pContext->instance = GetModuleHandle(NULL);
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_pContext->instance;
		wc.hIcon = LoadIcon(m_pContext->instance, MAKEINTRESOURCE(100));
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = TEXT("Avatar");
		if (!RegisterClassEx(&wc)) {
			CLog::Error("Register window failed");
			return false;
		}
		// 窗口样式(默认居中显示)
		RECT windowRect;
		windowRect.left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		windowRect.top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		windowRect.right = windowRect.left + width;
		windowRect.bottom = windowRect.top + height;
		DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		if (resizable) dwStyle |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);

		// 创建窗口
		if (!(m_pContext->hwnd = CreateWindowExA(
			dwExStyle, "Avatar", title.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			NULL, NULL, m_pContext->instance, NULL))) {
			Destroy();
			CLog::Error("Create window failed");
			return false;
		}
	}
	// 获取 HDC
	if (!(m_pContext->hdc = GetDC(m_pContext->hwnd))) {
		Destroy();
		CLog::Error("Get HDC failed");
		return false;
	}
	// 像素格式
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// 创建临时上下文
	int pixelFormat = ChoosePixelFormat(m_pContext->hdc, &pfd);
	SetPixelFormat(m_pContext->hdc, pixelFormat, &pfd);
	HGLRC hrc = wglCreateContext(m_pContext->hdc);
	wglMakeCurrent(m_pContext->hdc, hrc);
	// 获取扩展入口
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	// 获取支持 MSAA 像素格式
	if (antialias && wglChoosePixelFormatARB) {
		UINT numFormats;
		int newPixelFormat = 0;
		int attributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB, 4, 0
		};
		if (wglChoosePixelFormatARB(m_pContext->hdc, attributes, NULL, 1, &newPixelFormat, &numFormats) && numFormats > 0) {
			pixelFormat = newPixelFormat;
		}
	}
	SetPixelFormat(m_pContext->hdc, pixelFormat, &pfd);
	// 设置渲染上下文
	m_pContext->hrc = wglCreateContext(m_pContext->hdc);
	wglMakeCurrent(m_pContext->hdc, m_pContext->hrc);
	wglDeleteContext(hrc);
	// 关闭垂直同步
	if (wglSwapIntervalEXT) wglSwapIntervalEXT(0);

	// 初始化 GLEW 库
	glewInit();
	CLog::Info("OpenGL version %s", glGetString(GL_VERSION));
	if (!glewIsSupported("GL_VERSION_3_3")) {
		CLog::Error("System do not support OpenGL 3.3, Device may not work properly");
	}
	if (antialias) glEnable(GL_MULTISAMPLE);

	// 检查系统是否支持该分辨率下的全屏模式
	if (fullscreen) {
		int i = 0;
		DEVMODE mode;
		mode.dmSize = sizeof(DEVMODE);
		mode.dmDriverExtra = 0;
		fullscreen = false;
		while (EnumDisplaySettings(NULL, i++, &mode)) {
			if (width == static_cast<int>(mode.dmPelsWidth) && height == static_cast<int>(mode.dmPelsHeight)) {
				fullscreen = true;
				break;
			}
		}
		// 设置全屏显示
		if (fullscreen) {
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (DWORD)width;
			dmScreenSettings.dmPelsHeight = (DWORD)height;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
				m_pContext->changed = true;
				SetWindowLong(m_pContext->hwnd, GWL_STYLE, WS_POPUP);
				SetWindowLong(m_pContext->hwnd, GWL_EXSTYLE, 0);
				SetWindowPos(m_pContext->hwnd, NULL, 0, 0, width, height, 0);
			}
		} else {
			CLog::Warn("AVATAR is going to run in windows mode");
		}
	}
	// 显示窗口
	ShowWindow(m_pContext->hwnd, SW_SHOW);
	SetForegroundWindow(m_pContext->hwnd);
	SetFocus(m_pContext->hwnd);
	// 刷新窗体大小
	CEngine::GetGraphicsManager()->SetWindowSize(width, height);
	return true;
}

/**
* 实例销毁
*/
void CDeviceWindows::Destroy() {
	// 恢复显示设置
	if (m_pContext->changed) {
		ChangeDisplaySettings(NULL, 0);
	}
	if (m_pContext->hrc) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_pContext->hrc);
		m_pContext->hrc = NULL;
	}
	if (m_pContext->hdc) {
		ReleaseDC(m_pContext->hwnd, m_pContext->hdc);
		m_pContext->hdc = NULL;
	}
	if (m_pContext->created) {
		if (m_pContext->hwnd) {
			DestroyWindow(m_pContext->hwnd);
		}
		UnregisterClass(TEXT("Avatar"), m_pContext->instance);
	}
	delete m_pContext;
	delete this;
}

/**
* 绘制窗口
*/
void CDeviceWindows::Render() {
	SwapBuffers(m_pContext->hdc);
}

/**
* 处理消息
*/
void CDeviceWindows::Handle(float dt) {
	if (m_pContext->created) {
		MSG	message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		CInputManager* pInputMgr = CEngine::GetInputManager();
		// 处理方向按键
		float fMoveSpeed = 5.0f * dt;
		if (KeyPressed[0]) fMoveSpeed *= 4.0f;
		if (KeyPressed[1]) pInputMgr->ForthBack(fMoveSpeed);
		if (KeyPressed[2]) pInputMgr->ForthBack(-fMoveSpeed);
		if (KeyPressed[3]) pInputMgr->RightLeft(fMoveSpeed);
		if (KeyPressed[4]) pInputMgr->RightLeft(-fMoveSpeed);
	}
}

/**
* Windows 窗口消息回调函数
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int button, delta;
	POINT point;
	CInputManager* pInputMgr = CEngine::GetInputManager();

	// 处理窗口消息
	switch (uMsg) {
	case WM_SHOWWINDOW:
		if (HIWORD(wParam)) pInputMgr->Pause();
		else pInputMgr->Start();
		return 0;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			pInputMgr->Resize(LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	case WM_CLOSE:
	case WM_QUIT:
		pInputMgr->Quit();
		return 0;
	case WM_ERASEBKGND:
		return 0;
#ifdef AVATAR_ENABLE_KEYBOARD
	case WM_CHAR:
		pInputMgr->KeyboardInput((int)wParam);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_SHIFT) KeyPressed[0] = true;
		else if (wParam == VK_UP) KeyPressed[1] = true;
		else if (wParam == VK_DOWN) KeyPressed[2] = true;
		else if (wParam == VK_RIGHT) KeyPressed[3] = true;
		else if (wParam == VK_LEFT) KeyPressed[4] = true;
		else if (wParam == VK_F1) pInputMgr->Function(1);
		else if (wParam == VK_F2) pInputMgr->Function(2);
		else if (wParam == VK_F3) pInputMgr->Function(3);
		else if (wParam == VK_F4) pInputMgr->Function(4);
		else if (wParam == VK_F5) pInputMgr->Function(5);
		else if (wParam == VK_F6) pInputMgr->Function(6);
		else if (wParam == VK_F7) pInputMgr->Function(7);
		else if (wParam == VK_F8) pInputMgr->Function(8);
		else if (wParam == VK_F9) pInputMgr->Function(9);
		else if (wParam == VK_F10) pInputMgr->Function(10);
		else if (wParam == VK_F11) pInputMgr->Function(11);
		else if (wParam == VK_F12) pInputMgr->Function(12);
		return 0;
	case WM_KEYUP:
		if (wParam == VK_SHIFT) KeyPressed[0] = false;
		else if (wParam == VK_UP) KeyPressed[1] = false;
		else if (wParam == VK_DOWN) KeyPressed[2] = false;
		else if (wParam == VK_RIGHT) KeyPressed[3] = false;
		else if (wParam == VK_LEFT) KeyPressed[4] = false;
		return 0;
#endif
#ifdef AVATAR_ENABLE_MOUSE
	case WM_MOUSEWHEEL:
		delta = (short)HIWORD(wParam) / 120;
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &point);
		pInputMgr->MouseInput(point.x, point.y, 0, delta);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		point.x = GET_X_LPARAM(lParam);
		point.y = GET_Y_LPARAM(lParam);
		if (MK_LBUTTON == wParam) {
			button = 1;
			// IME 位置
			if (HIMC himc = ImmGetContext(hWnd)) {
				COMPOSITIONFORM cf;
				cf.ptCurrentPos.x = point.x;
				cf.ptCurrentPos.y = point.y;
				cf.dwStyle = CFS_FORCE_POSITION;
				ImmSetCompositionWindow(himc, &cf);
			}
		}
		else if (MK_RBUTTON == wParam) button = 2;
		else if (MK_MBUTTON == wParam) button = 3;
		else button = 0;
		pInputMgr->MouseInput(point.x, point.y, button, 0);
		return 0;
#endif
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#endif
