/**
@mainpage Avatar - 跨平台高性能3D引擎

##简介##
<hr>
这是我的一个业余项目，Avatar 是一款跨平台高性能的3D引擎，底层采用 C++ 开发，目前支持 Windows、Linux、
Android 三个平台。渲染部分基于 OpenGL3.3/ES3.0，引擎具有很高的可移植性，除了 freetype、glew、libcurl、
libjpeg、libpng、lua、minimp3、openal、rapidxml、zlib 这些开源的第三方库以外无其他依赖库。引擎的特点是
简单高效的同时提供非常丰富的功能，关于引擎名称 Avatar (译：女神)藏有一段小故事，它是我大学新学到的第一
个单词(^_^)。部分特性见下方介绍。关于作者和引擎详见 http://blog.csdn.net/luoyexunxue。对于引擎的使用，
可参考 @ref page_example

引擎的部分特性：
<br/>
-# <b>支持多种纹理格式 BMP TGA PNG JPG，同时支持程序纹理</b>
-# <b>支持载入动画模型 MS3D DAE GLTF 和静态模型 OBJ STL 3DS 以及骨骼动画 BVH 并可配置自定义加载器</b>
-# <b>基于 OpenAL 的 3D 音效，支持 WAV 和 MP3 格式音频载入</b>
-# <b>集成 FreeType 字体库，支持自定义字体显示</b>
-# <b>集成 Lua 脚本引擎，支持使用 Lua 与引擎的交互，基于脚本的 GUI 系统，具有非常灵活的事件处理能力</b>
-# <b>能够实现诸如实时监控，摄像头视频采集，摄像头控制等功能</b>
-# <b>实现了一个小型快速的物理引擎，能够进行刚体模拟</b>
-# <b>内建多种后处理，如立体视觉透视矫正，HDR，FXAA，红蓝3D，VolumeLight 以及用户自定义后处理</b>
-# <b>支持场景节点动画，包括缩放，旋转，平移，包含多种动画方式</b>
-# <b>包含多种相机，可完成诸如第一人称游戏、第三人称游戏、模型浏览、飞行模拟、VR模式等多种控制</b>
-# <b>完整的日志系统，可指定输出级别，可输出到文件和控制台</b>
-# <b>支持读取多种文件来源，本地文件系统，压缩文件，HTTP/FTP 网络资源</b>
-# <b>包含 Delaunay 三角网生成、FFT 变换/逆变换、JSON 解析、模型分层切片、UDP 网络通信等基础库</b>

##历程##
<hr>
Avatar 是我从2013年3月份辞掉第一份工作后开始的，最初的目只是为了制作 3D 游戏，后经过断断续续的更新，已经相对成熟
<br/>
2013-03 开始开发 Windows 版本<br/>
2014-04 移植到 Linux 系统上，需要感谢 MaYuan 的帮助<br/>
2014-05 移植到 Raspberry Pi 上<br/>
2014-08 移植到 Android 上面<br/>
2014-08 使用引擎开发了 Android 平台游戏 3D 拼图<br/>
2014-09 引擎加入物理系统<br/>
2015-03 图形引擎切换至 OpenGL/ES 2.0 着色器模式<br/>
2015-04 增加后处理管理器<br/>
2015-05 实现了大气层散射渲染<br/>
2015-05 加入 Lua 脚本引擎<br/>
2015-08 实现了基于屏幕镜头光晕效果渲染<br/>
2015-08 添加基于脚本的 GUI 系统<br/>
2015-10 添加 ShadowMap 支持和雾效果支持<br/>
2015-11 物理引擎完善，增加了碰撞检测和反应部分<br/>
2016-02 添加动画引擎，支持缩放旋转平移动画<br/>
2016-04 修改了场景节点，完善了场景管理，丰富了脚本接口<br/>
2016-09 解决了物理引擎稳定性问题<br/>
2016-10 增加了 ZIP 文件系统，可直接从 ZIP 中读取文件<br/>
2017-03 增加了 MeshLoader 接口，并增加了 3DS 支持<br/>
2017-03 增加了程序纹理 Procedural Texture<br/>
2017-07 增加了 Collada(DAE) 模型的支持<br/>
2017-12 图形渲染 API 升级到 OpenGL3.3/ES3.0，去掉了 Raspberry Pi 的支持<br/>
2018-01 去掉了对 libiconv 库依赖，增加 UTF-8 与 wchar_t 相互转换的支持<br/>
2018-02 增加了骨骼动画的简单物理模拟，基于此可实现乳摇等效果<br/>
2018-04 材质系统改为基于物理的 BRDF 方式<br/>
2018-10 新增了 Geographic 相机，可用于地理信息系统类应用<br/>
2019-02 增加了全景图渲染功能<br/>
2019-03 添加 GLTF 模型文件载入功能<br/>
2019-05 新增插件功能<br/>
2019-09 PBR 增加 IBL 环境光照特性<br/>

##架构##
<hr>
一、目录结构<br/>
@code
Avatar		-- 工程的主目录
|-	extension	-- 扩展库包含目录
|-	media		-- 程序运行需要的资源
|-	thirdparty 	-- 第三方库
|	|-	freetype	-- freetype 字体渲染库
|	|-	glew		-- OpenGL 扩展库
|	|-	libcurl		-- URL 网络请求库
|	|-	libjpeg		-- jpg 文件读写库
|	|-	libpng		-- png 文件读写库
|	|-	lua			-- lua 脚本解析库
|	|-	minimp3		-- mp3 文件解码库
|	|-	openal		-- OpenAL 库
|	|-	rapidxml	-- XML 解析库
|	|-	zlib		-- 文件压缩库
extension	-- 扩展库目录
|-	VideoPlay	-- 使用 FFmpeg 的视频播放扩展（已提供DLL编译版本，编译需要 FFmpeg 支持）
|-	VideoCap	-- 使用 OpenCV 的摄像头视频扩展（已提供DLL编译版本，编译需要 OpenCV 2.4 支持）
@endcode
二、主要组成<br/>
CEngine 引擎核心，各管理器的获取等<br/>
CDevice 设备抽象层，实现了 CDeviceWindows，CDeviceLinux，CDeviceAndroid<br/>
CGraphicsManager 管理图形操作，具有渲染，相机设置，光源设置，阴影设置，雾效设置，截屏，拾取等方法<br/>
CFileManager 管理文件操作，包括目录操作，BIN，TXT，BMP，TGA，PNG，JPG，WAV，MP3 文件的读取与写入<br/>
CInputManager 管理引擎输入操作，支持左右，前后，上下，方位角，俯仰角，翻滚角，开火，跳跃，开始，暂停，退出，状态设定，功能选择等输入操作<br/>
CSoundManager 管理声音资源，实现基于 OpenAL，包含 3D 音源的创建，播放，暂停，停止，和属性设置，以及听众位置、速度、方向设置<br/>
CSceneManager 管理场景节点树，提供通过 ID，名称，类型，射线拾取场景中的场景节点，并包含节点插入，删除，自动排序等操作<br/>
CFontManager 管理字体资源，使用 FreeType 库加载字体库以及渲染字体，支持 FreeType、TrueType 字体载入，生成纹理图片支持对齐方式<br/>
CTextureManager 管理纹理资源，可优化纹理资源的使用，可创建普通二维纹理，立方体纹理，内存纹理，渲染纹理，支持灰度，RGB，RGBA 格式<br/>
CShaderManager 管理着色器程序资源，为着色器资源提供统一的管理器，优化使用<br/>
CPhysicsManager 管理物理模拟，模拟刚体运动，碰撞检测<br/>
CScriptManager 管理脚本接口，引擎以 Lua 作为脚本语言，提供丰富的交互接口<br/>
CPostProcessManager 管理后处理程序资源，对渲染结果进行后处理操作<br/>
CAnimationManager 管理动画模拟，为场景节点提供补间动画，包含缩放、旋转、平移动画<br/>

##编译##
<hr>
需要单独编译第三方库文件，Windows 平台已提供预编译版本<br/>
| 名称 | 描述 | 引用 |
| :------- | :--- | :------ |
| glew | OpenGL 扩展 | CGraphicsManager CShaderManager CTextureManager CMesh |
| openal | 三维音效库 | CSoundManager |
| zlib | 压缩与解压缩库 | CZipReader |
| libpng | PNG 文件读写库 | CFileManager |
| libjpeg | JPG 文件读写库 | CFileManager |
| freetype | 字体读取库 | CFontManager |
| lua | 脚本解析库 | CScriptManager CScriptContext |
| libcurl | URL 网络请求库 | CUrlConnection |
编译前可以修改 AvatarConfig.h 文件进行配置<br/>
AVATAR_ENABLE_VLD 使用 Visual Leak Detector 做内存泄漏检测<br/>
AVATAR_ENABLE_VIDEOCAP 使用摄像机捕捉库（目前只能在 Windows 下使用）<br/>
AVATAR_ENABLE_VIDEOPLAY 使用视频播放库<br/>
AVATAR_ENABLE_LOGO 在屏幕右上角显示引擎 LOGO<br/>
AVATAR_ENABLE_MOUSE 启用鼠标输入<br/>
AVATAR_ENABLE_KEYBOARD 启用键盘输入<br/>
AVATAR_CONTROL_NETWORK 启用网络游戏手柄控制（UDP协议，JSON数据）<br/>
*/

