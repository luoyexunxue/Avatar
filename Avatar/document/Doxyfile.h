/**
@mainpage Avatar - ��ƽ̨������3D����

##���##
<hr>
�����ҵ�һ��ҵ����Ŀ��Avatar ��һ���ƽ̨�����ܵ�3D���棬�ײ���� C++ ������Ŀǰ֧�� Windows��Linux��
Android ����ƽ̨����Ⱦ���ֻ��� OpenGL3.3/ES3.0��������кܸߵĿ���ֲ�ԣ����� freetype��glew��libcurl��
libjpeg��libpng��lua��minimp3��openal��rapidxml��zlib ��Щ��Դ�ĵ����������������������⡣������ص���
�򵥸�Ч��ͬʱ�ṩ�ǳ��ḻ�Ĺ��ܣ������������� Avatar (�룺Ů��)����һ��С���£������Ҵ�ѧ��ѧ���ĵ�һ
������(^_^)���������Լ��·����ܡ��������ߺ�������� http://blog.csdn.net/luoyexunxue�����������ʹ�ã�
�ɲο� @ref page_example

����Ĳ������ԣ�
<br/>
-# <b>֧�ֶ��������ʽ BMP TGA PNG JPG��ͬʱ֧�ֳ�������</b>
-# <b>֧�����붯��ģ�� MS3D DAE GLTF �;�̬ģ�� OBJ STL 3DS �Լ��������� BVH ���������Զ��������</b>
-# <b>���� OpenAL �� 3D ��Ч��֧�� WAV �� MP3 ��ʽ��Ƶ����</b>
-# <b>���� FreeType ����⣬֧���Զ���������ʾ</b>
-# <b>���� Lua �ű����棬֧��ʹ�� Lua ������Ľ��������ڽű��� GUI ϵͳ�����зǳ������¼���������</b>
-# <b>�ܹ�ʵ������ʵʱ��أ�����ͷ��Ƶ�ɼ�������ͷ���Ƶȹ���</b>
-# <b>ʵ����һ��С�Ϳ��ٵ��������棬�ܹ����и���ģ��</b>
-# <b>�ڽ����ֺ����������Ӿ�͸�ӽ�����HDR��FXAA������3D��VolumeLight �Լ��û��Զ������</b>
-# <b>֧�ֳ����ڵ㶯�����������ţ���ת��ƽ�ƣ��������ֶ�����ʽ</b>
-# <b>�����������������������һ�˳���Ϸ�������˳���Ϸ��ģ�����������ģ�⡢VRģʽ�ȶ��ֿ���</b>
-# <b>��������־ϵͳ����ָ��������𣬿�������ļ��Ϳ���̨</b>
-# <b>֧�ֶ�ȡ�����ļ���Դ�������ļ�ϵͳ��ѹ���ļ���HTTP/FTP ������Դ</b>
-# <b>���� Delaunay ���������ɡ�FFT �任/��任��JSON ������ģ�ͷֲ���Ƭ��UDP ����ͨ�ŵȻ�����</b>

##����##
<hr>
Avatar ���Ҵ�2013��3�·ݴǵ���һ�ݹ�����ʼ�ģ������Ŀֻ��Ϊ������ 3D ��Ϸ���󾭹��϶������ĸ��£��Ѿ���Գ���
<br/>
2013-03 ��ʼ���� Windows �汾<br/>
2014-04 ��ֲ�� Linux ϵͳ�ϣ���Ҫ��л MaYuan �İ���<br/>
2014-05 ��ֲ�� Raspberry Pi ��<br/>
2014-08 ��ֲ�� Android ����<br/>
2014-08 ʹ�����濪���� Android ƽ̨��Ϸ 3D ƴͼ<br/>
2014-09 �����������ϵͳ<br/>
2015-03 ͼ�������л��� OpenGL/ES 2.0 ��ɫ��ģʽ<br/>
2015-04 ���Ӻ��������<br/>
2015-05 ʵ���˴�����ɢ����Ⱦ<br/>
2015-05 ���� Lua �ű�����<br/>
2015-08 ʵ���˻�����Ļ��ͷ����Ч����Ⱦ<br/>
2015-08 ��ӻ��ڽű��� GUI ϵͳ<br/>
2015-10 ��� ShadowMap ֧�ֺ���Ч��֧��<br/>
2015-11 �����������ƣ���������ײ���ͷ�Ӧ����<br/>
2016-02 ��Ӷ������棬֧��������תƽ�ƶ���<br/>
2016-04 �޸��˳����ڵ㣬�����˳��������ḻ�˽ű��ӿ�<br/>
2016-09 ��������������ȶ�������<br/>
2016-10 ������ ZIP �ļ�ϵͳ����ֱ�Ӵ� ZIP �ж�ȡ�ļ�<br/>
2017-03 ������ MeshLoader �ӿڣ��������� 3DS ֧��<br/>
2017-03 �����˳������� Procedural Texture<br/>
2017-07 ������ Collada(DAE) ģ�͵�֧��<br/>
2017-12 ͼ����Ⱦ API ������ OpenGL3.3/ES3.0��ȥ���� Raspberry Pi ��֧��<br/>
2018-01 ȥ���˶� libiconv ������������ UTF-8 �� wchar_t �໥ת����֧��<br/>
2018-02 �����˹��������ļ�����ģ�⣬���ڴ˿�ʵ����ҡ��Ч��<br/>
2018-04 ����ϵͳ��Ϊ��������� BRDF ��ʽ<br/>
2018-10 ������ Geographic ����������ڵ�����Ϣϵͳ��Ӧ��<br/>
2019-02 ������ȫ��ͼ��Ⱦ����<br/>
2019-03 ��� GLTF ģ���ļ����빦��<br/>
2019-05 �����������<br/>
2019-09 PBR ���� IBL ������������<br/>

##�ܹ�##
<hr>
һ��Ŀ¼�ṹ<br/>
@code
Avatar		-- ���̵���Ŀ¼
|-	extension	-- ��չ�����Ŀ¼
|-	media		-- ����������Ҫ����Դ
|-	thirdparty 	-- ��������
|	|-	freetype	-- freetype ������Ⱦ��
|	|-	glew		-- OpenGL ��չ��
|	|-	libcurl		-- URL ���������
|	|-	libjpeg		-- jpg �ļ���д��
|	|-	libpng		-- png �ļ���д��
|	|-	lua			-- lua �ű�������
|	|-	minimp3		-- mp3 �ļ������
|	|-	openal		-- OpenAL ��
|	|-	rapidxml	-- XML ������
|	|-	zlib		-- �ļ�ѹ����
extension	-- ��չ��Ŀ¼
|-	VideoPlay	-- ʹ�� FFmpeg ����Ƶ������չ�����ṩDLL����汾��������Ҫ FFmpeg ֧�֣�
|-	VideoCap	-- ʹ�� OpenCV ������ͷ��Ƶ��չ�����ṩDLL����汾��������Ҫ OpenCV 2.4 ֧�֣�
@endcode
������Ҫ���<br/>
CEngine ������ģ����������Ļ�ȡ��<br/>
CDevice �豸����㣬ʵ���� CDeviceWindows��CDeviceLinux��CDeviceAndroid<br/>
CGraphicsManager ����ͼ�β�����������Ⱦ��������ã���Դ���ã���Ӱ���ã���Ч���ã�������ʰȡ�ȷ���<br/>
CFileManager �����ļ�����������Ŀ¼������BIN��TXT��BMP��TGA��PNG��JPG��WAV��MP3 �ļ��Ķ�ȡ��д��<br/>
CInputManager �����������������֧�����ң�ǰ�����£���λ�ǣ������ǣ������ǣ�������Ծ����ʼ����ͣ���˳���״̬�趨������ѡ����������<br/>
CSoundManager ����������Դ��ʵ�ֻ��� OpenAL������ 3D ��Դ�Ĵ��������ţ���ͣ��ֹͣ�����������ã��Լ�����λ�á��ٶȡ���������<br/>
CSceneManager �������ڵ������ṩͨ�� ID�����ƣ����ͣ�����ʰȡ�����еĳ����ڵ㣬�������ڵ���룬ɾ�����Զ�����Ȳ���<br/>
CFontManager ����������Դ��ʹ�� FreeType �����������Լ���Ⱦ���壬֧�� FreeType��TrueType �������룬��������ͼƬ֧�ֶ��뷽ʽ<br/>
CTextureManager ����������Դ�����Ż�������Դ��ʹ�ã��ɴ�����ͨ��ά���������������ڴ�������Ⱦ����֧�ֻҶȣ�RGB��RGBA ��ʽ<br/>
CShaderManager ������ɫ��������Դ��Ϊ��ɫ����Դ�ṩͳһ�Ĺ��������Ż�ʹ��<br/>
CPhysicsManager ��������ģ�⣬ģ������˶�����ײ���<br/>
CScriptManager ����ű��ӿڣ������� Lua ��Ϊ�ű����ԣ��ṩ�ḻ�Ľ����ӿ�<br/>
CPostProcessManager ������������Դ������Ⱦ������к������<br/>
CAnimationManager ������ģ�⣬Ϊ�����ڵ��ṩ���䶯�����������š���ת��ƽ�ƶ���<br/>

##����##
<hr>
��Ҫ����������������ļ���Windows ƽ̨���ṩԤ����汾<br/>
| ���� | ���� | ���� |
| :------- | :--- | :------ |
| glew | OpenGL ��չ | CGraphicsManager CShaderManager CTextureManager CMesh |
| openal | ��ά��Ч�� | CSoundManager |
| zlib | ѹ�����ѹ���� | CZipReader |
| libpng | PNG �ļ���д�� | CFileManager |
| libjpeg | JPG �ļ���д�� | CFileManager |
| freetype | �����ȡ�� | CFontManager |
| lua | �ű������� | CScriptManager CScriptContext |
| libcurl | URL ��������� | CUrlConnection |
����ǰ�����޸� AvatarConfig.h �ļ���������<br/>
AVATAR_ENABLE_VLD ʹ�� Visual Leak Detector ���ڴ�й©���<br/>
AVATAR_ENABLE_VIDEOCAP ʹ���������׽�⣨Ŀǰֻ���� Windows ��ʹ�ã�<br/>
AVATAR_ENABLE_VIDEOPLAY ʹ����Ƶ���ſ�<br/>
AVATAR_ENABLE_LOGO ����Ļ���Ͻ���ʾ���� LOGO<br/>
AVATAR_ENABLE_MOUSE �����������<br/>
AVATAR_ENABLE_KEYBOARD ���ü�������<br/>
AVATAR_CONTROL_NETWORK ����������Ϸ�ֱ����ƣ�UDPЭ�飬JSON���ݣ�<br/>
*/

