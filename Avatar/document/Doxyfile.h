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
| OnReady | ��ʼ����� | - | - |
| OnUpdate | �߼����� | dt | - |
| OnExit | �����˳� | - | - |
| OnSize | ���ڴ�С�ı� | width, height | - |
| OnInput | �����¼� | name, value, arg1, arg2, arg3 | name Ϊ 'function' \| 'key' \| 'fire' \| 'move' \| 'turn' |

##�ӿ�һ����##
<hr>
| �ӿ����� | ���� | ���� | ����ֵ |
| :------- | :--- | :--- | :----- |
| engine.info | ��ȡ������Ϣ | - | {VERSION, PLATFORM} |
| engine.fps | ��ȡ��ǰ֡�� | - | fps |
| engine.log | ʹ����־�������־ | enable, &lt;level&gt; \| msg | - |
| engine.speed | ��������ʱ����� | speed, &lt;fixed&gt; | - |
| engine.timer | ��ȡ��ʱ����ʱ | name, &lt;reset&gt; | elapse |
| engine.directory | ��ȡ����������Ŀ¼ | &lt;dir&gt; | &lt;dir&gt; |
| engine.input | ������Ϣ | method, [params] | - |
| engine.script | ִ�� LUA �ű� | file | success |
| engine.execute | ִ�лص����� | function, &lt;argument&gt; | result |
| engine.read | ��ȡָ���ļ����� | file | data |
| engine.write | д��������ָ���ļ� | file, data | - |
| engine.plugin | ���������� | path | desc |
| gui.enable | ʹ�ܻ��ֹ GUI | enable | - |
| gui.size | ��ȡ GUI ������С | - | width, height |
| gui.scale | ���� GUI ���� | scale | - |
| gui.create | ���� GUI Ԫ�� | name, type, desc, &lt;event&gt; | - |
| gui.modify | �޸� GUI Ԫ�� | name, desc | - |
| gui.delete | ɾ�� GUI Ԫ�� | name | - |
| gui.attrib | ��ȡ GUI Ԫ������ | name, attrib | value |
| camera.type | ��ȡ������������� | &lt;type, {param}&gt; | &lt;type&gt; |
| camera.control | ���û��������� | enable | - |
| camera.fov | ��ȡ����������ӽ� | &lt;fov&gt; | &lt;fov&gt; |
| camera.clip | ��ȡ����������ü��� | &lt;near, far&gt; | &lt;near, far&gt; |
| camera.position | ��ȡ���������λ�� | &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| camera.target | ��ȡ���������Ŀ��� | &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| camera.angle | ��ȡ����������Ƕ� | &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| camera.direction | ��ȡ����������������� | - | lx, ly, lz, rx, ry, rz, ux, uy, uz |
| camera.bind | ��ָ�������ڵ������ | &lt;name, x, y, z, ax, ay, az&gt; | - |
| scene.insert | ���볡���ڵ� | parent, type, name, {attrib} | success |
| scene.delete | ɾ�������ڵ� | name | - |
| scene.clear | ������г����ڵ� | - | - |
| scene.list | ��ȡ�����ڵ��б� | - | {name} |
| scene.pick | ����ѡ�񳡾��ڵ� | x, y, z, dx, dy, dz | name, x, y, z, mesh, face / empty |
| scene.scale | ��ȡ�����ó����ڵ����� | name, &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| scene.angle | ��ȡ�����ó����ڵ�Ƕ� | name, &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| scene.orientation | ��ȡ�����ó����ڵ㷽λ | name, &lt;x, y, z, w&gt; | &lt;x, y, z, w&gt; |
| scene.position | ��ȡ�����ó����ڵ�λ�� | name, &lt;x, y, z&gt; | &lt;x, y, z&gt; |
| scene.visible | ��ȡ�����ó����ڵ�ɼ��� | name, &lt;visible&gt; | &lt;visible&gt; |
| scene.update | ���³����ڵ����� | name, mesh, {data} | - |
| scene.vertex | ��ȡ�����ڵ����񶥵���Ϣ | name, mesh, face, vert | &lt;px, py, pz, s, t, nx, ny, nz, r, g, b, a&gt; |
| scene.material | ��ȡ�����ó����ڵ�������� | name, mesh, &lt;{param}&gt; | &lt;{param}&gt; |
| scene.renderMode | ���ó����ڵ���Ⱦģʽ | name, cullFace, useDepth, addColor | - |
| scene.boundingBox | ��ȡ�����ڵ��Χ�� | name | &lt;minx, miny, minz, maxx, maxy, maxz&gt; |
| scene.handle | ���ó����ڵ㷽�� | name, function, &lt;{param}&gt; | [ret] |
| post.list | ��ȡ��ע��ĺ����б� | - | {name} |
| post.enable | ��ȡ������ָ������ | name, &lt;enable&gt; | &lt;enable&gt; |
| post.register | ע���û��Զ������ | name, shader, &lt;texture, cube&gt; | success |
| post.param | ���ú������ | name, key, [values] | - |
| graphics.screenshot | ��Ļ��ͼ | file, &lt;redraw&gt; | - |
| graphics.stereo | ��ȡ������������ʾ | &lt;enable&gt; | &lt;enable&gt; |
| graphics.windowSize | ��ȡ���ڴ�С | - | width, height |
| graphics.lightPosition | ��ȡ��Դλ�� | - | x, y, z, w |
| graphics.directionLight | ���÷�����Դ | dx, dy, dz, &lt;r, g, b&gt; | - |
| graphics.pointLight | ���õ��Դ | x, y, z, &lt;range, r, g, b&gt; | - |
| graphics.spotLight | ���þ۹�ƹ�Դ | x, y, z, &lt;dx, dy, dz, angle, range, r, g, b&gt; | - |
| graphics.shadow | ��ȡ��������Ӱ�Ƿ���ʾ | &lt;enable&gt; | &lt;enable&gt; |
| graphics.fog | ��ȡ���������Ƿ���ʾ | &lt;enable, r, g, b, start, end&gt; | &lt;enable&gt; |
| graphics.environmentMap | ��ȡ�����û�����ͼ | &lt;enable, cubemap&gt; | &lt;enable&gt; |
| graphics.background | ���û�ͼ������ɫ | r, g, b, a | - |
| graphics.pickingRay | ����Ļ�ϵĵ��ȡ�ռ����� | x, y | x, y, z, dx, dy, dz |
| graphics.project | ��ȡ�ռ��ͶӰ����Ļ�ϵ�λ�� | x, y, z | x, y, z |
| graphics.renderTarget | ����������ȾĿ�� | texture, &lt;clear&gt; | last |
| texture.create | �������������� | name, &lt;width, height, shader \| depth&gt; | &lt;texture&gt; |
| texture.delete | ɾ��������Դ | name | - |
| texture.update | ������������ | name, file | success |
| shader.create | ������ɫ�� | name, vert, frag | &lt;shader&gt; |
| shader.delete | ɾ����ɫ����Դ | name | - |
| shader.update | ������ɫ������ | name, define, undef | success |
| shader.param | ������ɫ������ | name, key, [values] | - |
| font.list | ��ȡ�Ѽ��ص������б� | - | {name} |
| font.load | �������� | name, file | success |
| font.clear | ���ȫ���Ѽ��ص����� | - | - |
| font.use | ���õ�ǰ���� | name | - |
| sound.create | ������Դ | file, &lt;background, loop, gain, pitch&gt; | id |
| sound.delete | ɾ����Դ | id | - |
| sound.play | ��ʼ�������� | id | - |
| sound.pause | ��ͣ�������� | id | - |
| sound.stop | ֹͣ�������� | id | - |
| sound.position | ������Դλ�� | id, x, y, z | - |
| sound.volume | ��ȡ���������� | &lt;volume&gt; | &lt;volume&gt; |
| sound.update | ������Ƶ���� | original, file | success |
| physics.bind | �󶨳����ڵ㵽������� | name, {attrib} | - |
| physics.unbind | ȡ�������ڵ������ģ�� | name | - |
| physics.collide | ������ײ�¼��ص� | name, event | - |
| physics.reset | ���������ٶȺͽ��ٶ���Ϊ�� | name | - |
| physics.velocity | ���ø������ٶȺͽ��ٶ� | name, lx, ly, lz, &lt;ax, ay, az&gt; | - |
| physics.applyForce | �����ʩ���� | name, x, y, z, &lt;px, py, pz&gt; | - |
| physics.applyImpulse | �����ʩ�ӳ��� | name, x, y, z, &lt;px, py, pz&gt; | - |
| physics.gravity | ���������������� | x, y, z | - |
| physics.joint | Ϊ�ؽ���������ģ�� | name, joint, &lt;mass, hk, vk, damping&gt; | - |
| animation.param | ���ö������� | name, type, duration, &lt;repeat, swing&gt; | - |
| animation.scale | �������Ŷ��� | name, x1, y1, z1, x2, y2, z2 | - |
| animation.rotation | ������ת���� | name, x1, y1, z1, x2, y2, z2 | - |
| animation.translation | ����λ�ƶ��� | name, x1, y1, z1, x2, y2, z2 | - |
| animation.start | ��ʼִ�ж��� | name, &lt;delay&gt; | - |
| animation.stop | ִֹͣ�ж��� | name | - |

@remarks <> �ڲ�����Ϊ��ѡ������[] �ڲ�����Ϊ�ɱ��������.
*/