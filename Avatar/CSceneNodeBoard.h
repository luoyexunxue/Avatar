//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#ifndef _CSCENENODEBOARD_H_
#define _CSCENENODEBOARD_H_
#include "CSceneNode.h"

/**
* @brief 贴图平面场景节点
* @attention 如果该节点为广告牌类型且有子节点，则仅渲染子节点
*/
class AVATAR_EXPORT CSceneNodeBoard: public CSceneNode {
public:
	//! 构造方法
	CSceneNodeBoard(const string& name, const string& texture, float width, float height, int billboard);

	//! 初始化场景节点
	virtual bool Init();
	//! 销毁场景节点
	virtual void Destroy();
	//! 渲染场景节点
	virtual void Render();
	//! 更新场景节点
	virtual void Update(float dt);
	//! 更新变换矩阵
	virtual void Transform();
	//! 获取网格数据
	virtual CMeshData* GetMeshData();

	//! 媒体信息
	void MediaInfo(int* width, int* height, float* length);
	//! 设置旋转轴
	void SetAxis(const CVector3& axis);
	//! 设置播放时间
	void SetPlayTime(float time);

private:
	//! 打开图像纹理或视频纹理
	CTexture* CreateTexture(const string& texture);

private:
	//! 网格纹理
	string m_strTexture;
	//! 平面宽度
	float m_fWidth;
	//! 平面高度
	float m_fHeight;
	//! 广告牌类型
	int m_iBillboardType;
	//! 旋转轴
	CVector3 m_cAxis;
	//! 摄像机ID
	int m_iCameraId;
	//! 视频上下文
	void* m_pVideoContext;
	//! 视频播放时间
	float m_fPlayTime;
	//! 音频流ID
	int m_iSoundId;
	//! 音频缓冲大小
	int m_iSoundBufferSize;
	//! 音频缓冲区
	unsigned char m_pSoundBuffer[8192];
	//! 内存文件
	unsigned char* m_pFileBuffer;
	//! 网格对象
	CMeshData* m_pMeshData;
	//! 纹理对象
	CTexture* m_pTexture;
};

#endif