/**
@page page_faq ��������

##��������##
<hr>
- �ռ�����ϵΪ��������ϵ��X ��Ϊ��Ļ���ң�Y ��Ϊ��Ļ���Z ��Ϊ��Ļ����
- ��Ļ����ϵԭ��Ϊ���½ǣ�X ������Ϊ���ң�Y ��������Ϊ����
- ������Ϊ���������з�ʽ���� OpenGL ����
- �����������������ת�þ���δ���ŵ���ת���������������
- ����ͨ��0��7�����Զ�����;��ͨ��8Ϊ��Ӱ��ͼ��ͨ��9/10/11ΪIBLͨ��

##���뼼��##
<hr>
- �Զ������ͳ��������ʹ�� uniform float uElapsedTime ��ȡ��ǰʱ��
- �Զ������ͳ��������ʹ�� uniform vec2 uResolution ��ȡ��Ļ�ֱ���
- �Զ������ͳ��������ʹ�� in vec2 vTexCoord ��Ϊ������������
- �Զ������ʹ�� uniform sampler2D uSamples[] ��Ϊ������������
- GUI �ű��ļ���ʹ�� UTF-8 ���룬�Է�ֹ GUI ������ʾ����

##��ɫ������##
<hr>
-# ���Ի����
@code
uniform sampler2D depthSampler;
const float zNear = 0.1;
const float zFar = 2000.0;
float LinearizeDepth(vec2 uv) {
	float z = texture2D(depthSampler, uv).x;
	return (2.0 * zNear) / (zFar + zNear - z * (zFar - zNear));
}
@endcode
-# ��ƺ��Ⱦ
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
-# Gamma����
@code
color = color / (color + vec3(1.0));
color = pow(color, vec3(1.0 / 2.2));
@endcode
*/

