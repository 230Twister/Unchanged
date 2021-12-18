#ifndef SKYBOX_H
#define SKYBOX_H
#include "Shader.h"
#include <string>

class SkyBox
{
public:
	Shader* skyboxShader;
	void renderSkybox();
	void renewSkybox(unsigned int time);
	SkyBox();
	~SkyBox();

private:
	void loadSkybox();
	unsigned int loadCubemap(std::string* faces);
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture;
	float* skyboxVertices;
	std::string* faces;
};

#endif // SKYBOX_H
