#ifndef WATER_H
#define WATER_H
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * A Gerstner Wave is defined as: (Parameter is u and v)
 * x = u + Q * A * D.x * cos(w * D.x * u + w * D.y * v + fi * t)
 * y = A * sin(w * D.x * u + w * D.y * v + fi * t);
 * z = v + Q * A * D.y * cos(w * D.x * u + w * D.y * v + fi * t)
 *  Where:
 * u and v are original (x, y) position of a vertex,
 * Q controls the sharpness of wave top, usually between 0 and 1,
 * A is the maximum height of wave,
 * D is the direction of the wave,
 * w = sqrt(2 * pi * g / l), l is wave length, w is frequency, g = 9.8m/s^2,
 * fi = 2 * pi * s / l, where s is speed
 */
struct GerstnerWave
{
	float Q;
	float A;
	glm::vec2 D;
	float l;
	float s;
};

class Water
{
public:
	Water();
	~Water();
	Shader* waterShader;
	void renderWater();

private:
	void loadWater();
	void genWaves();

	unsigned int waterVAO;
	unsigned int waterVBO;
	unsigned int waterEBO;

	int width;
	int vertexCount;
	float* waterVertices;
	int indexCount;
	unsigned int* waterIndices;

	glm::vec2 windDir;
	int waveCount;
	GerstnerWave* waves;

	static inline float randf(float min, float max, int precision = 1000){
		if (min > max) std::swap(min, max);
		float delta = max - min;
		auto i = int(delta * precision);
		return ((float)(rand() % i) / (float)precision) + min;
	}
};

#endif //
