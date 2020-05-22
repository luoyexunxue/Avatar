local frag = [[
uniform float uElapsedTime;
uniform vec2 uResolution;
in vec2 vTexCoord;
out vec4 fragColor;

mat2 rotate2d(float angle){
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}

float variation(vec2 v1, vec2 v2, float strength, float speed) {
	return sin(
        dot(normalize(v1), normalize(v2)) * strength + uElapsedTime * speed
    ) / 100.0;
}

vec3 paintCircle (vec2 uv, vec2 center, float rad, float width) {

    vec2 diff = center-uv;
    float len = length(diff);

    len += variation(diff, vec2(0.0, 1.0), 5.0, 2.0);
    len -= variation(diff, vec2(1.0, 0.0), 5.0, 2.0);

    float circle = smoothstep(rad-width, rad, len) - smoothstep(rad, rad+width, len);
    return vec3(circle);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // 移植需要转换坐标 vTexCoord.xy
	// vec2 uv = fragCoord.xy / iResolution.xy;
	vec2 uv = vTexCoord.xy;
    uv.x *= 1.5;
    uv.x -= 0.25;

    vec3 color;
    float radius = 0.35;
    vec2 center = vec2(0.5);


    //paint color circle
    color = paintCircle(uv, center, radius, 0.1);

    //color with gradient
    vec2 v = rotate2d(uElapsedTime) * uv;
    color *= vec3(v.x, v.y, 0.7-v.y*v.x);

    //paint white circle
    color += paintCircle(uv, center, radius, 0.01);
    float al = 1.;
    // if(color.r + color.g + color.b == 0.){
    //     al = 0.;
    // }

	fragColor = vec4(color, al);
}
void main()
{
    mainImage(fragColor, gl_FragCoord.xy);
}
]]
post.register('colorcircle', frag)