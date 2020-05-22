//================================================
// Copyright (c) 2016 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CDeviceLinux.h"
#include "AvatarConfig.h"

#ifdef AVATAR_LINUX
#include "CEngine.h"
#include "CLog.h"
#include <X11/X.h>
#include <X11/Xlib.h>

// 方向键按键记录
static bool KeyPressed[5];

/**
* 构造函数
*/
CDeviceLinux::CDeviceLinux() {
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
CDeviceLinux::~CDeviceLinux() {
	m_pInstance = 0;
}

/**
* 单例实例
*/
CDeviceLinux* CDeviceLinux::m_pInstance = 0;

/**
* 创建窗口
*/
bool CDeviceLinux::Create(const string& title, int width, int height, bool resizable, bool fullscreen, bool antialias) {
	// 打开本地显示设备
	m_pContext->display = XOpenDisplay(NULL);
	if (m_pContext->display == NULL) {
		CLog::Error("Create display failed");
		return false;
	}
	// 选择像素模式
	int numFormats = 0;
	int attrib[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		GLX_SAMPLE_BUFFERS, 1,
		GLX_SAMPLES, 4,
		None
	};
	if (!antialias) attrib[22] = None;
	int screen = DefaultScreen(m_pContext->display);
	GLXFBConfig* configList = glXChooseFBConfig(m_pContext->display, screen, attrib, &numFormats);
	if (configList == NULL) {
		Destroy();
		CLog::Error("Pixel format not support");
		return false;
	}
	GLXFBConfig bestFit = configList[0];
	XFree(configList);

	// 获取桌面窗口
	XVisualInfo *xvi = glXGetVisualFromFBConfig(m_pContext->display, bestFit);
	Window root = RootWindow(m_pContext->display, xvi->screen);
	XSetWindowAttributes swa;
	swa.colormap = XCreateColormap(m_pContext->display, root, xvi->visual, AllocNone);
	swa.event_mask = StructureNotifyMask | ExposureMask;
	swa.event_mask |= FocusChangeMask;
	swa.event_mask |= PointerMotionMask;
	swa.event_mask |= ButtonPressMask | ButtonReleaseMask;
	swa.event_mask |= KeyPressMask | KeyReleaseMask;

	// 从已有的窗口创建
	if (title.length() > 1 && title.at(0) == '$') root = strtoul(title.substr(1).c_str(), 0, 0);
	m_pContext->window = XCreateWindow(m_pContext->display, root, 0, 0, width, height, 0,
		xvi->depth, InputOutput, xvi->visual, CWColormap | CWEventMask, &swa);
	XFree(xvi);
	// 显示窗口
	XStoreName(m_pContext->display, m_pContext->window, title.c_str());
	XMapWindow(m_pContext->display, m_pContext->window);

	// OpenGL context
	m_pContext->context = glXCreateNewContext(m_pContext->display, bestFit, GLX_RGBA_TYPE, 0, True);
	glXMakeCurrent(m_pContext->display, m_pContext->window, m_pContext->context);
	// 关闭垂直同步
	typedef void (*PFNGLXSWAPINTERVALMESAPROC)(int interval);
	PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)glXGetProcAddress((GLubyte*)"glXSwapIntervalMESA");
	if (glXSwapIntervalMESA) glXSwapIntervalMESA(0);

	// 全屏需要 VidMode 或者 RandR 扩展
	if (fullscreen) {
		CLog::Warn("AVATAR is going to run in windows mode");
	}
	// 初始化 GLEW 库
	glewInit();
	CLog::Info("OpenGL version %s", glGetString(GL_VERSION));
	if (!glewIsSupported("GL_VERSION_3_3")) {
		CLog::Error("System do not support OpenGL 3.3, Device may not work properly");
	}
	if (antialias) glEnable(GL_MULTISAMPLE);

	// 刷新窗体大小
	CEngine::GetGraphicsManager()->SetWindowSize(width, height);
	return true;
}

/**
* 实例销毁
*/
void CDeviceLinux::Destroy() {
	if (m_pContext->context) {
		glXMakeCurrent(m_pContext->display, None, NULL);
		glXDestroyContext(m_pContext->display, m_pContext->context);
		m_pContext->context = NULL;
	}
	if (m_pContext->window) {
		XDestroyWindow(m_pContext->display, m_pContext->window);
		m_pContext->window = 0;
	}
	if (m_pContext->display) {
		XCloseDisplay(m_pContext->display);
		m_pContext->display = NULL;
	}
	delete m_pContext;
	delete this;
}

/**
* 绘制窗口
*/
void CDeviceLinux::Render() {
	glXSwapBuffers(m_pContext->display, m_pContext->window);
}

