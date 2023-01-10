#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform sampler2D noisetex;                 // 噪声图
uniform vec3 viewPos;                       // 观察者位置
uniform vec3 lightPos;

#define bottom 55   // 云层底部
#define top 62      // 云层顶部
#define width 100    // 云层 xz 坐标范围
#define baseBright  vec3(1.26,1.25,1.29)    // 基础颜色 -- 亮部
#define baseDark    vec3(0.31,0.31,0.32)    // 基础颜色 -- 暗部
#define lightBright vec3(1.29, 1.17, 1.05)  // 光照颜色 -- 亮部
#define lightDark   vec3(0.7,0.75,0.8)      // 光照颜色 -- 暗部

float getDensity(vec3 pos) {
    // 高度衰减
    float mid = (bottom + top) / 2.0;
    float h = top - bottom;
    float weight = 1.0 - 2.0 * abs(mid - pos.y) / h;
    weight = pow(weight, 0.5);

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
    vec3 step = direction * 0.25;   // 步长
    vec4 colorSum = vec4(0);        // 云的颜色积累
    vec3 point = viewPos;           // 起始点

    // 跳过不是云层的位置，直接到达云层位置进行光线前进
    if (point.y < bottom) {
        point += direction * ((bottom - point.y) / abs(direction.y));
    } else if (point.y > top) {
        point += direction * ((point.y - top) / abs(direction.y));
    }

    // 片段和摄像机之间没有云层
    if (length(TexCoords - viewPos) < length(point - viewPos)) {
        return colorSum;
    }

    for (int i = 0; i < 100; i++) {
        point += step;
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
        density *= 0.5;

        vec3 base = mix(baseBright, baseDark, density) * density;
        vec3 light = mix(lightDark, lightBright, delta);
        vec4 color = vec4(base * light, density);
        colorSum += color * (1.0 - colorSum.a);
    }

    return colorSum;
}

void main()
{    
    FragColor = texture(skybox, TexCoords);
    vec4 cloud = getCloud();

    // 薄雾，远处的云渐渐与天空融合
    float dist = length(viewPos - TexCoords);
    dist = max(dist, 0.0f);
    float FogFactor = 1 - exp(-0.001 * dist);
    cloud = mix(cloud, FragColor, FogFactor);

    FragColor.rgb = FragColor.rgb * (1.0 - cloud.a) + cloud.rgb;
    
}