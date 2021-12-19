#include "Model/Zombie.h"
#include "Model/Model.h"
#include "Shader.h"

Model* Zombie::model = NULL;

Zombie::Zombie(glm::vec3 pos) {
	position = pos;
	health = 20;
}

void Zombie::init() {
	model = new Model("../../../world_model/player/zb.obj");
}

/**
 * @brief äÖÈ¾½©Ê¬Ä£ÐÍ
 * @param shader 
*/
void Zombie::render(Shader* shader) {
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);

	shader->setMat4("model", mat);

	model->Draw(*shader);
}

void Zombie::setPosition(glm::vec3 pos){
	position = pos;
}
