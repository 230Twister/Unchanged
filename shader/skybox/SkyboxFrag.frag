#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform sampler2D noisetex;                 // 噪声图
uniform vec3 viewPos;                       // 观察者位置
uniform vec3 lightPos;                      // 太阳位置
uniform int time;                           // 游戏时间

#define bottom 45   // 云层底部
#define top 52      // 云层顶部
#define width 100    // 云层 xz 坐标范围
#define baseBright  vec3(1.26,1.25,1.29)    // 基础颜色 -- 亮部
#define baseDark    vec3(0.31,0.31,0.32)    // 基础颜色 -- 暗部
#define lightBright vec3(1.29, 1.17, 1.05)  // 光照颜色 -- 亮部
#define lightDark   vec3(0.7,0.75,0.8)      // 光照颜色 -- 暗部
#define PI 3.1415926538

float getDensity(vec3 pos) {
    // 高度衰减
    float mid = (bottom + top) / 2.0;
    float h = top - bottom;
    float weight = 1.0 - 2.0 * abs(mid - pos.y) / h;
    weight = pow(weight, 0.5);

    pos.x += time * 0.05;
    pos.x += 100; pos.z += 100;
    vec2 coord = pos.xz * 0.0025;
    float noise = texture2D(noisetex, coord).x;
	noise += texture2D(noisetex, coord * 3.5).x / 3.5;
	noise += texture2D(noisetex, coord * 12.25).x / 12.25;
	noise += texture2D(noisetex, coord * 42.87).x / 42.87;	
	noise /= 1.4472;
    noise *= weight;

    if (noise < 0.4) return 0;

    return noise;
}

// 计算体积云颜色
vec4 getCloud() {
    vec3 direction = normalize(TexCoords - viewPos);
    vec3 astep = direction * 0.25;      // 步长
    vec4 colorSum = vec4(0);            // 云的颜色积累
    vec3 point = viewPos;               // 起始点

    // 跳过不是云层的位置，直接到达云层位置进行ray-marching
    point += (1.0 - step(bottom, point.y)) * direction * ((bottom - point.y) / abs(direction.y));
    point += step(top, point.y) * direction * ((point.y - top) / abs(direction.y));

    // 片段和摄像机之间没有云层
    if (length(TexCoords - viewPos) < length(point - viewPos)) {
        return colorSum;
    }

    for (int i = 0; i < 50; i++) {
        point += astep;
        // 超出云层范围则直接跳出
        if (bottom > point.y || point.y > top 
            || -width > point.x || point.x > width 
            || -width > point.z || point.z > width ) {
            break;
        }
        float density = getDensity(point);                  // 获取当前点的云层密度
        vec3 lightDir = normalize(lightPos - point);        // 光源方向
        float lightDensity = getDensity(point + lightDir);  // 向光源方向步进一步然后采样云层密度
        float delta = clamp(density - lightDensity, 0.0f, 1.0f);
        density *= 0.7;

        vec3 base = mix(baseBright, baseDark, density) * density;
        vec3 light = mix(lightDark, lightBright, delta);
        vec4 color = vec4(base * light, density);
        
        colorSum += color * (1.0 - colorSum.a);
    }
    colorSum.rgb *= mix(1.0, 0.15, clamp((time - 1800) * 0.01, 0.0, 1.0));
    colorSum.rgb *= mix(0.15, 1.0, clamp(time * 0.01, 0.0, 1.0));
    return colorSum;
}

vec3 mie(float dist, vec3 sunL){
    return max(exp(-pow(dist, 0.25)) * sunL - 0.4, 0.0);
}

float getStarNoise(vec2 pos) {
    float noise = texture2D(noisetex, pos).x;	
	noise *= 0.975;
    return noise;
}

// 经验公式渲染天空
// https://www.shadertoy.com/view/4tVSRt
// https://www.shadertoy.com/view/lsXGWH
vec4 getSky() {
    vec3 pos = normalize(TexCoords);
    float lightAngle = time / 3600.0 * 2 * PI;
    vec3 sunPos = normalize(vec3(cos(lightAngle), sin(lightAngle), 0.0));
    
    float coeiff = 0.25;
    vec3 totalSkyLight = vec3(0.3, 0.5, 1.0);   // 天空的基础颜色
    float sunDistance = acos(dot(pos, sunPos)) * PI;

    float scatterMult = clamp(sunDistance, 0.0, 1.0);
	float sun = clamp(1.0 - smoothstep(0.01, 0.11, scatterMult), 0.0, 1.0);

    float dist = pos.y;
    float circleAng = dot(normalize(vec3(pos.x, 0.0, pos.z)), normalize(vec3(sunPos.x, 0.0, sunPos.z)));
    dist += mix(0.0f, 0.3f, clamp(1.0 - circleAng, 0.0f, 1.0f));
    dist = clamp(dist, 0.03, 0.8);
	dist = (coeiff * mix(scatterMult, 1.0, dist)) / dist;
    
    vec3 mieScatter = mie(sunDistance, vec3(1.1));
	vec3 color = dist * totalSkyLight;
    
    color = max(color, 0.0);
	color = max(mix(pow(color, 1.0 - color),
	color / (2.0 * color + 0.5 - color),
	clamp(sunPos.y * 4.0, 0.4, 1.0)), 0.0)
	+ sun + mieScatter;
	
	color *=  (pow(1.0 - scatterMult, 10.0) * 10.0) + 1.0;
	
    sunPos.y += (1.0 - step(0.0, sunPos.y)) * 7.5 * sunPos.y;
	float underscatter = distance(sunPos.y * 0.5 + 0.5, 1.0);
	color = mix(color, vec3(0.0), clamp(underscatter, 0.0, 1.0));
	
    color /= (2.0 * color + 0.5 - color);
    color.b += mix(0.0f, 0.2f, step(0.0, sunPos.y) * sunPos.y);     // 调得蓝一点

    // 月亮
    lightAngle -= PI;
    vec3 moonPos = normalize(vec3(cos(lightAngle), sin(lightAngle), 0.0));
    float moonDistance = acos(dot(pos, moonPos)) * PI;
    color += (1.0 - step(0.1, moonDistance));

    // 星星
    vec2 coord = TexCoords.xz * 0.005 + TexCoords.y * 0.008;
    float star = pow(getStarNoise(coord), 40.0) * 20.0;
    float r1 = getStarNoise(coord * getStarNoise(vec2(sin(time*0.001))));
	float r2 = getStarNoise(coord * getStarNoise(vec2(cos(time*0.001), sin(time*0.001))));
	float r3 = getStarNoise(coord * getStarNoise(vec2(sin(time*0.005), cos(time*0.005))));
    sunPos.y += step(0.0, sunPos.y) * 7.5 * sunPos.y;
	underscatter = distance(clamp(sunPos.y * 0.5 + 0.5, 0.0, 1.0), 1.0);
    color += mix(vec3(0.0), vec3(star * r1, star * r2, star * r3), clamp(underscatter, 0.0, 1.0));
     
	return vec4(color, 1.0f);	
}

void main()
{   
    FragColor = getSky();

    vec4 cloud = getCloud();

    // 薄雾，远处的云渐渐与天空融合
    float dist = length(viewPos - TexCoords) - abs(TexCoords.y - viewPos.y);
    dist = max(dist, 0.0f) * 0.5;
    float FogFactor = 1 - exp(-0.003 * dist);
    cloud = mix(cloud, FragColor, FogFactor);

    FragColor.rgb = FragColor.rgb * (1.0 - cloud.a) + cloud.rgb;
    
}