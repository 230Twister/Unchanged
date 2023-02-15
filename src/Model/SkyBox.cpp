#include "Model/SkyBox.h"
#include "Util/FastNoiseLite.h"
#include <glad/glad.h>
#include <iostream>

float SkyBox::skyboxVertices[108] = {
		-400.0f,  400.0f, -400.0f,
		-400.0f, -400.0f, -400.0f,
		 400.0f, -400.0f, -400.0f,
		 400.0f, -400.0f, -400.0f,
		 400.0f,  400.0f, -400.0f,
		-400.0f,  400.0f, -400.0f,

		-400.0f, -400.0f,  400.0f,
		-400.0f, -400.0f, -400.0f,
		-400.0f,  400.0f, -400.0f,
		-400.0f,  400.0f, -400.0f,
		-400.0f,  400.0f,  400.0f,
		-400.0f, -400.0f,  400.0f,

		 400.0f, -400.0f, -400.0f,
		 400.0f, -400.0f,  400.0f,
		 400.0f,  400.0f,  400.0f,
		 400.0f,  400.0f,  400.0f,
		 400.0f,  400.0f, -400.0f,
		 400.0f, -400.0f, -400.0f,

		-400.0f, -400.0f,  400.0f,
		-400.0f,  400.0f,  400.0f,
		 400.0f,  400.0f,  400.0f,
		 400.0f,  400.0f,  400.0f,
		 400.0f, -400.0f,  400.0f,
		-400.0f, -400.0f,  400.0f,

		-400.0f,  400.0f, -400.0f,
		 400.0f,  400.0f, -400.0f,
		 400.0f,  400.0f,  400.0f,
		 400.0f,  400.0f,  400.0f,
		-400.0f,  400.0f,  400.0f,
		-400.0f,  400.0f, -400.0f,

		-400.0f, -400.0f, -400.0f,
		-400.0f, -400.0f,  400.0f,
		 400.0f, -400.0f, -400.0f,
		 400.0f, -400.0f, -400.0f,
		-400.0f, -400.0f,  400.0f,
		 400.0f, -400.0f,  400.0f
};

SkyBox::SkyBox() {
	skyboxShader = new Shader("shader/skybox/SkyboxVert.vs", "shader/skybox/SkyboxFrag.frag");

	loadSkybox();
	loadNoiseTexture();
}

SkyBox::~SkyBox() {
	delete skyboxShader;
}

/**
 * @brief 加载天空盒
*/
void SkyBox::loadSkybox() {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void generateNoise(FastNoiseLite& noise, float* noiseData, unsigned char* gen_image, int channel) {
	const int NOISE_WIDTH = 512;
	int index = 0;
	for (int y = 0; y < NOISE_WIDTH; y++)
		for (int x = 0; x < NOISE_WIDTH; x++)
			noiseData[index++] = noise.GetNoise((float)x, (float)y);

	float fmax = noiseData[0], fmin = noiseData[0];
	for (int i = 1; i < NOISE_WIDTH * NOISE_WIDTH; i++) {
		fmax = std::max(fmax, noiseData[i]);
		fmin = std::min(fmin, noiseData[i]);
	}
	float scale = 255 / (fmax - fmin);
	for (int i = 0; i < NOISE_WIDTH * NOISE_WIDTH; i++) {
		unsigned char value = (noiseData[i] - fmin) * scale;
		gen_image[i * 3 + channel] = value;
	}
}

/**
 * @brief 导入噪声图纹理
*/
void SkyBox::loadNoiseTexture() {
	srand((unsigned)time(0));

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	noise.SetFrequency(0.09);
	noise.SetSeed(rand());

	const int NOISE_WIDTH = 512;
	float* noiseData = new float[NOISE_WIDTH * NOISE_WIDTH];
	unsigned char* gen_img = new unsigned char[NOISE_WIDTH * NOISE_WIDTH * 3];
	generateNoise(noise, noiseData, gen_img, 0);
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
	noise.SetFrequency(0.03);
	generateNoise(noise, noiseData, gen_img, 1);
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Value);
	generateNoise(noise, noiseData, gen_img, 2);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, NOISE_WIDTH, NOISE_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, gen_img);   // 生成纹理
	delete[] noiseData;
	delete[] gen_img;
}

/**
 * @brief 渲染天空盒
*/
void SkyBox::renderSkybox(unsigned int time) {
	int index;
	if (time % 3600 < 300) {
		index = 0;
	}
	else if (time % 3600 < 1500) {
		index = 1;
	}
	else if (time % 3600 < 1800) {
		index = 2;
	}
	else {
		index = 3;
	}
	glBindVertexArray(skyboxVAO);

	glActiveTexture(GL_TEXTURE0);
	skyboxShader->setInt("noisetex", 0);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}