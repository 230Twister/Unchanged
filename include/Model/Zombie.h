#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "glm/glm.hpp"

class Model;
class Shader;

class Zombie {
private:
	static Model* model;

	glm::vec3 position;
	int health;
public:
	Zombie(glm::vec3);
	void init();

	void render(Shader*);
};

#endif // !ZOMBIE_H
