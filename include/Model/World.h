#ifndef WORLD_H
#define WORLD_H

#include "Shader.h"
#include "Camera.h"

class World{
private:
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;	// ���������ͼ�ֱ���	
	const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;			//���崰�ڴ�С

	unsigned int time;
	glm::vec3 sunLightDirection;	// ̫���ⷽ������
	glm::mat4 lightSpaceMatrix;		// ��ռ�ת������

	Shader* modelShader;			// ģ����Ⱦ��ɫ��
	Shader* shadowMappingShader;	// �����ͼ��ɫ��

	unsigned int depthMapFBO;		// ���֡����

	Camera* camera;					// �����

	void calculateLightSpaceMatrix();
	void renderObjects(const Shader*);
public:
	World();

	// ��Ⱦ����غ���
	void loadDepthMap();
	void renderDepthMap();
	void render();

	void setTime(unsigned int);		// ����ʱ��

	~World();
};

#endif // !WORLD
