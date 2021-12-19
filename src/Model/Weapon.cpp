#include "Model\Weapon.h"

Weapon::Weapon()
{
	model = new Model("../../../world_model/weapon/weapon.obj");
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	yaw = 0.0f;
	angle = 0.0f;
	direction = 1;
	length = 0.6f;
	speed = 0.1f;
	range = -70.0f;
}

/**
 * @brief 渲染玩家模型
 * @param shader 使用的着色器
*/
void Weapon::Render(Shader* shader)
{
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::translate(mat, position);
	mat = glm::scale(mat, glm::vec3(0.7f));
	mat = glm::rotate(mat, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::translate(mat, glm::vec3(0.0f, -length / 4, 0.0f));
	mat = glm::rotate(mat, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::translate(mat, glm::vec3(0.0f, length / 4, 0.0f));

	shader->setMat4("model", mat);

	model->Draw(*shader);
	if (direction)
	{
		angle -= speed;
		if (angle <= range)
			direction = 0;
	}
	else
	{
		angle += speed;
		if (angle >= 0.0f)
			direction = 1;
	}
}

void Weapon::SetPosition(glm::vec3 pos)
{
	position = pos;
}

void Weapon::SetYaw(float yaw)
{
	this->yaw = yaw;
}
