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
	skyboxShader = new Shader("../../../shader/SkyboxVert.vs", "../../../shader/SkyboxFrag.frag");
	loadSkybox();
	loadCubemap();
}

SkyBox::~SkyBox() {
	delete skyboxShader;
}

/**
 * @brief º”‘ÿÃÏø’∫–
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
 * @brief º”‘ÿÃÏø’∫–Õº∆¨
*/
void SkyBox::loadCubemap() {

	for (int i = 0; i < 4; i++) {
		glGenTextures(1, &cubemapTexture[i]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture[i]);

		int width, height, nrChannels;
		for (unsigned int j = 0; j < 6; j++) {
			unsigned char* data = stbi_load(faces[i][j].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				std::cout << "Read img sucess: " << faces[i][j] << std::endl;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
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
}

/**
 * @brief ‰÷»æÃÏø’∫–
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
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture[index]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}