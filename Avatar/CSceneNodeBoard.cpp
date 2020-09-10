//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CSceneNodeBoard.h"
#include "CGeometryCreator.h"
#include "AvatarConfig.h"
#include "CEngine.h"
#include "CStringUtil.h"
#include "CTimer.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#ifdef AVATAR_ENABLE_VIDEOCAP
#include "extension/VideoCap.h"
#ifdef AVATAR_WINDOWS
#pragma comment(lib, "extension/VideoCap.lib")
#endif
#endif
#ifdef AVATAR_ENABLE_VIDEOPLAY
#include "extension/VideoPlay.h"
#ifdef AVATAR_WINDOWS
#pragma comment(lib, "extension/VideoPlay.lib")
#endif
#endif

/**
* 构造函数
* @note billboard 0不跟随镜头 1定轴 2定视角 3无限制
*/
CSceneNodeBoard::CSceneNodeBoard(const string& name, const string& texture, float width, float height, int billboard)
	: CSceneNode("board", name) {
	m_pMeshData = 0;
	m_pTexture = 0;
	m_iCameraId = -1;
	m_pVideoContext = 0;
	m_fPlayTime = 0.0f;
	m_iSoundId = 0;
	m_iSoundBufferSize = 0;
	m_pFileBuffer = 0;
	m_strTexture = texture;
	m_fWidth = width;
	m_fHeight = height;
	m_iBillboardType = billboard;
	m_cAxis.SetValue(CVector3::Z);
}

/**
* 初始化场景节点
*/
bool CSceneNodeBoard::Init() {
	m_pTexture = CreateTexture(m_strTexture);
	if (!m_pTexture) return false;
	m_pTexture->SetWrapModeClampToEdge(true, true);
	m_pMeshData = new CMeshData();
	m_pMeshData->AddMesh(CGeometryCreator::CreatePlane(m_fWidth, m_fHeight, -2));
	m_pMeshData->GetMesh(0)->GetMaterial()->SetTexture(m_pTexture);
	m_cLocalBoundingBox.m_cMin.SetValue(-0.5f * m_fWidth, -0.001f, -0.5f * m_fHeight);
	m_cLocalBoundingBox.m_cMax.SetValue(0.5f * m_fWidth, 0.001f, 0.5f * m_fHeight);
	return true;
}

/**
* 销毁场景节点
*/
void CSceneNodeBoard::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pTexture);
#ifdef AVATAR_ENABLE_VIDEOCAP
	if (m_iCameraId >= 0) VideoCapClose(m_iCameraId);
#endif
#ifdef AVATAR_ENABLE_VIDEOPLAY
	if (m_pVideoContext) {
		VideoPlayClose(m_pVideoContext);
		CEngine::GetSoundManager()->Drop(m_iSoundId);
	}
#endif
	if (m_pMeshData) delete m_pMeshData;
	if (m_pFileBuffer) delete[] m_pFileBuffer;
}

/**
* 渲染场景节点
*/
void CSceneNodeBoard::Render() {
	if (m_iBillboardType > 0 && !m_lstChildren.empty()) return;
	if (CEngine::GetGraphicsManager()->IsDepthRender()) {
		m_pTexture->UseTexture();
		m_pMeshData->GetMesh(0)->Render(false);
	} else {
		m_pMeshData->GetMesh(0)->Render();
	}
}

