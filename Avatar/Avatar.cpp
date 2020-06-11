//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "AvatarConfig.h"

/**
* VLD 在 windows 下可用，只在 Debug 版本下使用
*/
#if (defined AVATAR_ENABLE_VLD) && (defined AVATAR_WINDOWS) && (defined _DEBUG)
#include <vld.h>
#endif

/**
* 网络控制
*/
#ifdef AVATAR_CONTROL_NETWORK
#include "CUdpSocket.h"
#include "CJsonParser.h"
#endif

#include "CEngine.h"
#include "CTimer.h"
#include "CLog.h"
#include "CPluginLoader.h"
#include <cstring>
#include <sstream>
using std::ostringstream;

/**
* 输出调试信息
*/
void DumpInfo();

/**
* 控制线程
*/
#ifdef AVATAR_WINDOWS
#include <Windows.h>
DWORD WINAPI ThreadRecv(LPVOID pParam);
#ifndef _WINDLL
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#else
#include <unistd.h>
#include <pthread.h>
void* ThreadRecv(void* pParam);
#endif

/**
* Windows、Linux 平台
*/
#if (defined AVATAR_WINDOWS) || (defined AVATAR_LINUX)
int main(int argc, char **argv) {
	// 引擎初始化参数
	SEngineConfig conf;
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (!strcmp(argv[i], "-d") && i + 1 < argc) conf.directory = argv[++i];
			else if (!strcmp(argv[i], "-w") && i + 1 < argc) conf.graphic.width = atoi(argv[++i]);
			else if (!strcmp(argv[i], "-h") && i + 1 < argc) conf.graphic.height = atoi(argv[++i]);
			else if (!strcmp(argv[i], "-f")) conf.graphic.fullscreen = true;
			else if (!strcmp(argv[i], "-a")) conf.graphic.antialias = true;
			else if (!strcmp(argv[i], "-l")) {
				conf.log.enable = true;
				if (i + 1 < argc && argv[i + 1][0] != '-') {
					const char* log = argv[++i];
					for (int n = strlen(log) - 1; n >= 0; n--) {
						if (log[n] == 'c') conf.log.console = true;
						else if (log[n] == 'f') conf.log.file = true;
						else if (log[n] == 't') conf.log.time = true;
					}
				} else {
					conf.log.console = true;
					conf.log.time = true;
				}
			}
		}
	}
	// 调试模式打开日志
#ifdef _DEBUG
	conf.log.enable = true;
	conf.log.console = true;
	conf.log.time = true;
#endif
	// 游戏初始化
	CEngine* engine = new CEngine();
	if (!engine->Init(conf)) {
		return 1;
	}

	// 创建控制线程
#ifdef AVATAR_CONTROL_NETWORK
#ifdef AVATAR_WINDOWS
	static DWORD threadId;
	CreateThread(NULL, 0, ThreadRecv, engine, 0, &threadId);
#else
	static pthread_t threadId;
	pthread_create(&threadId, 0, ThreadRecv, engine);
#endif
#endif

	while (engine->IsRunning()) {
		engine->Update();
		engine->Render();
	}
	// 销毁游戏实例
	DumpInfo();
	engine->Destroy();
	delete engine;
	return 0;
}
#endif

/**
* Android 平台
*/
#ifdef AVATAR_ANDROID
#include <jni.h>

