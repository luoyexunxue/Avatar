local frag1 = [[
uniform sampler2D uTexture;
uniform float uElapsedTime;
uniform vec2 uResolution;
in vec2 vTexCoord;
out vec4 fragColor;
void main()
{
	vec2 position = vec2(2.0) * vTexCoord - vec2(1.0);
	vec2 uv;
	float r = sqrt(dot(position, position));
	float a = atan(position.y, position.x) + 0.9 * sin(0.5 * r - 0.5 * uElapsedTime);
	float h = (0.5 + 0.5 * sin(9.0 * a));
	float s = smoothstep(0.4, 0.5, h);
	uv.x = uElapsedTime + 1.0 / (r + 0.1 * s);
	uv.y = 3.0 * a / 3.1416;
	vec3 col = texture(uTexture, uv).xyz;
	float ao = smoothstep(0.0, 0.3, h) - smoothstep(0.5, 1.0, h);
	col *= 1.0 - 0.6 * ao * r;
	col *= r * r;
	fragColor = vec4(col, 1.0);
}
]]

local frag2 = [[
uniform sampler2D uTexture;
uniform vec2 uResolution;
in vec2 vTexCoord;
out vec4 fragColor;
float pattern()
{
	float s = sin(1.1);
	float c = cos(1.1);
	vec2 tex = vTexCoord * uResolution - vec2(0.5);
	vec2 point = vec2(c * tex.x - s * tex.y, s * tex.x + c * tex.y) * 3.0;
	return (sin(point.x) * sin(point.y)) * 4.0;
}
void main()
{
	vec4 color = texture(uTexture, vTexCoord);
	float average = (color.r + color.g + color.b) / 3.0;
	fragColor = vec4(vec3(average * 10.0 - 5.0 + pattern()), 1.0);
}
]]

local frag3 = [[
uniform samplerCube uTexture;
uniform vec2 uResolution;
uniform float uScale = 0.2;
uniform float uHeight = 1.0;
in vec2 vTexCoord;
out vec4 fragColor;
void main()
{
	float r = uScale * min(uResolution.x, uResolution.y);
	vec2 uv = (vTexCoord - 0.5) * uResolution;
	vec3 pp = vec3(uv.x, uv.y, -r);
	vec3 pa = vec3(0.0, 0.0, uHeight * r);
	vec3 dir = normalize(pp - pa);
	float d = -dot(pa, dir);
	float k = uHeight * r;
	float n = sqrt(r * r - (k * k - d * d));
	vec3 sample = dir * (d + n) + pa;
	fragColor = vec4(texture(uTexture, sample));
	fragColor.a = 1.0;
}
]]

