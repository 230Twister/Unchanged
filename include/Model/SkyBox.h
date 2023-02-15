#ifndef SKYBOX_H
#define SKYBOX_H
#include "Shader.h"
#include <string>

class SkyBox
{
public:
	Shader* skyboxShader;
	void renderSkybox(unsigned int);
	SkyBox();
	~SkyBox();

private:
	void loadSkybox();
	void loadNoiseTexture();
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int noiseTexture;
	static float skyboxVertices[108];
};

#endif // SKYBOX_H
