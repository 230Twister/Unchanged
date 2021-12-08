#include "Model/SkyBox.h"
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

SkyBox::SkyBox() {
	skyboxVertices = new float[108]{
		-300.0f,  300.0f, -300.0f,
		-300.0f, -300.0f, -300.0f,
		 300.0f, -300.0f, -300.0f,
		 300.0f, -300.0f, -300.0f,
		 300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,

		-300.0f, -300.0f,  300.0f,
		-300.0f, -300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f, -300.0f,
		-300.0f,  300.0f,  300.0f,
		-300.0f, -300.0f,  300.0f,

		 300.0f, -300.0f, -300.0f,
		 300.0f, -300.0f,  300.0f,
		 300.0f,  300.0f,  300.0f,
		 300.0f,  300.0f,  300.0f,
		 300.0f,  300.0f, -300.0f,
		 300.0f, -300.0f, -300.0f,

		-300.0f, -300.0f,  300.0f,
		-300.0f,  300.0f,  300.0f,
		 300.0f,  300.0f,  300.0f,
		 300.0f,  300.0f,  300.0f,
		 300.0f, -300.0f,  300.0f,
		-300.0f, -300.0f,  300.0f,

		-300.0f,  300.0f, -300.0f,
		 300.0f,  300.0f, -300.0f,
		 300.0f,  300.0f,  300.0f,
		 300.0f,  300.0f,  300.0f,
		-300.0f,  300.0f,  300.0f,
		-300.0f,  300.0f, -300.0f,

		-300.0f, -300.0f, -300.0f,
		-300.0f, -300.0f,  300.0f,
		 300.0f, -300.0f, -300.0f,
		 300.0f, -300.0f, -300.0f,
		-300.0f, -300.0f,  300.0f,
		 300.0f, -300.0f,  300.0f
	};
	for (int i = 0; i < 108; i++) std::cout << skyboxVertices[i] << " ";
	faces = new std::string[6] {
		"../../../skybox_img/right.png",
		"../../../skybox_img/left.png",
		"../../../skybox_img/top.png",
		"../../../skybox_img/bottom.png",
		"../../../skybox_img/back.png",
		"../../../skybox_img/front.png"
	};
	skyboxShader = new Shader("../../../shader/SkyboxVert.vs", "../../../shader/SkyboxFrag.frag");
	loadSkybox();
}

SkyBox::~SkyBox() {
	/*delete skyboxVertices;
	delete faces;
	delete skyboxShader;*/
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