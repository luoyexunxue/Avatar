//================================================
// Copyright (c) 2020 周仁锋. All rights reserved.
// ye_luo@qq.com
//================================================
#include "CPostProcessSsao.h"
#include "CEngine.h"
#include <cstdlib>

/**
* 初始化后处理对象
*/
bool CPostProcessSsao::Init(int width, int height) {
	// 生成深度图
	const char* depthShaderVs = "\
		uniform mat4 uProjMatrix;\
		uniform mat4 uViewMatrix;\
		uniform mat4 uModelMatrix;\
		uniform mat4 uViewProjMatrix;\
		in vec4 aPosition;\
		in vec2 aTexCoord;\
		in vec4 aNormal;\
		in vec4 aColor;\
		out vec2 vTexCoord;\
		void main()\
		{\
			vTexCoord = aTexCoord;\
			gl_Position = uViewProjMatrix * uModelMatrix * aPosition;\
		}";
	const char* depthShaderPs = "\
		uniform sampler2D uAlphaMap;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			if (texture(uAlphaMap, vTexCoord).a < 0.5) discard;\
			const float zFar = 2000.0;\
			float depth = (gl_FragCoord.z / gl_FragCoord.w) / zFar;\
			fragColor = vec4(depth);\
		}";
	// 生成 SSAO 光照图
	const char* ssaoShader = "\
		uniform sampler2D uDepthTexture;\
		uniform sampler2D uRandomTexture;\
		uniform float uRandTextureTiles;\
		uniform float uSamplesFactor;\
		uniform vec3 uSampleSphere[16];\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		vec3 normalFromDepth(float depth, vec2 texCoord)\
		{\
			const vec2 offset1 = vec2(0.0, 0.01);\
			const vec2 offset2 = vec2(0.01, 0.0);\
			float depth1 = texture(uDepthTexture, texCoord + offset1).r;\
			float depth2 = texture(uDepthTexture, texCoord + offset2).r;\
			vec3 p1 = vec3(offset1, depth1 - depth);\
			vec3 p2 = vec3(offset2, depth2 - depth);\
			vec3 normal = cross(p1, p2);\
			normal.z = -normal.z;\
			return normalize(normal);\
		}\
		void main()\
		{\
			const float base = 0.2;\
			const float area = 0.0075;\
			const float fallOff = 0.000001;\
			const float radius = 0.0005;\
			vec3 random = texture(uRandomTexture, vTexCoord * uRandTextureTiles).rgb;\
			float depth = texture(uDepthTexture, vTexCoord).r;\
			vec3 position = vec3(vTexCoord, depth);\
			vec3 normal = normalFromDepth(depth, vTexCoord);\
			float radiusDepth = radius / depth;\
			float occlusion = 0.0;\
			for (int i = 0; i < 16; i++)\
			{\
				vec3 ray = radiusDepth * reflect(uSampleSphere[i], random);\
				vec3 hemiRay = position + dot(ray, normal) * ray;\
				float occlusionDepth = texture(uDepthTexture, clamp(hemiRay.xy, 0.0, 1.0)).r;\
				float difference = depth - occlusionDepth;\
				occlusion += step(fallOff, difference) * (1.0 - smoothstep(fallOff, area, difference));\
			}\
			float result = clamp(1.0 - occlusion * uSamplesFactor + base, 0.0, 1.0);\
			fragColor = vec4(result, result, result, 1.0);\
		}";
	// 模糊处理
	const char* blurShader = "\
		uniform sampler2D uTexture;\
		uniform vec2 uScreenSize;\
		uniform vec2 uDirection;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			float weights[7];\
			weights[0] = 0.05;\
			weights[1] = 0.1;\
			weights[2] = 0.2;\
			weights[3] = 0.3;\
			weights[4] = 0.2;\
			weights[5] = 0.1;\
			weights[6] = 0.05;\
			vec2 texelStep = vec2(1.0 / uScreenSize.x, 1.0 / uScreenSize.y) * uDirection;\
			vec2 start = vTexCoord - 3.0 * texelStep;\
			vec4 baseColor = vec4(0.0, 0.0, 0.0, 0.0);\
			vec2 texelOffset = vec2(0.0, 0.0);\
			for (int i = 0; i < 7; i++)\
			{\
				baseColor += texture(uTexture, start + texelOffset) * weights[i];\
				texelOffset += texelStep;\
			}\
			fragColor = vec4(baseColor.rgb, 1.0);\
		}";
	// 使用 SSAO 光照图生成最终场景
	const char* combineShader = "\
		uniform sampler2D uTexture;\
		uniform sampler2D uTextureAO;\
		in vec2 vTexCoord;\
		out vec4 fragColor;\
		void main()\
		{\
			vec4 ssaoColor = texture(uTextureAO, vTexCoord);\
			vec4 sceneColor = texture(uTexture, vTexCoord);\
			fragColor = ssaoColor * sceneColor;\
		}";
	// 创建着色器和纹理
	CShaderManager* pShaderMgr = CEngine::GetShaderManager();
	CTextureManager* pTextureMgr = CEngine::GetTextureManager();
	m_pPostProcessShader = pShaderMgr->Create("postprocess_ssao", GetVertexShader(), combineShader);
	m_pPostProcessShader->SetUniform("uTexture", 0);
	m_pPostProcessShader->SetUniform("uTextureAO", 1);
	m_pDepthShader = pShaderMgr->Create("postprocess_ssao_depth", depthShaderVs, depthShaderPs);
	m_pDepthShader->SetUniform("uAlphaMap", 0);
	m_pSsaoShader = pShaderMgr->Create("postprocess_ssao_ao", GetVertexShader(), ssaoShader);
	m_pSsaoShader->SetUniform("uDepthTexture", 0);
	m_pSsaoShader->SetUniform("uRandomTexture", 1);
	m_pSsaoShader->SetUniform("uRandTextureTiles", 4.0f);
	m_pSsaoShader->SetUniform("uSamplesFactor", 1.0f / 16.0f);
	float sampleSphere[] = {
		 0.5381f,  0.1856f, -0.4319f,
		 0.1379f,  0.2486f,  0.4430f,
		 0.3371f,  0.5679f, -0.0057f,
		-0.6999f, -0.0451f, -0.0019f,
		 0.0689f, -0.1598f, -0.8547f,
		 0.0560f,  0.0069f, -0.1843f,
		-0.0146f,  0.1402f,  0.0762f,
		 0.0100f, -0.1924f, -0.0344f,
		-0.3577f, -0.5301f, -0.4358f,
		-0.3169f,  0.1063f,  0.0158f,
		 0.0103f, -0.5869f,  0.0046f,
		-0.0897f, -0.4940f,  0.3287f,
		 0.7119f, -0.0154f, -0.0918f,
		-0.0533f,  0.0596f, -0.5411f,
		 0.0352f, -0.0631f,  0.5460f,
		-0.4776f,  0.2847f, -0.0271f
	};
	m_pSsaoShader->SetUniform("uSampleSphere", sampleSphere, 3, 16);
	m_pBlurShader = pShaderMgr->Create("postprocess_ssao_blur", GetVertexShader(), blurShader);
	m_pBlurShader->SetUniform("uTexture", 0);
	m_pBlurShader->SetUniform("uScreenSize", CVector2(width, height));
	m_pRenderTexture = pTextureMgr->Create("postprocess_ssao", width, height, false, true, false);
	m_pSsaoTexture = pTextureMgr->Create("postprocess_ssao_ao", width, height, false, false, false);
	m_pBlurTexture = pTextureMgr->Create("postprocess_ssao_blur", width, height, false, false, false);
	m_pDepthTexture = pTextureMgr->Create("postprocess_ssao_depth", width, height, true, true, false);
	unsigned char noise[256 * 256 * 3];
	for (int i = 0; i < 196608; i++) noise[i] = (unsigned char)(rand() & 0xFF);
	m_pRandomTexture = pTextureMgr->Create("", 256, 256, 3, noise, false);
	return m_pPostProcessShader->IsValid() && m_pDepthShader->IsValid()
		&& m_pSsaoShader->IsValid() && m_pBlurShader->IsValid();
}