/**
@page page_faq 常见问题

##基本概念##
<hr>
- 空间坐标系为右手坐标系，X 轴为屏幕向右，Y 轴为屏幕向里，Z 轴为屏幕向上
- 屏幕坐标系原点为左下角，X 正方向为向右，Y 轴正方向为向上
- 矩阵定义为列优先排列方式，与 OpenGL 兼容
- 正交矩阵的逆矩阵等于转置矩阵，未缩放的旋转矩阵就是正交矩阵
- 纹理通道0至7用于自定义用途，通道8为阴影贴图，通道9/10/11为IBL通道

##编码技巧##
<hr>
- 自定义后处理和程序纹理均使用 uniform float uElapsedTime 获取当前时间
- 自定义后处理和程序纹理均使用 uniform vec2 uResolution 获取屏幕分辨率
- 自定义后处理和程序纹理均使用 in vec2 vTexCoord 作为纹理坐标输入
- 自定义后处理使用 uniform sampler2D uSamples[] 作为纹理数据输入
- GUI 脚本文件需使用 UTF-8 编码，以防止 GUI 中文显示乱码

##着色器代码##
<hr>
-# 线性化深度
@code
uniform sampler2D depthSampler;
const float zNear = 0.1;
const float zFar = 2000.0;
float LinearizeDepth(vec2 uv) {
	float z = texture2D(depthSampler, uv).x;
	return (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));
}
@endcode
-# 草坪渲染
@code
in vec2 vTexCoord;
out vec4 fragColor;
float rand(vec2 n) {
	return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}
float fbm(vec2 n) {
	float total = 0.0, amplitude = 1.0;
	for (int i = 0; i < 4; i++) {
		total += noise(n) * amplitude;
		n += n;
		amplitude *= 0.5;
	}
	return total;
}
void main() {
	float ratioy = mod(gl_FragCoord.y * 100.0 , fbm(vTexCoord * 2.0));
	vec3 color = vec3(0.6, 0.9, 0.5) * ratioy;
	fragColor = vec4(color, 1.0);
}
@endcode
-# Gamma矫正
@code
color = color / (color + vec3(1.0));
color = pow(color, vec3(1.0 / 2.2));
@endcode
*/

