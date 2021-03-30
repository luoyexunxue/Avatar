LOCAL_PATH := $(call my-dir)

# 连接 OpenAL 预编译静态库
include $(CLEAR_VARS)
LOCAL_MODULE := libopenal
LOCAL_SRC_FILES := ../libopenal/$(TARGET_ARCH_ABI)/libopenal.so
include $(PREBUILT_SHARED_LIBRARY)

# 编译引擎并链接相应的第三方库
include $(CLEAR_VARS)
LOCAL_MODULE := engine
LOCAL_SRC_FILES := CAnimationManager.cpp CAsyncTask.cpp CBase64.cpp CBoundingBox.cpp CCamera.cpp \
	CCameraFree.cpp CCameraGaze.cpp CCameraGeographic.cpp CCameraSmooth.cpp CCameraViewer.cpp CColor.cpp \
	CContactDetector.cpp CDelaunay.cpp CDeviceAndroid.cpp CDynamicBvt.cpp CEngine.cpp CFFT.cpp \
	CFileManager.cpp CFontManager.cpp CFrustum.cpp CGeometryCreator.cpp CGraphicsManager.cpp \
	CGuiButton.cpp CGuiEditBox.cpp CGuiEnvironment.cpp CGuiLabel.cpp CGuiListView.cpp CGuiPanel.cpp \
	CGuiTrackBar.cpp CIKSolver.cpp CInputManager.cpp CJsonObject.cpp CLog.cpp CMaterial.cpp \
	CMatrix4.cpp CMesh.cpp CMeshBlend.cpp CMeshData.cpp CMeshLoader.cpp CMeshLoader3ds.cpp \
	CMeshLoaderBasic.cpp CMeshLoaderCollada.cpp CMeshLoaderGltf.cpp CMeshSlicer.cpp CPhysicsManager.cpp \
	CPlane.cpp CPluginLoader.cpp CPostProcess.cpp CPostProcessAnaglyph.cpp CPostProcessBeam.cpp \
	CPostProcessBloom.cpp CPostProcessBlur.cpp CPostProcessCartoon.cpp CPostProcessEdge.cpp \
	CPostProcessEmboss.cpp CPostProcessFxaa.cpp CPostProcessGray.cpp CPostProcessHdr.cpp \
	CPostProcessManager.cpp CPostProcessOculus.cpp CPostProcessPanorama.cpp CPostProcessPass.cpp \
	CPostProcessRain.cpp CPostProcessSnow.cpp CPostProcessSsao.cpp CPostProcessUser.cpp CProfile.cpp \
	CQuaternion.cpp CRay.cpp CRectangle.cpp CRigidBody.cpp CSceneManager.cpp CSceneNode.cpp \
	CSceneNodeAnimation.cpp CSceneNodeAxis.cpp CSceneNodeBlast.cpp CSceneNodeBoard.cpp CSceneNodeCloud.cpp \
	CSceneNodeDecal.cpp CSceneNodeFlame.cpp CSceneNodeFresnel.cpp CSceneNodeGeometry.cpp CSceneNodeLensflare.cpp \
	CSceneNodeLine.cpp CSceneNodeMesh.cpp CSceneNodeParticles.cpp CSceneNodePlanet.cpp CSceneNodePlant.cpp \
	CSceneNodeScreen.cpp CSceneNodeSkybox.cpp CSceneNodeSound.cpp CSceneNodeStatic.cpp CSceneNodeTerrain.cpp \
	CSceneNodeText.cpp CSceneNodeWater.cpp CScriptContext.cpp CScriptManager.cpp CShader.cpp \
	CShaderManager.cpp CSoundManager.cpp CStringUtil.cpp CTexture.cpp CTextureManager.cpp \
	CThreadPool.cpp CTimer.cpp CUdpSocket.cpp CUrlConnection.cpp CVector2.cpp \
	CVector3.cpp CVertex.cpp CVertexJoint.cpp CZipReader.cpp Avatar.cpp

LOCAL_SHARED_LIBRARIES += libopenal
LOCAL_STATIC_LIBRARIES := libjpeg
LOCAL_STATIC_LIBRARIES += libpng
LOCAL_STATIC_LIBRARIES += freetype
LOCAL_STATIC_LIBRARIES += liblua
LOCAL_STATIC_LIBRARIES += libcurl
LOCAL_STATIC_LIBRARIES += videoplay
include $(BUILD_STATIC_LIBRARY)