/**
* 渲染区域大小改变
*/
void CPostProcessSsao::Resize(int width, int height) {
	CEngine::GetTextureManager()->Resize(m_pRenderTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pSsaoTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pBlurTexture, width, height);
	CEngine::GetTextureManager()->Resize(m_pDepthTexture, width, height);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uScreenSize", CVector2(width, height));
}

/**
* 销毁后处理对象
*/
void CPostProcessSsao::Destroy() {
	CEngine::GetTextureManager()->Drop(m_pRenderTexture);
	CEngine::GetTextureManager()->Drop(m_pSsaoTexture);
	CEngine::GetTextureManager()->Drop(m_pBlurTexture);
	CEngine::GetTextureManager()->Drop(m_pDepthTexture);
	CEngine::GetTextureManager()->Drop(m_pRandomTexture);
	CEngine::GetShaderManager()->Drop(m_pPostProcessShader);
	CEngine::GetShaderManager()->Drop(m_pDepthShader);
	CEngine::GetShaderManager()->Drop(m_pSsaoShader);
	CEngine::GetShaderManager()->Drop(m_pBlurShader);
}

/**
* 应用当前后处理
*/
void CPostProcessSsao::Apply(CTexture* target, CMesh* mesh) {
	// 渲染深度图像
	CGraphicsManager* pGraphicsMgr = CEngine::GetGraphicsManager();
	CCamera* pCamera = pGraphicsMgr->GetCamera();
	CMatrix4 viewProj = pCamera->GetProjMatrix() * pCamera->GetViewMatrix();
	pGraphicsMgr->SetRenderTarget(m_pDepthTexture, 0, true, true, true);
	pGraphicsMgr->SetColorBlendEnable(false);
	CEngine::GetSceneManager()->RenderDepth(viewProj, "postprocess_ssao_depth");
	pGraphicsMgr->SetColorBlendEnable(true);
	// 渲染 AO 图
	pGraphicsMgr->SetRenderTarget(m_pSsaoTexture, 0, true, false, false);
	m_pSsaoShader->UseShader();
	m_pRandomTexture->UseTexture(1);
	m_pDepthTexture->UseTexture(0);
	mesh->Render(false);
	// AO 水平模糊
	pGraphicsMgr->SetRenderTarget(m_pBlurTexture, 0, true, false, false);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uDirection", CVector2(2.5f, 0.0f));
	m_pSsaoTexture->UseTexture();
	mesh->Render(false);
	// AO 垂直模糊
	pGraphicsMgr->SetRenderTarget(m_pSsaoTexture, 0, true, false, false);
	m_pBlurShader->UseShader();
	m_pBlurShader->SetUniform("uDirection", CVector2(0.0f, 2.5f));
	m_pBlurTexture->UseTexture();
	mesh->Render(false);
	// 渲染最终场景
	pGraphicsMgr->SetRenderTarget(target, 0, true, false, false);
	m_pPostProcessShader->UseShader();
	m_pSsaoTexture->UseTexture(1);
	m_pRenderTexture->UseTexture(0);
	mesh->Render(false);
}