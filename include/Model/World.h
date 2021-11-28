#ifndef WORLD_H
#define WORLD_H

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

class World{
private:
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;	// 定义深度贴图分辨率	
	const unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;			//定义窗口大小

	Model* model;

	unsigned int time;
	glm::vec3 sunLightDirection;	// 太阳光方向向量
	glm::mat4 lightSpaceMatrix;		// 光空间转换矩阵

	Shader* modelShader;			// 模型渲染着色器
	Shader* shadowMappingShader;	// 深度贴图着色器
	Shader* skyboxShader;			// 天空盒着色器


	unsigned int depthMap;			// 深度贴图
	unsigned int depthMapFBO;		// 深度帧缓冲

	Camera* camera;					// 摄像机

	void calculateLightSpaceMatrix();
	void renderObjects(Shader*);
public:
	World(const char*);

	// 渲染的相关函数
	void loadDepthMap();
	void renderDepthMap();
	void render();

	Camera* getCamera() {
		return camera;
	}

	void setTime(unsigned int);		// 设置时间

	~World();
};

#endif // !WORLD
