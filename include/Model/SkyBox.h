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
	void loadCubemap();
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture[4];
	static float skyboxVertices[108];
	std::string faces[4][6];
};

#endif // SKYBOX_H
