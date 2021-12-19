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

    float constant;
    float linear;
    float quadratic;
};
uniform PointLight point_light;             // ���Դ
uniform DirectionalLight direction_light;   // ƽ�й�Դ
uniform SpotLight spot_light;               // �۹�
uniform vec3 viewPos;                       // �۲���λ��

out vec4 FragColor;

in vec3 Normal;                     // ������
in vec3 FragPos;                    // Ƭ������
in vec2 TexCoords;                  // ��������
in vec4 FragDirectSpacePos[4];      // Ƭ��λ��ƽ�й�ռ������
in vec4 FragSpotSpacePos;           // Ƭ��λ�ھ۹�ռ������

uniform sampler2D texture_diffuse1;         // ��������ͼ
uniform sampler2D texture_specular1;        // �߹���ͼ
uniform sampler2DArray  texture_cascadeMap; // CSM�����ͼ
uniform sampler2D texture_spotShadowMap;    // �����ͼ
uniform vec3 farBounds;

float CSMshadow() {
    int index = 3;
    if (gl_FragCoord.z < farBounds.x) {
        index = 0;
    }
    else if (gl_FragCoord.z < farBounds.y) {
        index = 1;
    }
    else if (gl_FragCoord.z < farBounds.z) {
        index = 2;
    }

    // ��ȡ��������
    vec3 projCoords = FragDirectSpacePos[index].xyz / FragDirectSpacePos[index].w;
    projCoords = projCoords * 0.5 + 0.5;

    // �ȶԵ�ǰƬ������������ͼ�ϵ����ֵ
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(texture_cascadeMap, 0));
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(texture_cascadeMap, vec3(projCoords.xy + vec2(x, y) * texelSize, index)).r; 
            shadow += currentDepth > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float caculateShadow(vec4 FragLightSpacePos, sampler2D texture_shadowMap) {
    // ��ȡ��������
    vec3 projCoords = FragLightSpacePos.xyz / FragLightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    // �ȶԵ�ǰƬ������������ͼ�ϵ����ֵ
    float closestDepth = texture(texture_shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(texture_shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(texture_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;

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
    vec3 lightDir = normalize(direction_light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = direction_light.diffuse * (diff * model_diffuse);

    // ����߹�
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);
    vec3 specular = direction_light.specular * (spec * model_specular);

    // ������Ӱ
    float shadow = 1.0 - CSMshadow();

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

    // ������Ӱ
    float shadow = (1.0 - caculateShadow(FragSpotSpacePos, texture_spotShadowMap));

    // ����˥��
    float distance    = length(spot_light.position - FragPos);
    float attenuation = 1.0 / (spot_light.constant + spot_light.linear * distance + 
                spot_light.quadratic * (distance * distance));

    return attenuation * shadow * (diffuse * intensity + specular * intensity);
}

void main()
{    
    vec3 model_diffuse = vec3(texture(texture_diffuse1, TexCoords));
    // ������
    vec3 ambient = direction_light.ambient * model_diffuse;

    vec3 result = ambient + getDirectionLight();
    
    FragColor = vec4(result, 1.0f);

}