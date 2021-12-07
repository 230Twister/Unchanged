#ifndef WORLD_H
#define WORLD_H

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

class Player;

class World{
private:
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;	// ���������ͼ�ֱ���	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;			//���崰�ڴ�С

	Model* model;
	Player* player;

	unsigned int time;
	glm::vec3 sunLightDirection;	// ̫���ⷽ������
	glm::mat4 lightSpaceMatrix;		// ��ռ�ת������

	Shader* modelShader;			// ģ����Ⱦ��ɫ��
	Shader* shadowMappingShader;	// �����ͼ��ɫ��
	Shader* skyboxShader;			// ��պ���ɫ��

	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture;

	unsigned int depthMap;			// �����ͼ
	unsigned int depthMapFBO;		// ���֡����

	Camera* camera;					// �����

	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void renderSkybox();
	void loadDepthMap();
	void loadSkybox();
public:
	World(const char*);

	// ��Ⱦ����غ���
	void renderDepthMap();
	void render();

	Model* getBaseModel();
	void addEntity(Player*);

	Camera* getCamera() {
		return camera;
	}
	Player* getPlayer() {
		return player;
	}

	void setCamera(Camera*);
	void setTime(unsigned int);		// ����ʱ��
	unsigned int getTime();

	~World();
};

#endif // !WORLD