/**
@page page_example ʾ������

##��ʾ��##
<hr>
@code
#include <CEngine.h>
int main(int argc, char **argv)
{
	// ��ʼ���������
	SEngineConfig conf;
	// ��ԴĿ¼
	conf.directory = "C:\\Avatar\\media";
	// ��ʼ��
	CEngine* engine = new CEngine();
	if (!engine->Init(conf))
	{
		return 1;
	}
	// ��ѭ��
	while (engine->IsRunning())
	{
		engine->Update();
		engine->Render();
	}
	// ����ʵ��
	engine->Destroy();
	delete engine;
	return 0;
}
@endcode
����ǳ��򵥵�����չʾ������Ļ���ʹ�ã�������ӻᴴ��һ�����壬������ C:\\Avatar\\media Ŀ¼�µ� avatar.lua �ű������ϵͳ��Դ��
*/

/**
@page page_interface �ű��ӿ�

##�ص�һ����##
<hr>
| �ӿ����� | ���� | ���� | ��ע |
| :------- | :--- | :--- | :--- |
| OnReady | ��ʼ����� | - | ��ʼ�����ʱ����һ�� |
| OnUpdate | �߼����� | dt:*float* | - |
| OnExit | �����˳� | - | �����������ʱ����һ�� |
| OnSize | ���ڴ�С�ı� | width:*integer*, height:*integer* | - |
| OnInput | �����¼� | name:*string*, value:*integer*, arg1:*number*, arg2:*number*, arg3:*number* | name Ϊ 'function/key/fire/move/turn' |

##�ӿ�һ����##
<hr>
| �ӿ����� | ���� | ���� | ����ֵ |
| :------- | :--- | :--- | :----- |
| engine.info | ��ȡ������Ϣ | - | {version:*string*, platform:*string*} |
| engine.fps | ��ȡ��ǰ֡�� | - | fps:*float* |
| engine.log | �����־ | [msg:*any*] | - |
| engine.speed | ��������ʱ����� | speed:*float*, &lt;fixed:*boolean*&gt; | - |
| engine.timer | ��ȡ��ʱ����ʱ | name:*string*, &lt;reset:*boolean*&gt; | elapse:*float* |
| engine.directory | ��ȡ����������Ŀ¼ | &lt;dir:*string*&gt; | &lt;dir:*string*&gt; |
| engine.input | ������Ϣ | method:*string*, [params:*boolean* / *integer* / *float*] | - |
| engine.script | ִ�� LUA �ű� | file:*string* | success:*boolean* |
| engine.execute | ִ�лص����� | function:*string*, &lt;argument:*string*&gt; | result:*integer* |
| engine.read | ��ȡָ���ļ����� | file:*string* | data:*string* |
| engine.write | д��������ָ���ļ� | file:*string*, data:*string* | - |
| engine.plugin | ���������� | path:*string* | desc:*string* |
| gui.enable | ʹ�ܻ��ֹ GUI | enable:*boolean* | - |
| gui.size | ��ȡ GUI ������С | - | width:*integer*, height:*integer* |
| gui.scale | ���� GUI ���� | scale:*float* | - |
| gui.create | ���� GUI Ԫ�� | name:*string*, type:*string*, desc:*string*, &lt;event:*function(evt:integer, arg1:integer, arg2:integer)*&gt; | - |
| gui.modify | �޸� GUI Ԫ�� | name:*string*, desc:*string* | - |
| gui.delete | ɾ�� GUI Ԫ�� | name:*string* | - |
| gui.attrib | ��ȡ GUI Ԫ������ | name:*string*, attrib:*string* | value:*string* |
| camera.type | ��ȡ������������� | &lt;type:*string*, {param}&gt; | &lt;type:*string*&gt; |
| camera.control | ���û��������� | enable:*boolean* | - |
| camera.fov | ��ȡ����������ӽ� | &lt;fov:*float*&gt; | &lt;fov:*float*&gt; |
| camera.clip | ��ȡ����������ü��� | &lt;near:*float*, far:*float*&gt; | &lt;near:*float*, far:*float*&gt; |
| camera.projection | ��ȡ������ͶӰ��ʽ | &lt;ortho:*boolean*, width:*float*, height:*float*&gt; | &lt;ortho:*boolean*&gt; |
| camera.position | ��ȡ���������λ�� | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.target | ��ȡ���������Ŀ��� | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.angle | ��ȡ����������Ƕ� | &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| camera.direction | ��ȡ����������������� | - | lx:*float*, ly:*float*, lz:*float*, rx:*float*, ry:*float*, rz:*float*, ux:*float*, uy:*float*, uz:*float* |
| camera.bind | �󶨳����ڵ������ | &lt;name:*string*, x:*float*, y:*float*, z:*float*, ax:*float*, ay:*float*, az:*float*&gt; | - |
| scene.insert | ���볡���ڵ� | parent:*string*, type:*string*, name:*string*, {attrib} | success:*boolean* |
| scene.delete | ɾ�������ڵ� | name:*string* | - |
| scene.clear | ������г����ڵ� | - | - |
| scene.list | ��ȡ�����ڵ��б� | - | {name:*string*} |
| scene.pick | ����ѡ�񳡾��ڵ� | x:*float*, y:*float*, z:*float*, dx:*float*, dy:*float*, dz:*float* | name:*string* / *nil*, &lt;x:*float*, y:*float*, z:*float*, mesh:*integer*, face:*integer*&gt; |
| scene.scale | ��ȡ�����ó����ڵ����� | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.angle | ��ȡ�����ó����ڵ�Ƕ� | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.orientation | ��ȡ�����ó����ڵ㷽λ | name:*string*, &lt;x:*float*, y:*float*, z:*float*, w:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*, w:*float*&gt; |
| scene.position | ��ȡ�����ó����ڵ�λ�� | name:*string*, &lt;x:*float*, y:*float*, z:*float*&gt; | &lt;x:*float*, y:*float*, z:*float*&gt; |
| scene.visible | ��ȡ�����ó����ڵ�ɼ��� | name:*string*, &lt;visible:*boolean*&gt; | &lt;visible:*boolean*&gt; |
| scene.update | ���³����ڵ����� | name:*string*, mesh:*integer*, {data} | - |
| scene.vertex | ��ȡ�����ڵ����񶥵���Ϣ | name:*string*, mesh:*integer*, face:*integer*, vert:*integer* | &lt;px:*float*, py:*float*, pz:*float*, s:*float*, t:*float*, nx:*float*, ny:*float*, nz:*float*, r:*float*, g:*float*, b:*float*, a:*float*&gt; |
| scene.material | ��ȡ�����ýڵ�������� | name:*string*, mesh:*integer*, &lt;{param}&gt; | &lt;{param}&gt; |
| scene.renderMode | ���ó����ڵ���Ⱦģʽ | name:*string*, cullFace:*boolean*, useDepth:*boolean*, addColor:*boolean* | - |
| scene.boundingBox | ��ȡ�����ڵ��Χ�� | name:*string* | &lt;minx:*float*, miny:*float*, minz:*float*, maxx:*float*, maxy:*float*, maxz:*float*&gt; |
| scene.handle | ���ó����ڵ㷽�� | name:*string*, function:*string*, &lt;{param}&gt; | [ret:*any*] |
| post.list | ��ȡ��ע��ĺ����б� | - | {name:*string*} |
| post.enable | ��ȡ������ָ������ | name:*string*, &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| post.register | ע���û��Զ������ | name:*string*, shader:*string*, &lt;texture:*string*, cube:*boolean*&gt; | success:*boolean* |
| post.param | ���ú������ | name:*string*, key:*string*, [value:*integer* / *float*] | - |
| graphics.screenshot | ��Ļ��ͼ | file:*string* / *nil*, &lt;redraw:*boolean*&gt; | &lt;data:*string*&gt; |
| graphics.stereo | ��ȡ������������ʾ | &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| graphics.windowSize | ��ȡ���ڴ�С | - | width:*integer*, height:*integer* |
| graphics.lightPosition | ��ȡ��Դλ�� | - | x:*float*, y:*float*, z:*float*, w:*float*, dx:*float*, dy:*float*, dz:*float*, dw:*float* |
| graphics.directionLight | ���÷�����Դ | dx:*float*, dy:*float*, dz:*float*, &lt;r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.pointLight | ���õ��Դ | x:*float*, y:*float*, z:*float*, &lt;range:*float*, r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.spotLight | ���þ۹�ƹ�Դ | x:*float*, y:*float*, z:*float*, &lt;dx:*float*, dy:*float*, dz:*float*, angle:*float*, range:*float*, r:*float*, g:*float*, b:*float*&gt; | - |
| graphics.shadow | ��ȡ��������Ӱ�Ƿ���ʾ | &lt;enable:*boolean*&gt; | &lt;enable:*boolean*&gt; |
| graphics.fog | ��ȡ���������Ƿ���ʾ | &lt;enable:*boolean*, r:*float*, g:*float*, b:*float*, start:*float*, end:*float*&gt; | &lt;enable:*boolean*&gt; |
| graphics.environmentMap | ��ȡ�����û�����ͼ | &lt;enable:*boolean*, cubemap:*string*&gt; | &lt;enable:*boolean*&gt; |
| graphics.background | ���û�ͼ������ɫ | r:*float*, g:*float*, b:*float*, a:*float* | - |
| graphics.pickingRay | ����Ļ�ϵĵ��ȡ�ռ����� | x:*integer*, y:*integer* | x:*float*, y:*float*, z:*float*, dx:*float*, dy:*float*, dz:*float* |
| graphics.project | ��ȡ�ռ������Ļ�ϵ�λ�� | x:*float*, y:*float*, z:*float* | x:*float*, y:*float*, z:*float* |
| graphics.renderTarget | ����������ȾĿ�� | texture:*string*, &lt;clear:*boolean*&gt; | last:*string* |
| texture.create | �������������� | name:*string*, &lt;width:*integer*, height:*integer*, shader:*string* / depth:*boolean*&gt; | &lt;texture:*string*&gt; |
| texture.delete | ɾ��������Դ | name:*string* | - |
| texture.update | ������������ | name:*string*, file:*string* / color:*string* | success:*boolean* |
| texture.data | ��ȡ�������� | name:*string*, &lt;face:*integer*, level:*integer*&gt; | pngData:*string* |
| shader.create | ������ɫ�� | name:*string*, vert:*string*, frag:*string* | &lt;shader:*string*&gt; |
| shader.delete | ɾ����ɫ����Դ | name:*string* | - |
| shader.update | ������ɫ������ | name:*string*, vert:*string*, frag:*string* | success:*boolean* |
| shader.define | ��ɫ������궨�� | name:*string*, define:*string*, undef:*string* | success:*boolean* |
| shader.param | ������ɫ������ | name:*string*, key:*string*, [value:*integer* / *float*] | - |
| font.list | ��ȡ�Ѽ��ص������б� | - | {name:*string*} |
| font.load | �������� | name:*string*, file:*string* | success:*boolean* |
| font.clear | ���ȫ���Ѽ��ص����� | - | - |
| font.use | ���õ�ǰ���� | name:*string* | - |
| sound.create | ������Դ | file:*string*, &lt;background:*boolean*, loop:*boolean*, gain:*float*, pitch:*float*&gt; | id:*integer* |
| sound.delete | ɾ����Դ | id:*integer* | - |
| sound.play | ��ʼ�������� | id:*integer* | - |
| sound.pause | ��ͣ�������� | id:*integer* / pause:*boolean* | - |
| sound.stop | ֹͣ�������� | id:*integer* | - |
| sound.position | ������Դλ�� | id:*integer*, x:*float*, y:*float*, z:*float* | - |
| sound.volume | ��ȡ���������� | &lt;volume:*integer*&gt; | &lt;volume:*integer*&gt; |
| sound.update | ������Ƶ���� | original:*string*, file:*string* | success:*boolean* |
| physics.bind | �󶨳����ڵ㵽���� | name:*string*, {attrib} | - |
| physics.unbind | ȡ�������ڵ����� | name:*string* | - |
| physics.collide | ������ײ�¼��ص� | name:*string*, event:*function(enter:boolean, name:string, &lt;x:float, y:float, z:float&gt;)* | - |
| physics.reset | �������ٶ���Ϊ�� | name:*string* | - |
| physics.velocity | ���ø����ٶ� | name:*string*, lx:*float*, ly:*float*, lz:*float*, &lt;ax:*float*, ay:*float*, az:*float*&gt; | - |
| physics.applyForce | �����ʩ���� | name:*string*, x:*float*, y:*float*, z:*float*, &lt;px:*float*, py:*float*, pz:*float*&gt; | - |
| physics.applyImpulse | �����ʩ�ӳ��� | name:*string*, x:*float*, y:*float*, z:*float*, &lt;px:*float*, py:*float*, pz:*float*&gt; | - |
| physics.gravity | ���������������� | x:*float*, y:*float*, z:*float* | - |
| physics.joint | Ϊ�ؽ���������ģ�� | name:*string*, joint:*string*, &lt;mass:*float*, bendFactor:*float*, stretchFactor:*float*, damping:*float*&gt; | - |
| animation.scale | ������Ŷ��� | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.rotation | �����ת���� | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.translation | ���λ�ƶ��� | name:*string*, x:*float*, y:*float*, z:*float*, interpolator:*string*, duration:*float* | - |
| animation.start | ��ʼִ�ж��� | name:*string*, &lt;repeat:*integer*, swing:*boolean*, delay:*float*&gt; | - |
| animation.stop | ִֹͣ�ж��� | name:*string*, &lt;reset:*boolean*&gt; | - |

@remarks <> �ڲ�����Ϊ��ѡ������[] �ڲ�����Ϊ�ɱ��������.
*/