#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class SkyBox;
class Water;
class Player;
class Zombie;
class ShadowMap;

class World{
private:
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;			// ���������ͼ�ֱ���	
	const unsigned int SPOT_SHADOW_WIDTH = 1024, SPOT_SHADOW_HEIGHT = 1024;	// ���������ͼ�ֱ���	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;					//���崰�ڴ�С

	Model* model;						// ��ͼģ��
	Player* player;						// ���
	std::vector<Zombie*> zombies;		// ��ʬ����

	GLuint hdrFBO;						// HDR����֡����
	GLuint colorBuffer;					// HDR��ɫ����
	GLuint hdrDepth;					// HDR��Ȼ���

	unsigned int time;					// ��ǰʱ��
	static const int DAY_TIME = 3600;	// һ���ʱ��
	static const int DAY = DAY_TIME / 2;

	glm::mat4 spotSpaceMatrix;			// �۹�ռ�ת������
	GLuint spotDepthMap;				// �۹������ͼ
	GLuint spotDepthMapFBO;				// �۹����֡����

	Shader* modelShader;				// ģ����Ⱦ��ɫ��
	Shader* shadowMappingPointShader;	// ����Ӱ�����ͼ��ɫ��
	Shader* shadowMappingShader;		// �۹������ͼ��ɫ��
	Shader* cascadedShadowShader;		// ������Ӱ��ɫ��
	Shader* hdrShader;					// HDR��ɫ��
	ShadowMap* shadowMap;				// ������Ӱʵ��

	// ����Ӱ����
	float nearPlane = 0.1f;
	float farPlane = 20.0f;
	std::vector<glm::mat4> shadowTransforms;
	GLuint depthMapPoint;											// �����ͼ
	GLuint depthMapFBOPoint;										// ���֡����
	glm::vec3 pointLightPosition = glm::vec3(41.5, 20.0, 27.0);		// ���Դλ��
	
	Camera* camera;		// �����
	SkyBox* skybox;		// ���
	Water* water;		// ˮ��

	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void loadDepthMap();
	void renderQuad();
	glm::vec3 getLightPosition();
	glm::vec3 getLightDirection();

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
	Zombie* getZombie(int index) {
		return zombies[index];
	}
	vector<Zombie*> getZombies() {
		return zombies;
	}

	void setCamera(Camera*);
	void setTime(unsigned int);		// ����ʱ��
	unsigned int getTime();

	~World();
};

#endif // !WORLD
