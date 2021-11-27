#version 330 core

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct DirectionalLight {
    vec3 direction;     // ƽ�йⷽ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;      // ��Դλ��
    vec3  direction;    // ��Դ�յķ���
    float cutOff;       // ��Ȧ�Ƕ�
    float outerCutOff;  // ��Ȧ�Ƕ�
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight point_light;             // ���Դ
uniform DirectionalLight direction_light;   // ƽ�й�Դ
uniform SpotLight spot_light;               // �۹�
uniform vec3 viewPos;                       // �۲���λ��

out vec4 FragColor;

in vec3 Normal;                     // ������
in vec3 FragPos;                    // Ƭ������
in vec2 TexCoords;                  // ��������
in vec4 FragLightSpacePos;          // Ƭ��λ�ڹ�ռ������

uniform sampler2D texture_diffuse1;     // ��������ͼ
uniform sampler2D texture_specular1;    // �߹���ͼ
uniform sampler2D texture_shadowMap;    // �����ͼ

float caculateShadow(vec3 lightDir) {
    // ��ȡ��������
    vec3 projCoords = FragLightSpacePos.xyz / FragLightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    // �ȶԵ�ǰƬ������������ͼ�ϵ����ֵ
    float closestDepth = texture(texture_shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

// ������Դ
vec3 getPointLight() {
    vec3 model_diffuse = vec3(texture(texture_diffuse1, TexCoords));
    vec3 model_specular = vec3(texture(texture_specular1, TexCoords));

    // ������
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(point_light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = point_light.diffuse * (diff * model_diffuse);

    // ����߹�
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
    vec3 specular = point_light.specular * (spec * model_specular);

    return diffuse + specular;
}

// ����ƽ�й�
vec3 getDirectionLight() {
    vec3 model_diffuse = vec3(texture(texture_diffuse1, TexCoords));
    vec3 model_specular = vec3(texture(texture_specular1, TexCoords));

    // ������
    vec3 normal = normalize(Normal);
    vec3 lightDir = vec3(0.0f, 1.0f, 1.0f);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = direction_light.diffuse * (diff * model_diffuse);

    // ����߹�
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
    vec3 specular = direction_light.specular * (spec * model_specular);

    // ������Ӱ
    float shadow = (1.0 - caculateShadow(direction_light.direction));

    return shadow * (diffuse + specular);
}

// ����۹�
vec3 getSpotLight() {
    vec3 model_diffuse = vec3(texture(texture_diffuse1, TexCoords));
    vec3 model_specular = vec3(texture(texture_specular1, TexCoords));

    vec3 lightDir = normalize(spot_light.position - FragPos);
    float theta = dot(lightDir, normalize(-spot_light.direction));
    float epsilon = spot_light.cutOff - spot_light.outerCutOff;
    float intensity = clamp((theta - spot_light.outerCutOff) / epsilon, 0.0, 1.0);    
      
    // ������
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = spot_light.diffuse * (diff * model_diffuse);

    // ����߹�
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
    vec3 specular = spot_light.specular * (spec * model_specular);

    return diffuse * intensity + specular * intensity;
}

void main()
{    
    vec3 model_diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // ������
    vec3 ambient = direction_light.ambient * model_diffuse;

    vec3 result = ambient + getPointLight() + getDirectionLight() + getSpotLight();
    FragColor = vec4(result, 1.0f);
}