local frag4 = [[
uniform sampler2D uTexture;
uniform float uElapsedTime;
uniform vec2 uResolution;
in vec2 vTexCoord;
out vec4 fragColor;
const int radius = 10;

float rand(vec2 co){
	// implementation found at: lumina.sourceforge.net/Tutorials/Noise.html
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise2f( in vec2 p )
{
	vec2 ip = vec2(floor(p));
	vec2 u = fract(p);
	// http://www.iquilezles.org/www/articles/morenoise/morenoise.htm
	u = u*u*(3.0-2.0*u);
	//u = u*u*u*((6.0*u-15.0)*u+10.0);
	
	float res = mix(
		mix(rand(ip),  rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),   rand(ip+vec2(1.0,1.0)),u.x),
		u.y)
	;
	return res*res;
	//return 2.0* (res-10.7);
}

float fbm(vec2 c) {
	float f = 0.0;
	float w = 1.0;
	for (int i = 0; i < 8; i++) {
		f+= w*noise2f(c);
		c*=2.0;
		w*=0.5;
	}
	return f;
}



vec2 cMul(vec2 a, vec2 b) {
	return vec2( a.x*b.x -  a.y*b.y,a.x*b.y + a.y * b.x);
}

float pattern(  vec2 p, out vec2 q, out vec2 r )
{
	q.x = fbm( p  +0.00*uElapsedTime * 2.); // @SLIDER: 5. could represent velocity of water
	q.y = fbm( p + vec2(1.0));
	
	r.x = fbm( p +1.0*q + vec2(1.7,9.2)+0.15*uElapsedTime * 2. );
	r.y = fbm( p+ 1.0*q + vec2(8.3,2.8)+0.126*uElapsedTime * 2.);
	//r = cMul(q,q+0.1*time);
	return fbm(p +1.0*r + 0.0* uElapsedTime);
}

const vec3 color1 = vec3(0.101961,0.619608,0.666667);
const vec3 color2 = vec3(0.666667,0.666667,0.498039);
const vec3 color3 = vec3(0,0,0.164706);
const vec3 color4 = vec3(0.666667,1,1);


vec3 screen( vec3 s, vec3 d )
{
	return s + d - s * d;
}

float overlay( float s, float d )
{
	return (d < 0.5) ? 2.0 * s * d : 1.0 - 2.0 * (1.0 - s) * (1.0 - d);
}

vec3 overlay( vec3 s, vec3 d )
{
	vec3 c;
	c.x = overlay(s.x,d.x);
	c.y = overlay(s.y,d.y);
	c.z = overlay(s.z,d.z);
	return c;
}


//	rgb<-->hsv functions by Sam Hocevar
//	http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c)
{
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
	
	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 saturation( vec3 s, vec3 d )
{
	d = rgb2hsv(d);
	d.y = rgb2hsv(s).y;
	return hsv2rgb(d);
}


vec3 sample2(const int x, const int y, vec2 delta, vec2 fragCoord)
{
	vec2 uv = (fragCoord.xy + vec2(x, y)) / uResolution.xy;
	uv = uv + delta;
	//uv.y = 1.0 - uv.y;
	
	return texture(uTexture, uv).xyz;
}



 void main() 
 {
	 vec2 src_size = vec2 (1.0 / uResolution.x, 1.0 / uResolution.y);
     vec2 uv = gl_FragCoord.xy/uResolution.xy;
     float n = float((radius + 1) * (radius + 1));
     int i; 
	 int j;
     vec3 m0 = vec3(0.0); vec3 m1 = vec3(0.0); vec3 m2 = vec3(0.0); vec3 m3 = vec3(0.0);
     vec3 s0 = vec3(0.0); vec3 s1 = vec3(0.0); vec3 s2 = vec3(0.0); vec3 s3 = vec3(0.0);
     vec3 c;

	 	 vec2 q;
	vec2 r;
	vec2 c3 = 1000.0*gl_FragCoord.xy/ uResolution.xy;
	float f = pattern(c3*0.01,q,r);
	vec3 col = mix(color1,color2,clamp((f*f)*4.0,0.0,1.0));
	col = color2;
	col = mix(col,color3,clamp(length(q),0.0,1.0));
	col = mix(col,color4,clamp(length(r.x),0.0,1.0));
	
	vec3 col2 = (0.2*f*f*f+0.6*f*f+0.5*f)*col;
	vec2 delta =  col2.xy * 0.025;
	 
	const vec3 lumi = vec3(0.2126, 0.7152, 0.0722);
	
	vec3 hc =sample2(-1,-1,delta,gl_FragCoord.xy) *  1.0 + sample2( 0,-1,delta,gl_FragCoord.xy) *  2.0
		 	+sample2( 1,-1,delta,gl_FragCoord.xy) *  1.0 + sample2(-1, 1,delta,gl_FragCoord.xy) * -1.0
		 	+sample2( 0, 1,delta,gl_FragCoord.xy) * -2.0 + sample2( 1, 1,delta,gl_FragCoord.xy) * -1.0;
		
	vec3 vc =sample2(-1,-1,delta,gl_FragCoord.xy) *  1.0 + sample2(-1, 0,delta,gl_FragCoord.xy) *  2.0
		 	+sample2(-1, 1,delta,gl_FragCoord.xy) *  1.0 + sample2( 1,-1,delta,gl_FragCoord.xy) * -1.0
		 	+sample2( 1, 0,delta,gl_FragCoord.xy) * -2.0 + sample2( 1, 1,delta,gl_FragCoord.xy) * -1.0;
	
	vec3 c2 = sample2(0, 0,delta,gl_FragCoord.xy);
	
	c2 -= pow(c2, vec3(0.2126, 0.7152, 0.0722)) * pow(dot(lumi, vc*vc + hc*hc), 0.5);
	

	uv = uv + delta; 
	 
     for (int j = -radius; j <= 0; ++j)  {
         for (int i = -radius; i <= 0; ++i)  {
             c = texture(uTexture, uv + vec2(i,j) * src_size).rgb;
             m0 += c;
             s0 += c * c;
         }
     }

     for (int j = -radius; j <= 0; ++j)  {
         for (int i = 0; i <= radius; ++i)  {
             c = texture(uTexture, uv + vec2(i,j) * src_size).rgb;
             m1 += c;
             s1 += c * c;
         }
     }

     for (int j = 0; j <= radius; ++j)  {
         for (int i = 0; i <= radius; ++i)  {
             c = texture(uTexture, uv + vec2(i,j) * src_size).rgb;
             m2 += c;
             s2 += c * c;
         }
     }

     for (int j = 0; j <= radius; ++j)  {
         for (int i = -radius; i <= 0; ++i)  {
             c = texture(uTexture, uv + vec2(i,j) * src_size).rgb;
             m3 += c;
             s3 += c * c;
         }
     }


	 vec4 result;
     float min_sigma2 = 1e+2;
     m0 /= n;
     s0 = abs(s0 / n - m0 * m0);

     float sigma2 = s0.r + s0.g + s0.b;
     if (sigma2 < min_sigma2) {
         min_sigma2 = sigma2;
         result = vec4(m0, 1.0);
     }

     m1 /= n;
     s1 = abs(s1 / n - m1 * m1);

     sigma2 = s1.r + s1.g + s1.b;
     if (sigma2 < min_sigma2) {
         min_sigma2 = sigma2;
         result = vec4(m1, 1.0);
     }

     m2 /= n;
     s2 = abs(s2 / n - m2 * m2);

     sigma2 = s2.r + s2.g + s2.b;
     if (sigma2 < min_sigma2) {
         min_sigma2 = sigma2;
         result = vec4(m2, 1.0);
     }

     m3 /= n;
     s3 = abs(s3 / n - m3 * m3);

     sigma2 = s3.r + s3.g + s3.b;
     if (sigma2 < min_sigma2) {
         min_sigma2 = sigma2;
         result = vec4(m3, 1.0);
     }
	

	vec4 res2 = vec4(overlay(screen( result.rgb,c2.rgb), result.rgb) , 1.0);

	vec3 col3 = texture(uTexture, gl_FragCoord.xy / uResolution.xy + col2.xy * 0.05 ).xyz;
	
	fragColor = vec4(saturation(col3,res2.rgb ),1.0);
 }
]]

local frag5 = [[
uniform sampler2D uTexture;
uniform float uElapsedTime;
uniform vec2 uResolution;
uniform float uAmplitude = 1.0;
in vec2 vTexCoord;
out vec4 fragColor;
void main()
{
    float t = uElapsedTime * 5.0;
    float x = 0.1 * sin(t + 10.0 * t) * sin(2.0 * t) * sin(3.0 * t) * sin(5.0 * t);
    float y = 0.1 * cos(t + 10.0 * t) * cos(2.0 * t) * cos(3.0 * t) * cos(5.0 * t);
    fragColor = texture(uTexture, vTexCoord + vec2(x, y) * vec2(uAmplitude));
}
]]

function load_from_shadertoy(file, name)
	local content = [[
		uniform sampler2D uTexture;
		uniform float uElapsedTime;
		uniform vec2 uResolution;
		in vec2 vTexCoord;
		out vec4 fragColor;
		vec4 iDate = vec4(2016.0, 6.0, 7.0, 1066.0);
		vec4 iMouse = vec4(640.0, 480.0, 0.0, 0.0);
		vec3 iChannelResolution = vec3(1024.0, 1024.0, 4.0);
	]]
	local source_file = io.open(file, "r")
	local start_main = false
	local iter = source_file:lines()
	for line in iter do
		line = string.gsub(line, "iResolution", "uResolution")
		line = string.gsub(line, "iGlobalTime", "uElapsedTime")
		line = string.gsub(line, "iChannel0", "uTexture")	
		local mainline = string.gsub(line, "mainImage%s-%(%s-out%s-vec4%s-fragColor%s-,%s-in%s-vec2%s-fragCoord%s-%)", "main()")
		if mainline ~= line then start_main = true end
		line = mainline
		if start_main then
			line = string.gsub(line, "%sfragCoord.xy", " gl_FragCoord.xy")
			line = string.gsub(line, "%sfragCoord", " gl_FragCoord.xy")
		end
		content = content .. line .. "\n";
	end
	source_file:close()
	post.register(name, content)
end

post.register("tunnel", frag1)
post.register("dot", frag2)
post.register('sphere', frag3, '', true)
post.register('paint', frag4)
post.register('shake', frag5)