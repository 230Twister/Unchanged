#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class SkyBox;
class Water;

class World{
private:
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;	// ���������ͼ�ֱ���	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;			//���崰�ڴ�С

	Model* model;

	unsigned int time;
	glm::vec3 sunLightDirection;	// ̫���ⷽ������
	glm::mat4 lightSpaceMatrix;		// ��ռ�ת������

	Shader* modelShader;			// ģ����Ⱦ��ɫ��
	Shader* shadowMappingShader;	// �����ͼ��ɫ��

	unsigned int depthMap;			// �����ͼ
	unsigned int depthMapFBO;		// ���֡����

	Camera* camera;					// �����

	SkyBox* skybox;

	Water* water;


	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void loadDepthMap();
public:
	World(const char*);

	// ��Ⱦ����غ���
	void renderDepthMap();
	void render();

	Model* getBaseModel();

	Camera* getCamera() {
		return camera;
	}

	void setCamera(Camera*);
	void setTime(unsigned int);		// ����ʱ��
	unsigned int getTime();

	~World();
};

#endif // !WORLD
