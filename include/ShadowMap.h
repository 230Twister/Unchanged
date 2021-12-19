#ifndef SHADOW_H
#define SHADOW_H

#define CSM_MAX_SPLITS 4
#include <glm/glm.hpp>

class Camera;
class Shader;

// 眼视锥体
struct Frustum {
	glm::vec3 m_points[8];
	float near;
	float far;
};

// 实现CSM阴影
class ShadowMap {
private:
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	float near = 0.1f;		// 近平面
	float far = 1000.0f;	// 远平面
	float aspect = 1200.0f / 800.0f;
	float fov = 45.2f;		// 视锥角度

	unsigned int shadowFBO;						// 帧缓冲
	unsigned int depthMaps;						// 几个分块的多层深度贴图

	glm::vec3 frustumCenter;
	Frustum frustums[CSM_MAX_SPLITS];			// 几个分块的视锥体
	glm::mat4 lightSpaceMatrix[CSM_MAX_SPLITS];	// 光空间转换矩阵
	float farBounds[CSM_MAX_SPLITS];			// 在光空间中的分片位置
public:
	ShadowMap();

	void setup(Camera*, glm::vec3&);
	void transmit(Shader*);
	void transmitRenderData(Shader*);
	void splitFrunstum(Camera*);
	void generateLightSpaceMatrix(glm::vec3&);
};

#endif // !SHADOW_H