/**
* 处理消息
*/
void CDeviceLinux::Handle(float dt) {
	int button, delta;
	KeySym ksym;
	CInputManager* pInputMgr = CEngine::GetInputManager();

	// 处理窗口消息
	while (XPending(m_pContext->display) > 0) {
		XEvent event;
		XNextEvent(m_pContext->display, &event);
		// AutoRepeat off
		if (event.type == KeyRelease && XEventsQueued(m_pContext->display, QueuedAfterReading)) {
			XEvent nexv;
			XPeekEvent(m_pContext->display, &nexv);
			if (nexv.type == KeyPress &&
				nexv.xkey.time == event.xkey.time &&
				nexv.xkey.keycode == event.xkey.keycode) {
				continue;
			}
		}
		switch (event.type) {
		case ConfigureNotify:
			pInputMgr->Resize(event.xconfigure.width, event.xconfigure.height);
			break;
		case MapNotify:
		case FocusIn:
			pInputMgr->Start();
			break;
		case UnmapNotify:
		case FocusOut:
			pInputMgr->Pause();
			break;
#ifdef AVATAR_ENABLE_MOUSE
		case MotionNotify:
			if (event.xbutton.state & Button1Mask) button = 1;
			else if (event.xbutton.state & Button3Mask) button = 2;
			else if (event.xbutton.state & Button2Mask) button = 3;
			else button = 0;
			pInputMgr->MouseInput(event.xbutton.x, event.xbutton.y, button, 0);
			break;
		case ButtonRelease:
			pInputMgr->MouseInput(event.xbutton.x, event.xbutton.y, 0, 0);
			break;
		case ButtonPress:
			button = 0;
			delta = 0;
			if (event.xbutton.button == Button1) button = 1;
			else if (event.xbutton.button == Button2) button = 3;
			else if (event.xbutton.button == Button3) button = 2;
			else if (event.xbutton.button == Button4) delta = 1;
			else if (event.xbutton.button == Button5) delta = -1;
			pInputMgr->MouseInput(event.xbutton.x, event.xbutton.y, button, delta);
			break;
#endif
#ifdef AVATAR_ENABLE_KEYBOARD
		case KeyPress:
			KeyPressed[0] = (event.xkey.state & ShiftMask) != 0;
			ksym = XLookupKeysym(&event.xkey, 0);
			if (ksym == XK_Up) KeyPressed[1] = true;
			else if (ksym == XK_Down) KeyPressed[2] = true;
			else if (ksym == XK_Right) KeyPressed[3] = true;
			else if (ksym == XK_Left) KeyPressed[4] = true;
			else if (ksym == XK_F1) pInputMgr->Function(1);
			else if (ksym == XK_F2) pInputMgr->Function(2);
			else if (ksym == XK_F3) pInputMgr->Function(3);
			else if (ksym == XK_F4) pInputMgr->Function(4);
			else if (ksym == XK_F5) pInputMgr->Function(5);
			else if (ksym == XK_F6) pInputMgr->Function(6);
			else if (ksym == XK_F7) pInputMgr->Function(7);
			else if (ksym == XK_F8) pInputMgr->Function(8);
			else if (ksym == XK_F9) pInputMgr->Function(9);
			else if (ksym == XK_F11) pInputMgr->Function(11);
			else if (ksym == XK_F12) pInputMgr->Function(12);
			break;
		case KeyRelease:
			KeyPressed[0] = (event.xkey.state & ShiftMask) != 0;
			ksym = XLookupKeysym(&event.xkey, 0);
			if (ksym == XK_Up) KeyPressed[1] = false;
			else if (ksym == XK_Down) KeyPressed[2] = false;
			else if (ksym == XK_Right) KeyPressed[3] = false;
			else if (ksym == XK_Left) KeyPressed[4] = false;
			else if (ksym == XK_Escape) pInputMgr->KeyboardInput(0x1B);
			else if (ksym == XK_BackSpace) pInputMgr->KeyboardInput(0x08);
			else if (ksym >= 32 && ksym <= 122) pInputMgr->KeyboardInput(static_cast<int>(ksym));
			break;
#endif
		default:
			break;
		}
	}
	// 处理方向按键
	float fMoveSpeed = 5.0f * dt;
	if (KeyPressed[0]) fMoveSpeed *= 4.0f;
	if (KeyPressed[1]) pInputMgr->ForthBack(fMoveSpeed);
	if (KeyPressed[2]) pInputMgr->ForthBack(-fMoveSpeed);
	if (KeyPressed[3]) pInputMgr->RightLeft(fMoveSpeed);
	if (KeyPressed[4]) pInputMgr->RightLeft(-fMoveSpeed);
}

#endif
