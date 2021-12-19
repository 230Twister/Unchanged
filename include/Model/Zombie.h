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
	bool dead;
public:
	Zombie(glm::vec3);
	void init();

	void render(Shader*);

	void setHealth(int);
	int getHealth();
	bool isDead();
	void setPosition(glm::vec3);
};

#endif // !ZOMBIE_H
