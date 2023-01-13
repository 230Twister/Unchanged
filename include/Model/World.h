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
class ShadowMap;

class World{
private:
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;			// 定义深度贴图分辨率	
	const unsigned int SPOT_SHADOW_WIDTH = 1024, SPOT_SHADOW_HEIGHT = 1024;	// 定义深度贴图分辨率	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;					//定义窗口大小

	Model* model;
	Player* player;
	std::vector<Zombie*> zombies;

	unsigned int time;					// 当前时间
	static const int DAY_TIME = 3600;	// 一天的时间
	static const int DAY = DAY_TIME / 2;

	glm::mat4 spotSpaceMatrix;			// 聚光空间转换矩阵
	unsigned int spotDepthMap;			// 聚光深度贴图
	unsigned int spotDepthMapFBO;		// 聚光深度帧缓冲

	Shader* modelShader;				// 模型渲染着色器
	Shader* shadowMappingPointShader;	// 点阴影深度贴图着色器
	Shader* shadowMappingShader;		// 聚光深度贴图着色器
	Shader* cascadedShadowShader;		// 级联阴影着色器
	ShadowMap* shadowMap;				// 级联阴影实现

	// 点阴影部分
	float nearPlane = 0.1f;
	float farPlane = 20.0f;
	std::vector<glm::mat4> shadowTransforms;
	unsigned int depthMapPoint;										// 深度贴图
	unsigned int depthMapFBOPoint;									// 深度帧缓冲
	glm::vec3 pointLightPosition = glm::vec3(41.5, 20.0, 27.0);				// 点光源位置
	
	Camera* camera;		// 摄像机
	SkyBox* skybox;		// 天空
	Water* water;		// 水面

	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void loadDepthMap();
	glm::vec3 getLightPosition();
	glm::vec3 getLightDirection();

public:
	World(const char*);

	// 渲染的相关函数
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
	void setTime(unsigned int);		// 设置时间
	unsigned int getTime();

	~World();
};

#endif // !WORLD
