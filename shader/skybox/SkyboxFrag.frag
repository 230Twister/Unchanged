#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform sampler2D noisetex;                 // ����ͼ
uniform vec3 viewPos;                       // �۲���λ��
uniform vec3 lightPos;

#define bottom 55   // �Ʋ�ײ�
#define top 62      // �Ʋ㶥��
#define width 100    // �Ʋ� xz ���귶Χ
#define baseBright  vec3(1.26,1.25,1.29)    // ������ɫ -- ����
#define baseDark    vec3(0.31,0.31,0.32)    // ������ɫ -- ����
#define lightBright vec3(1.29, 1.17, 1.05)  // ������ɫ -- ����
#define lightDark   vec3(0.7,0.75,0.8)      // ������ɫ -- ����

float getDensity(vec3 pos) {
    // �߶�˥��
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

// �����������ɫ
vec4 getCloud() {
    vec3 direction = normalize(TexCoords - viewPos);
    vec3 step = direction * 0.25;   // ����
    vec4 colorSum = vec4(0);        // �Ƶ���ɫ����
    vec3 point = viewPos;           // ��ʼ��

    // ���������Ʋ��λ�ã�ֱ�ӵ����Ʋ�λ�ý��й���ǰ��
    if (point.y < bottom) {
        point += direction * ((bottom - point.y) / abs(direction.y));
    } else if (point.y > top) {
        point += direction * ((point.y - top) / abs(direction.y));
    }

    // Ƭ�κ������֮��û���Ʋ�
    if (length(TexCoords - viewPos) < length(point - viewPos)) {
        return colorSum;
    }

    for (int i = 0; i < 100; i++) {
        point += step;
        // �����Ʋ㷶Χ��ֱ������
        if (bottom > point.y || point.y > top 
            || -width > point.x || point.x > width 
            || -width > point.z || point.z > width ) {
            break;
        }
        float density = getDensity(point);                  // ��ȡ��ǰ����Ʋ��ܶ�
        vec3 lightDir = normalize(lightPos - point);        // ��Դ����
        float lightDensity = getDensity(point + lightDir);  // ���Դ���򲽽�һ��Ȼ������Ʋ��ܶ�
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

    // ����Զ�����ƽ���������ں�
    float dist = length(viewPos - TexCoords);
    dist = max(dist, 0.0f);
    float FogFactor = 1 - exp(-0.001 * dist);
    cloud = mix(cloud, FragColor, FogFactor);

    FragColor.rgb = FragColor.rgb * (1.0 - cloud.a) + cloud.rgb;
    
}