/**
* JNI 接口
*/
extern "C" {
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_init(JNIEnv* env, jobject obj, jstring directory, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_resize(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_update(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_move(JNIEnv* env, jobject obj, jfloat rightLeft, jfloat forthBack, jfloat upDown);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_orient(JNIEnv* env, jobject obj, jfloat qx, jfloat qy, jfloat qz, jfloat qw);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_gravity(JNIEnv* env, jobject obj, jfloat gx, jfloat gy, jfloat gz);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_mouse(JNIEnv* env, jobject obj, jint x, jint y, jint button, jint delta);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_function(JNIEnv* env, jobject obj, jint function);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_script(JNIEnv* env, jobject obj, jstring script);
	JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_exit(JNIEnv* env, jobject obj);
};

/**
* Engine 实例指针
*/
static CEngine* engine;
/**
* 标记是否初始化
*/
static bool _avatar_init = false;

/**
* 初始化
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_init(JNIEnv* env, jobject obj, jstring directory, jint width, jint height) {
	if (engine) {
		engine->Destroy();
		delete engine;
	}
	engine = new CEngine();

	// 游戏初始化
	char path[256] = {0};
	env->GetStringUTFRegion(directory, 0, env->GetStringLength(directory), path);
	SEngineConfig conf;
	conf.directory = path;
	conf.graphic.width = width;
	conf.graphic.height = height;
	conf.log.enable = true;
	conf.log.console = true;
	conf.log.time = true;
	if (!engine->Init(conf)) {
		return;
	}
	// 创建控制线程
#if (defined AVATAR_CONTROL_NETWORK)
	static pthread_t threadId;
	pthread_create(&threadId, 0, ThreadRecv, engine);
#endif
	_avatar_init = true;
}

/**
* 窗体大小
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_resize(JNIEnv* env, jobject obj, jint width, jint height) {
	if (_avatar_init) {
		CEngine::GetInputManager()->Resize(width, height);
	}
}

/**
* 更新和渲染
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_update(JNIEnv* env, jobject obj) {
	if (_avatar_init && engine->IsRunning()) {
		engine->Update();
		engine->Render();
	}
}

/**
* 移动
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_move(JNIEnv* env, jobject obj, jfloat rightLeft, jfloat forthBack, jfloat upDown) {
	if (_avatar_init) {
		CInputManager* pInputMgr = CEngine::GetInputManager();
		pInputMgr->RightLeft(rightLeft);
		pInputMgr->ForthBack(forthBack);
		pInputMgr->UpDown(upDown);
	}
}

/**
* 摄像机方位
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_orient(JNIEnv* env, jobject obj, jfloat qx, jfloat qy, jfloat qz, jfloat qw) {
	if (_avatar_init) {
		CEngine::GetInputManager()->Orientation(qx, qy, qz, qw);
	}
}

/**
* 重力感应输入
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_gravity(JNIEnv* env, jobject obj, jfloat gx, jfloat gy, jfloat gz) {
	if (_avatar_init) {
		CMatrix4 view = CEngine::GetGraphicsManager()->GetCamera()->GetViewMatrix();
		CVector3 g = view.Transpose() * CVector3(gx, gy, gz, 0.0f);
		CEngine::GetInputManager()->GravityInput(g[0], g[1], g[2]);
	}
}

/**
* 鼠标输入
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_mouse(JNIEnv* env, jobject obj, jint x, jint y, jint button, jint delta) {
	if (_avatar_init) {
		CEngine::GetInputManager()->MouseInput(x, y, button, delta);
	}
}

/**
* 功能选择
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_function(JNIEnv* env, jobject obj, jint function) {
	if (_avatar_init) {
		CEngine::GetInputManager()->Function(function);
	}
}

/**
* 执行脚本
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_script(JNIEnv* env, jobject obj, jstring script) {
	if (_avatar_init) {
		char command[1024] = {0};
		env->GetStringUTFRegion(script, 0, env->GetStringLength(script), command);
		CEngine::GetScriptManager()->Script(command);
	}
}

/**
* 退出
*/
JNIEXPORT void JNICALL Java_com_luoyexunxue_avatar_AvatarNative_exit(JNIEnv* env, jobject obj) {
	if (engine) {
		DumpInfo();
		engine->Destroy();
		delete engine;
		engine = 0;
	}
	_avatar_init = false;
}
#endif

/**
* 输出调试信息
*/
void DumpInfo() {
	CFileManager* pFileMgr = CEngine::GetFileManager();
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CSceneManager* pSceneMgr = CEngine::GetSceneManager();
	CPhysicsManager* pPhysicsMgr = CEngine::GetPhysicsManager();
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	CSoundManager* pSoundMgr = CEngine::GetSoundManager();
	CFontManager* pFontMgr = CEngine::GetFontManager();
	CPostProcessManager* pPostMgr = CEngine::GetPostProcessManager();
	ostringstream out;
	out << std::boolalpha;
	out << "====================================>> DUMP" << std::endl;
	out << "Directory: " << pFileMgr->GetDataDirectory() << std::endl;
	int width, height;
	pGraphicsMgr->GetWindowSize(&width, &height);
	out << "Graphics: " << width << "*" << height;
	out << " shadow=" << pGraphicsMgr->GetShadowEnable();
	out << " fog=" << pGraphicsMgr->GetFogEnable();
	out << " stereo=" << pGraphicsMgr->GetStereoMode() << std::endl;
	CCamera* camera = pGraphicsMgr->GetCamera();
	out << "Camera: type=" << camera->GetName() << " fov=" << camera->GetFieldOfView();
	out << " near=" << camera->GetNearClipDistance() << " far=" << camera->GetFarClipDistance() << std::endl;
	out << "Total mesh: " << pSceneMgr->GetMeshCount() << std::endl;
	out << "Total vertex: " << pSceneMgr->GetVertexCount() << std::endl;
	out << "Total triangle: " << pSceneMgr->GetTriangleCount() << std::endl;
	vector<CSceneNode*> node;
	pSceneMgr->GetNodeList(node);
	out << "SceneNode List:" << std::endl;
	for (size_t i = 0; i < node.size(); i++) {
		out << " - type=" << node[i]->GetType();
		out << " name='" << node[i]->GetName() << "'";
		out << " visible=" << node[i]->m_bVisible;
		out << " enabled=" << node[i]->m_bEnabled << std::endl;
	}
	vector<CRigidBody*> rigid;
	pPhysicsMgr->GetRigidBodyList(rigid);
	out << "RigidBody List:" << std::endl;
	for (size_t i = 0; i < rigid.size(); i++) {
		out << " - type=" << rigid[i]->GetGeometry().ToString();
		out << " name='" << rigid[i]->GetSceneNode()->GetName() << "'" << std::endl;
	}
	vector<CShader*> shader;
	pShaderMgr->GetShaderList(shader);
	out << "Shader List:" << std::endl;
	for (size_t i = 0; i < shader.size(); i++) {
		out << " - valid=" << shader[i]->IsValid();
		out << " name='" << shader[i]->GetName() << "'" << std::endl;
	}
	vector<CTexture*> texture;
	pTextureMgr->GetTextureList(texture);
	out << "Texture List:" << std::endl;
	for (size_t i = 0; i < texture.size(); i++) {
		out << " - valid=" << texture[i]->IsValid();
		out << " size=" << texture[i]->GetWidth() << "*" << texture[i]->GetHeight() << "*" << texture[i]->GetChannel();
		out << " path='" << texture[i]->GetFilePath() << "'" << std::endl;
	}
	vector<string> sound;
	pSoundMgr->GetSoundList(sound);
	out << "Sound List:" << std::endl;
	for (size_t i = 0; i < sound.size(); i++) out << " - " << sound[i] << std::endl;
	vector<string> font;
	pFontMgr->GetFontList(font);
	out << "Font List:" << std::endl;
	for (size_t i = 0; i < font.size(); i++) out << " - " << font[i] << std::endl;
	vector<string> post;
	pPostMgr->GetPostProcessList(post);
	out << "Post List:" << std::endl;
	for (size_t i = 0; i < post.size(); i++) out << " - " << post[i] << std::endl;
	vector<string> plugin;
	CPluginLoader::GetPluginList(plugin);
	out << "Plugin List:" << std::endl;
	for (size_t i = 0; i < plugin.size(); i++) out << " - " << plugin[i] << std::endl;
	out << "====================================<< END";
	CLog::Message(out.str());
}

/**
* '高级控制' 接收线程
*/
#ifdef AVATAR_WINDOWS
DWORD WINAPI ThreadRecv(LPVOID pParam) {
#else
void* ThreadRecv(void* pParam) {
#endif
	CLog::Debug("Created control thread");
	CInputManager* pInputMgr = CEngine::GetInputManager();

#ifdef AVATAR_CONTROL_NETWORK
	char buffer[1024];
	CUdpSocket sock;
	sock.Open(1314);
	sock.EnableBlockmode(false);
#endif

	while (((CEngine*)pParam)->IsRunning()) {
#ifdef AVATAR_CONTROL_NETWORK
		int ret = sock.RecvData(buffer, 1024);
		if (ret > 0) {
			buffer[ret] = 0;
			CJsonParser parser(buffer);
			if (parser.IsContain("dx")) pInputMgr->RightLeft(parser["dx"].ToFloat());
			if (parser.IsContain("dy")) pInputMgr->ForthBack(parser["dy"].ToFloat());
			if (parser.IsContain("dz")) pInputMgr->UpDown(parser["dz"].ToFloat());
			if (parser.IsContain("yaw")) pInputMgr->Yaw(parser["yaw"].ToFloat());
			if (parser.IsContain("pitch")) pInputMgr->Pitch(parser["pitch"].ToFloat());
			if (parser.IsContain("roll")) pInputMgr->Roll(parser["roll"].ToFloat());
			if (parser.IsContain("fire") && parser["fire"].ToBool()) pInputMgr->Fire();
			if (parser.IsContain("jump") && parser["jump"].ToBool()) pInputMgr->Jump();
			if (parser.IsContain("angle")) {
				CQuaternion orient;
				CJsonParser& ori = parser["angle"];
				orient.FromEulerAngles(ori[0].ToFloat(), ori[1].ToFloat(), ori[2].ToFloat());
				pInputMgr->Orientation(orient[0], orient[1], orient[2], orient[3]);
			}
			if (parser.IsContain("command")) {
				string strValue = parser["command"].ToString();
				CLog::Debug("Execute script %s", strValue.c_str());
				CEngine::GetScriptManager()->Script(strValue.c_str());
			}
		}
#endif
		CTimer::Sleep(1);
	}

#ifdef AVATAR_CONTROL_NETWORK
	sock.Close();
#endif
	return 0;
}