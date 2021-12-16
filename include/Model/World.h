#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class SkyBox;
class Water;
class Star;
class Player;
class Zombie;

class World{
private:
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;	// ���������ͼ�ֱ���	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;			//���崰�ڴ�С

	Model* model;
	Player* player;
	std::vector<Zombie*> zombies;

	Star* sun;
	Star* moon;

	unsigned int time;					// ��ǰʱ��
	static const int DAY_TIME = 3600;	// һ���ʱ��
	static const int DAY = DAY_TIME / 2;

	glm::mat4 directSpaceMatrix;		// ƽ�й�ռ�ת������
	glm::mat4 spotSpaceMatrix;			// ƽ�й�ռ�ת������

	Shader* modelShader;				// ģ����Ⱦ��ɫ��
	Shader* shadowMappingShader;		// �����ͼ��ɫ��

	unsigned int directDepthMap;		// ƽ�й������ͼ
	unsigned int directDepthMapFBO;		// ƽ�й����֡����
	unsigned int spotDepthMap;			// �۹������ͼ
	unsigned int spotDepthMapFBO;		// �۹����֡����

	Camera* camera;						// �����

	SkyBox* skybox;
	Water* water;

	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void renderSun();
	void renderMoon();
	void loadDepthMap();

public:
	World(const char*);

	// ��Ⱦ����غ���
	void renderDepthMap();
	void render();

	Model* getBaseModel();
	void addPlayer(Player*);
	void addZombie(Zombie*);

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
