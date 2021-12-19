#ifndef SHADOW_H
#define SHADOW_H

#define CSM_MAX_SPLITS 4
#include <glm/glm.hpp>

class Camera;
class Shader;

// ����׶��
struct Frustum {
	glm::vec3 m_points[8];
	float near;
	float far;
};

// ʵ��CSM��Ӱ
class ShadowMap {
private:
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	float near = 0.1f;		// ��ƽ��
	float far = 1000.0f;	// Զƽ��
	float aspect = 1200.0f / 800.0f;
	float fov = 45.2f;		// ��׶�Ƕ�

	unsigned int shadowFBO;						// ֡����
	unsigned int depthMaps;						// �����ֿ�Ķ�������ͼ

	glm::vec3 frustumCenter;
	Frustum frustums[CSM_MAX_SPLITS];			// �����ֿ����׶��
	glm::mat4 lightSpaceMatrix[CSM_MAX_SPLITS];	// ��ռ�ת������
	float farBounds[CSM_MAX_SPLITS];			// �ڹ�ռ��еķ�Ƭλ��
public:
	ShadowMap();

	void setup(Camera*, glm::vec3&);
	void transmit(Shader*);
	void transmitRenderData(Shader*);
	void splitFrunstum(Camera*);
	void generateLightSpaceMatrix(glm::vec3&);
};

#endif // !SHADOW_H
