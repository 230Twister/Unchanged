#ifndef WORLD_H
#define WORLD_H

#include "Camera.h"
#include "Model.h"
#include "Shader.h"

class SkyBox;
class Water;

class Player;

class World{
private:
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;	// 定义深度贴图分辨率	
	const unsigned int SCR_WIDTH = 1200, SCR_HEIGHT = 800;			//定义窗口大小

	Model* model;
	Player* player;

	Sun* sun;
	Moon* moon;

	unsigned int time;				// 当前时间
	static const int dayTime = 1800; // 一天的时间
	static const int day = dayTime / 2;
	glm::vec3 sunLightDirection;	// 太阳光方向向量
	glm::mat4 lightSpaceMatrix;		// 光空间转换矩阵

	Shader* modelShader;			// 模型渲染着色器
	Shader* shadowMappingShader;	// 深度贴图着色器

	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture;

	unsigned int sunVAO;
	unsigned int sunVBO;
	unsigned int moonVAO;
	unsigned int moonVBO;

	unsigned int depthMap;			// 深度贴图
	unsigned int depthMapFBO;		// 深度帧缓冲

	Camera* camera;					// 摄像机

	SkyBox* skybox;

	Water* water;


	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
	void renderSun(Shader*);
	void renderMoon(Shader*);
	void renderSkybox();
	void loadDepthMap();
	void loadSkybox();
	void loadSun();
	void loadMoon();
public:
	World(const char*);

	// 渲染的相关函数
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
	void setTime(unsigned int);		// 设置时间
	unsigned int getTime();

	~World();
};

#endif // !WORLD