/**
@page page_example 示例代码

##简单示例##
<hr>
@code
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
@endcode
这个非常简单的例子展示了引擎的基本使用，这个例子会创建一个窗体，并加载 C:\\Avatar\\media 目录下的 avatar.lua 脚本及相关系统资源。
*/

/**
@page page_interface 脚本接口

##回调一览表##
<hr>
| 接口名称 | 描述 | 参数 | 备注 |
| :------- | :--- | :--- | :--- |
| OnReady | 初始化完成 | - | 初始化完成时调用一次 |
| OnUpdate | 逻辑更新 | dt:*float* | - |
| OnExit | 引擎退出 | - | 引擎结束运行时调用一次 |
| OnSize | 窗口大小改变 | width:*integer*, height:*integer* | - |
| OnInput | 输入事件 | name:*string*, value:*integer*, arg1:*number*, arg2:*number*, arg3:*number* | name 为 'function/key/fire/move/turn' |

##接口一览表##
<hr>
| 接口名称 | 描述 | 参数 | 返回值 |
| :------- | :--- | :--- | :----- |
| engine.info | 获取引擎信息 | - | {version:*string*, platform:*string*} |
| engine.fps | 获取当前帧率 | - | fps:*float* |
| engine.log | 输出日志 | [msg:*any*] | - |
| engine.speed | 设置引擎时间快慢 | speed:*float*, &lt;fixed:*boolean*&gt; | - |
| engine.timer | 获取定时器定时 | name:*string*, &lt;reset:*boolean*&gt; | elapse:*float* |
| engine.directory | 获取或设置数据目录 | &lt;dir:*string*&gt; | &lt;dir:*string*&gt; |
| engine.input | 输入消息 | method:*string*, [params:*boolean* / *integer* / *float*] | - |
| engine.script | 执行 LUA 脚本 | file:*string* | success:*boolean* |
| engine.execute | 执行回调方法 | function:*string*, &lt;argument:*string*&gt; | result:*integer* |
| engine.read | 读取指定文件数据 | file:*string* | data:*string* |
| engine.write | 写入数据至指定文件 | file:*string*, data:*string* | - |
| engine.plugin | 加载引擎插件 | path:*string* | desc:*string* |
| gui.enable | 使能或禁止 GUI | enable:*boolean* | - |
| gui.size | 获取 GUI 画布大小 | - | width:*integer*, height:*integer* |
| gui.scale | 设置 GUI 缩放 | scale:*float* | - |
| gui.create | 创建 GUI 元素 | name:*string*, type:*string*, desc:*string*, &lt;event:*function(evt:integer, arg1:integer, arg2:integer)*&gt; | - |
| gui.modify | 修改 GUI 元素 | name:*string*, desc:*string* | - |
| gui.delete | 删除 GUI 元素 | name:*string* | - |
| gui.attrib | 获取 GUI 元素属性 | name:*string*, attrib:*string* | value:*string* |
| camera.type | 获取或设置相机类型 | &lt;type:*string*, {param}&gt; | &lt;type:*string*&gt; |
| camera.control | 绑定用户控制输入 | enable:*boolean* | - |
| camera.fov | 获取或设置相机视角 | &lt;fov:*float*&gt; | &lt;fov:*float*&gt; |
| camera.clip | 获取或设置相机裁剪面 | &lt;near:*float*, far:*float*&gt; | &lt;near:*float*, far:*float*&gt; |
| camera.projection | 获取或设置投影方式 | &lt;ortho:*boolean*, width:*float*, height:*float*&gt; | &lt;ortho:*boolean*&gt; |
| camera.position | 获取或设置相机位置 | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.target | 获取或设置相机目标点 | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.angle | 获取或设置相机角度 | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.direction | 获取相机方向正交向量组 | - | lx:*float*, ly:*float*, lz:*float*, rx:*float*, ry:*float*, rz:*float*, ux:*float*, uy:*float*, uz:*float* |
| camera.bind | 绑定场景节点或解除绑定 | &lt;name:*string*, x:*float*, y:*float*, z:*float*, ax:*float*, ay:*float*, az:*float*&gt; | - |
| scene.insert | 插入场景节点 | parent:*string*, type:*string*, name:*string*, {attrib} | success:*boolean* |
| scene.delete | 删除场景节点 | name:*string* | - |
| scene.clear | 清除所有场景节点 | - | - |
| scene.list | 获取场景节点列表 | - | {name:*string*} |
| scene.pick | 射线选择场景节点 | x:*float*, y:*float*, z:*float*, dx:*float*, dy:*float*, dz:*float* | name:*string* / *nil*, &lt;x:*float*, y:*float*, z:*float*, mesh:*integer*, face:*integer*&gt; |
| scene.scale | 获取或设置场景节点缩放 | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.angle | 获取或设置场景节点角度 | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.orientation | 获取或设置场景节点方位 | name:*string*, &lt;x:*float*, y:*float*, z:*float*, w:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*, w:*float*&gt; |
| scene.position | 获取或设置场景节点位置 | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.visible | 获取或设置场景节点可见性 | name:*string*, &lt;visible:*boolean*&gt; | &lt;visible:*boolean*&gt; |
| scene.update | 更新场景节点网格 | name:*string*, mesh:*integer*, {data} | - |
| scene.vertex | 获取场景节点网格顶点信息 | name:*string*, mesh:*integer*, face:*integer*, vert:*integer* | &lt;px:*float*, py:*float*, pz:*float*, s:*float*, t:*float*, nx:*float*, ny:*float*, nz:*float*, r:*float*, g:*float*, b:*float*, a:*float*&gt; |
| scene.material | 获取或设置节点网格材质 | name:*string*, mesh:*integer*, &lt;{param}&gt; | &lt;{param}&gt; |
| scene.renderMode | 设置场景节点渲染模式 | name:*string*, cullFace:*boolean*, useDepth:*boolean*, addColor:*boolean* | - |
| scene.boundingBox | 获取场景节点包围盒 | name:*string* | &lt;minx:*float*, miny:*float*, minz:*float*, maxx:*float*, maxy:*float*, maxz:*float*&gt; |
| scene.handle | 调用场景节点方法 | name:*string*, function:*string*, &lt;{param}&gt; | [ret:*any*] |
| post.list | 获取已注册的后处理列表 | - | {name:*string*} |
| post.enable | 获取或设置指定后处理 | name:*string*, &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| post.register | 注册用户自定义后处理 | name:*string*, shader:*string*, &lt;texture:*string*, cube:*boolean*&gt; | success:*boolean* |
| post.param | 设置后处理参数 | name:*string*, key:*string*, [value:*integer* / *float*] | - |
| graphics.screenshot | 屏幕截图 | file:*string* / *nil*, &lt;redraw:*boolean*&gt; | &lt;data:*string*&gt; |
| graphics.stereo | 获取或设置立体显示 | &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| graphics.windowSize | 获取窗口大小 | - | width:*integer*, height:*integer* |
| graphics.lightPosition | 获取光源位置 | - | x:*float*, y:*float*, z:*float*, w:*float*, dx:*float*, dy:*float*, dz:*float*, dw:*float* |
| graphics.directionLight | 设置方向光光源 | dx:*float*, dy:*float*, dz:*float*, &lt;r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.pointLight | 设置点光源 | x:*float*, y:*float*, z:*float*, &lt;range:*float*, r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.spotLight | 设置聚光灯光源 | x:*float*, y:*float*, z:*float*, &lt;dx:*float*, dy:*float*, dz:*float*, angle:*float*, range:*float*, r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.shadow | 获取或设置阴影是否显示 | &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| graphics.fog | 获取或设置雾是否显示 | &lt;enable:*boolean*, r:*float*, g:*float*, b:*float*, start:*float*, end:*float*&gt; | &lt;enable:*boolean*&gt; |
| graphics.environmentMap | 获取或设置环境贴图 | &lt;enable:*boolean*, cubemap:*string*&gt; | &lt;enable:*boolean*&gt; |
| graphics.background | 设置绘图背景颜色 | r:*float*, g:*float*, b:*float*, a:*float* | - |
| graphics.pickingRay | 从屏幕上的点获取空间射线 | x:*integer*, y:*integer* | x:*float*, y:*float*, z:*float*, dx:*float*, dy:*float*, dz:*float* |
| graphics.project | 获取空间点在屏幕上的位置 | x:*float*, y:*float*, z:*float* | x:*float*, y:*float*, z:*float* |
| graphics.renderTarget | 设置离线渲染目标 | texture:*string*, &lt;clear:*boolean*&gt; | last:*string* |
| texture.create | 创建或载入纹理 | name:*string*, &lt;width:*integer*, height:*integer*, shader:*string* / depth:*boolean*&gt; | &lt;texture:*string*&gt; |
| texture.delete | 删除纹理资源 | name:*string* | - |
| texture.update | 更新纹理数据 | name:*string*, file:*string* / color:*string* | success:*boolean* |
| texture.data | 读取纹理数据 | name:*string*, &lt;face:*integer*, level:*integer*&gt; | pngData:*string* |
| shader.create | 创建着色器 | name:*string*, vert:*string*, frag:*string* | &lt;shader:*string*&gt; |
| shader.delete | 删除着色器资源 | name:*string* | - |
| shader.update | 更新着色器程序 | name:*string*, vert:*string*, frag:*string* | success:*boolean* |
| shader.define | 着色器程序宏定义 | name:*string*, define:*string*, undef:*string* | success:*boolean* |
| shader.param | 设置着色器参数 | name:*string*, key:*string*, [value:*integer* / *float*] | - |
| font.list | 获取已加载的字体列表 | - | {name:*string*} |
| font.load | 加载字体 | name:*string*, file:*string* | success:*boolean* |
| font.clear | 清空全部已加载的字体 | - | - |
| font.use | 设置当前字体 | name:*string* | - |
| sound.create | 创建音源 | file:*string*, &lt;background:*boolean*, loop:*boolean*, gain:*float*, pitch:*float*&gt; | id:*integer* |
| sound.delete | 删除音源 | id:*integer* | - |
| sound.play | 开始播放声音 | id:*integer* | - |
| sound.pause | 暂停播放声音 | id:*integer* / pause:*boolean* | - |
| sound.stop | 停止播放声音 | id:*integer* | - |
| sound.position | 设置音源位置 | id:*integer*, x:*float*, y:*float*, z:*float* | - |
| sound.volume | 获取或设置音量 | &lt;volume:*integer*&gt; | &lt;volume:*integer*&gt; |
| sound.update | 更新音频数据 | original:*string*, file:*string* | success:*boolean* |
| physics.bind | 绑定场景节点到刚体 | name:*string*, {attrib} | - |
| physics.unbind | 取消场景节点刚体绑定 | name:*string* | - |
| physics.collide | 设置碰撞事件回调 | name:*string*, event:*function(enter:boolean, name:string, &lt;x:float, y:float, z:float&gt;)* | - |
| physics.reset | 将刚体速度设为零 | name:*string* | - |
| physics.velocity | 设置刚体速度 | name:*string*, lx:*float*, ly:*float*, lz:*float*, &lt;ax:*float*, ay:*float*, az:*float*&gt; | - |
| physics.applyForce | 向刚体施加力 | name:*string*, x:*float*, y:*float*, z:*float*, &lt;px:*float*, py:*float*, pz:*float*&gt; | - |
| physics.applyImpulse | 向刚体施加冲量 | name:*string*, x:*float*, y:*float*, z:*float*, &lt;px:*float*, py:*float*, pz:*float*&gt; | - |
| physics.gravity | 设置物理引擎重力 | x:*float*, y:*float*, z:*float* | - |
| physics.joint | 为关节设置物理模拟 | name:*string*, joint:*string*, &lt;mass:*float*, bendFactor:*float*, stretchFactor:*float*, damping:*float*&gt; | - |
| animation.scale | 添加缩放动画 | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.rotation | 添加旋转动画 | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.translation | 添加位移动画 | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.start | 开始执行动画 | name:*string*, &lt;repeat:*integer*, swing:*boolean*, delay:*float*&gt; | - |
| animation.stop | 停止执行动画 | name:*string*, &lt;reset:*boolean*&gt; | - |

@remarks <> 内部参数为可选参数，[] 内部参数为可变个数参数.
*/