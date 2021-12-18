#include "Model/SkyBox.h"
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>
SkyBox::SkyBox() {
	skyboxVertices = new float[108]{
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
	faces = new std::string[6];
	skyboxShader = new Shader("../../../shader/SkyboxVert.vs", "../../../shader/SkyboxFrag.frag");
}

SkyBox::~SkyBox() {
	delete [] skyboxVertices;
	delete [] faces;
	delete skyboxShader;
}

void SkyBox::renewSkybox(unsigned int time) {
	if (time % 3600 < 300) {
		faces[0] = "../../../skybox_img/morning_right.jpg";
		faces[1] = "../../../skybox_img/morning_left.jpg";
		faces[2] = "../../../skybox_img/morning_top.jpg";
		faces[3] = "../../../skybox_img/bottom.jpg";
		faces[4] = "../../../skybox_img/morning_back.jpg";
		faces[5] = "../../../skybox_img/morning_front.jpg";
	}
	else if (time % 3600 < 1500) {
		faces[0] = "../../../skybox_img/midday_right.jpg";
		faces[1] = "../../../skybox_img/midday_left.jpg";
		faces[2] = "../../../skybox_img/midday_top.jpg";
		faces[3] = "../../../skybox_img/bottom.jpg";
		faces[4] = "../../../skybox_img/midday_back.jpg";
		faces[5] = "../../../skybox_img/midday_front.jpg";
	}
	else if (time % 3600 < 1800) {
		faces[0] = "../../../skybox_img/dusk_right.jpg";
		faces[1] = "../../../skybox_img/dusk_left.jpg";
		faces[2] = "../../../skybox_img/dusk_top.jpg";
		faces[3] = "../../../skybox_img/bottom.jpg";
		faces[4] = "../../../skybox_img/dusk_back.jpg";
		faces[5] = "../../../skybox_img/dusk_front.jpg";
	}
	else {
		faces[0] = "../../../skybox_img/night_right.jpg";
		faces[1] = "../../../skybox_img/night_left.jpg";
		faces[2] = "../../../skybox_img/night_top.jpg";
		faces[3] = "../../../skybox_img/bottom.jpg";
		faces[4] = "../../../skybox_img/night_back.jpg";
		faces[5] = "../../../skybox_img/night_front.jpg";
	}
	loadSkybox();
}


/**
 * @brief ¼ÓÔØÌì¿ÕºÐ
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

    cubemapTexture = loadCubemap(faces);
}

// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
unsigned int SkyBox::loadCubemap(std::string* faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++){
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data){
			std::cout << "Read img sucess: " << faces[i] << std::endl;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

/**
 * @brief äÖÈ¾Ìì¿ÕºÐ
*/
void SkyBox::renderSkybox() {
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}