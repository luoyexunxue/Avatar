![avatar](media/logo_small.png)

#### 介绍
跨平台三维引擎
Avatar 是一款跨平台高性能的3D引擎，底层采用 C++ 开发，目前支持 Windows、Linux、Android 三个平台。渲染部分基于 OpenGL3.3/ES3.0，引擎具有很高的可移植性，除了 freetype、glew、libcurl、libjpeg、libpng、lua、minimp3、openal、rapidxml、zlib 这些开源的第三方库以外无其他依赖库。引擎的特点是简单高效的同时提供非常丰富的功能。

#### 示例
```c++
#include <CEngine.h>
int main(int argc, char **argv)
{
	// 初始化引擎参数
	SEngineConfig conf;
	// 资源目录
	conf.directory = "C:\\Avatar\\media";
	// 初始化
	CEngine* engine = new CEngine();
	if (!engine->Init(conf))
	{
		return 1;
	}
	// 主循环
	while (engine->IsRunning())
	{
		engine->Update();
		engine->Render();
	}
	// 销毁实例
	engine->Destroy();
	delete engine;
	return 0;
}
```