#include <vector>
#include <string>
#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

using namespace std;

// ÃÏø’∫–æÿ’Û–≈œ¢
float skyboxVertices[108] = {
	// positions          
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

// º”‘ÿÃÏø’∫–Ã˘Õº
std::vector<std::string> faces {
	"../../../src/Skybox/right.jpg",
	"../../../src/Skybox/left.jpg",
	"../../../src/Skybox/top.jpg",
	"../../../src/Skybox/bottom.jpg",
	"../../../src/Skybox/back.jpg",
	"../../../src/Skybox/front.jpg"
};

// Loads a cubemap texture from 6 individual texture faces
// Order should be:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
unsigned int loadCubemap(vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			std::cout << "good " << faces[i] << std::endl;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
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