/**
* 更新场景节点
*/
void CSceneNodeBoard::Update(float dt) {
	if (m_iBillboardType > 0) {
		CCamera* pCamera = CEngine::GetGraphicsManager()->GetCamera();
		CMatrix4 viewMat = pCamera->GetViewMatrix();
		CVector3 direction = viewMat.Transpose() * CVector3::Z;
		CVector3 original = CVector3(0.0f, -1.0f, 0.0f);
		if (m_iBillboardType == 1) {
			m_cOrientation.FromVector(original.CrossProduct(m_cAxis), direction.CrossProduct(m_cAxis));
		} else if (m_iBillboardType == 2) {
			float translate[16] = {
				viewMat[0], viewMat[1], viewMat[2], 0.0f,
				-viewMat[8], -viewMat[9], -viewMat[10], 0.0f,
				viewMat[4], viewMat[5], viewMat[6], 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			m_cOrientation.FromMatrix(CMatrix4(translate));
		} else m_cOrientation.FromVector(original, direction);
		Transform();
	}
#ifdef AVATAR_ENABLE_VIDEOCAP
	if (m_iCameraId >= 0) {
		VideoCapFrame(m_iCameraId, (unsigned char*)m_pTexture->MapBuffer(false));
		m_pTexture->UnmapBuffer();
	}
#endif
#ifdef AVATAR_ENABLE_VIDEOPLAY
	if (m_pVideoContext) {
		m_fPlayTime += dt;
		VideoPlayTime(m_pVideoContext, m_fPlayTime);
		if (VideoPlayVideo(m_pVideoContext, (unsigned char*)m_pTexture->MapBuffer(false)) > 0) {
			m_fPlayTime = 0.0f;
			VideoPlayTime(m_pVideoContext, -1.0f);
		}
		m_pTexture->UnmapBuffer();
		if (m_iSoundId > 0) {
			if (m_iSoundBufferSize == 0) m_iSoundBufferSize += VideoPlayAudio(m_pVideoContext, m_pSoundBuffer, 8192);
			if (m_iSoundBufferSize > 0) {
				if (CEngine::GetSoundManager()->Update(m_iSoundId, m_pSoundBuffer, m_iSoundBufferSize)) {
					m_iSoundBufferSize = 0;
				}
			}
		}
	}
#endif
}

/**
* 更新变换矩阵
*/
void CSceneNodeBoard::Transform() {
	if (m_pParent) {
		// 仅保留父节点的平移项
		CMatrix4 worldParent = m_pParent->m_cWorldMatrix;
		worldParent[12] = 0.0f;
		worldParent[13] = 0.0f;
		worldParent[14] = 0.0f;
		m_cModelMatrix.MakeTransform(m_cScale, m_cOrientation, m_cPosition);
		m_cModelMatrix = worldParent.Invert() * m_cModelMatrix;
		m_cWorldMatrix = m_pParent->m_cWorldMatrix * m_cModelMatrix;
		list<CSceneNode*>::iterator iter = m_lstChildren.begin();
		while (iter != m_lstChildren.end()) {
			(*iter)->Transform();
			++iter;
		}
	} else CSceneNode::Transform();
}

/**
* 获取网格数据
*/
CMeshData* CSceneNodeBoard::GetMeshData() {
	if (m_iBillboardType > 0 && !m_lstChildren.empty()) return 0;
	return m_pMeshData;
}

/**
* 获取媒体信息
*/
void CSceneNodeBoard::MediaInfo(int* width, int* height, float* length) {
	*width = m_pTexture->GetWidth();
	*height = m_pTexture->GetHeight();
#ifdef AVATAR_ENABLE_VIDEOPLAY
	if (m_pVideoContext) {
		VideoPlayInfo(m_pVideoContext, 0, 0, length, 0, 0, 0);
	}
#endif
}

/**
* 设置旋转轴，仅 billboardType 为1时有效
*/
void CSceneNodeBoard::SetAxis(const CVector3& axis) {
	m_cAxis.SetValue(axis);
}

/**
* 设置播放时间
*/
void CSceneNodeBoard::SetPlayTime(float time) {
#ifdef AVATAR_ENABLE_VIDEOPLAY
	if (m_pVideoContext) {
		VideoPlaySeek(m_pVideoContext, time);
	}
#endif
}

/**
* 创建纹理对象
*/
CTexture* CSceneNodeBoard::CreateTexture(const string& texture) {
	string ext = CStringUtil::UpperCase((CFileManager::GetExtension(texture)));
	if (ext == "BMP" || ext == "JPG" || ext == "JPEG" || ext == "PNG" || ext == "TGA") {
		return CEngine::GetTextureManager()->Create(texture);
	}
	if (texture.find("camera:") == 0) {
#ifdef AVATAR_ENABLE_VIDEOCAP
		int frameWidth;
		int frameHeight;
		m_iCameraId = atoi(texture.substr(texture.find(':') + 1).c_str());
		if (VideoCapOpen(m_iCameraId, &frameWidth, &frameHeight) != 0) return 0;
		return CEngine::GetTextureManager()->Create(texture, frameWidth, frameHeight, 3, 0, false);
#endif
	}
#ifdef AVATAR_ENABLE_VIDEOPLAY
	if (texture.find("://") != string::npos || texture.find("screen:") == 0) {
		if (VideoPlayOpen(texture.c_str(), 0, &m_pVideoContext) != 0) return 0;
	} else {
		string videoFile = texture;
		CFileManager::CBinaryFile buffer;
		if (!CFileManager::IsFullPath(texture)) {
			videoFile = CEngine::GetFileManager()->GetDataDirectory() + texture;
			if (!CEngine::GetFileManager()->FileExists(texture) && CEngine::GetFileManager()->ReadFile(texture, &buffer)) {
				m_pFileBuffer = new unsigned char[buffer.size];
				std::memcpy(m_pFileBuffer, buffer.contents, buffer.size);
			}
		}
		const char* pathData = buffer.size > 0 ? (char*)m_pFileBuffer : videoFile.c_str();
		if (VideoPlayOpen(pathData, buffer.size, &m_pVideoContext) != 0) return 0;
	}
	int frameWidth, frameHeight;
	int channel = 0, sampleRate = 0, sampleBits = 0;
	VideoPlayInfo(m_pVideoContext, &frameWidth, &frameHeight, 0, &channel, &sampleRate, &sampleBits);
	if (channel > 0) m_iSoundId = CEngine::GetSoundManager()->Create(channel, sampleRate, sampleBits);
	return CEngine::GetTextureManager()->Create(texture, frameWidth, frameHeight, 3, 0, false);
#else
	return CEngine::GetTextureManager()->Create("");
#endif
}