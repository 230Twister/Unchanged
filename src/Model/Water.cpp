#include "Model/Water.h"
#include <time.h>

Water::Water() {
	// Initialize water surface vertex data
	width = 350; // 35m * 35m, vertex stride 10cm
	vertexCount = 3 * width * width;
	waterVertices = new float[vertexCount];
	indexCount = 2 * 3 * width * width;
	waterIndices = new unsigned int[indexCount];

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < width; ++j) {
			waterVertices[3 * (i * width + j)] = (i - width / 2.0f);
			waterVertices[3 * (i * width + j) + 1] = 6.3f;
			waterVertices[3 * (i * width + j) + 2] = (j - width / 2.0f);
		}
	}
	for (unsigned int i = 0; i < width - 1; ++i) {
		for (unsigned int j = 0; j < width - 1; ++j) {
			waterIndices[6 * (i * width + j)] = (i * width + j);
			waterIndices[6 * (i * width + j) + 1] = (i * width + j + 1);
			waterIndices[6 * (i * width + j) + 2] = ((i + 1) * width + j);
			waterIndices[6 * (i * width + j) + 3] = (i * width + j + 1);
			waterIndices[6 * (i * width + j) + 4] = ((i + 1) * width + j);
			waterIndices[6 * (i * width + j) + 5] = ((i + 1) * width + j + 1);
		}
	}

	waterShader = new Shader("shader/water/WaterVert.vs", "shader/water/WaterFrag.frag");
	loadWater();

	windDir = glm::vec2(0.5f, 0.5f);
	waveCount = 10;
	waves = new GerstnerWave[waveCount];

	/*for (int i = 0; i < waveCount; ++i) {
		using namespace std;
		cout << "Wave " << i << " attributes:" << endl;
		cout << "Amplitude = " << waves[i].A << endl;
		cout << "Direction = (" << waves[i].D.x << ", " << waves[i].D.y << ")" << endl;
		cout << "Wave length = " << waves[i].l << endl;
		cout << "Steepness = " << waves[i].Q << endl;
		cout << "Speed = " << waves[i].s << endl;
	}*/

	genWaves();
}

Water::~Water() {
	delete[] waterVertices;
	delete[] waterIndices;
	delete waterShader;
	delete[] waves;
}

void Water::loadWater() {
	glGenVertexArrays(1, &waterVAO);
	glBindVertexArray(waterVAO);
	glGenBuffers(1, &waterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, waterVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &waterEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexCount, waterIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

/**
 * Automatically generate Gerster wave parameters,
 * pass the data into the given shader
 * and store them in the given buffer.
 * @param shader
 *     specify the shader to pass data to
 * @param windDir
 *     The wave is generated based on given wind direction,
 *     however there will be a random offset to the direction.
 * @param waveCount
 *     The number of waves to generate
 * @param waves
 *     The buffer to store generated waves.
 *     It's caller's responsibility to ensure it has enough space.
 */
void Water::genWaves() {
	srand((unsigned int)time(nullptr));
	int r = rand() % 100;
	windDir = glm::vec2(cosf(r), sinf(r));
	waterShader->use();
	waterShader->setInt("waveCount", waveCount);
	for (int i = 0; i < waveCount; ++i) {
		waves[i].A = randf(0.05f, 0.1f);
		// waves[i].A = randf(0.1f, 0.5f);
		waterShader->setFloat("waves[" + std::to_string(i) + "].A", waves[i].A);

		waves[i].Q = randf(0.3f, 0.4f);
		waterShader->setFloat("waves[" + std::to_string(i) + "].Q", waves[i].Q);

		// The wave direction is determined by wind direction
		// but have a random angle to the wind direction
		float windAngle = acosf((windDir.x / sqrtf(windDir.x * windDir.x + windDir.y * windDir.y)));
		if (windDir.y < 0) windAngle = -windAngle;
		float waveAngle = randf(windAngle - glm::radians(60.0f),
			windAngle + glm::radians(60.0f));
		waves[i].D.x = cos(waveAngle);
		waves[i].D.y = sin(waveAngle);
		waterShader->setVec2("waves[" + std::to_string(i) + "].D", waves[i].D);

		// waves[i].s = randf(0.5f, 1.0f);
		waves[i].s = randf(5.0f, 10.0f);
		waterShader->setFloat("waves[" + std::to_string(i) + "].s", waves[i].s);

		waves[i].l = waves[i].A * randf(500.0f, 800.0f);
		waterShader->setFloat("waves[" + std::to_string(i) + "].l", waves[i].l);
	}
}

void Water::renderWater() {
	glBindVertexArray(waterVAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}