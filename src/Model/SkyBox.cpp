#include "Model/SkyBox.h"
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

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
	skyboxShader = new Shader("../../../shader/skybox/SkyboxVert.vs", "../../../shader/skybox/SkyboxFrag.frag");
	faces[0][0] = "../../../skybox/morning_right.jpg";
	faces[0][1] = "../../../skybox/morning_left.jpg";
	faces[0][2] = "../../../skybox/morning_top.jpg";
	faces[0][3] = "../../../skybox/bottom.jpg";
	faces[0][4] = "../../../skybox/morning_back.jpg";
	faces[0][5] = "../../../skybox/morning_front.jpg";

	faces[1][0] = "../../../skybox/midday_right.jpg";
	faces[1][1] = "../../../skybox/midday_left.jpg";
	faces[1][2] = "../../../skybox/midday_top.jpg";
	faces[1][3] = "../../../skybox/bottom.jpg";
	faces[1][4] = "../../../skybox/midday_back.jpg";
	faces[1][5] = "../../../skybox/midday_front.jpg";

	faces[2][0] = "../../../skybox/dusk_right.jpg";
	faces[2][1] = "../../../skybox/dusk_left.jpg";
	faces[2][2] = "../../../skybox/dusk_top.jpg";
	faces[2][3] = "../../../skybox/bottom.jpg";
	faces[2][4] = "../../../skybox/dusk_back.jpg";
	faces[2][5] = "../../../skybox/dusk_front.jpg";

	faces[3][0] = "../../../skybox/night_right.jpg";
	faces[3][1] = "../../../skybox/night_left.jpg";
	faces[3][2] = "../../../skybox/night_top.jpg";
	faces[3][3] = "../../../skybox/bottom.jpg";
	faces[3][4] = "../../../skybox/night_back.jpg";
	faces[3][5] = "../../../skybox/night_front.jpg";

	loadSkybox();
	loadCubemap();
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

/**
 * @brief 加载天空盒图片
*/
void SkyBox::loadCubemap() {

	for (int i = 0; i < 4; i++) {
		glGenTextures(1, &cubemapTexture[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture[i]);

		int width, height, nrChannels;
		for (unsigned int j = 0; j < 6; j++) {
			unsigned char* data = stbi_load(faces[i][j].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				std::cout << "Read img success: " << faces[i][j] << std::endl;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else {
				std::cout << "Cubemap texture failed to load at path: " << faces[i][j] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	// 导入噪声图纹理
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	int width, height, nrChannels;
	unsigned char* image = stbi_load("../../../skybox/noise.png", &width, &height, &nrChannels, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);   // 生成纹理
	stbi_image_free(image);

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
	skyboxShader->setInt("skybox", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture[index]);
	glActiveTexture(GL_TEXTURE1);
	skyboxShader->setInt("noisetex", 1);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}