#include "ShadowMap.h"
#include "glad/glad.h"
#include "Camera.h"
#include "Shader.h"

#include <string>
#include <glm/gtc/matrix_transform.hpp>

ShadowMap::ShadowMap() {
	// 在z轴上分割眼视锥体
	float lambda = 0.9f;
	float ratio = far / near;
	frustums[0].near = near;

	for (int i = 1; i < CSM_MAX_SPLITS; i++) {
		float si = i / float(CSM_MAX_SPLITS);
		float sub_near = lambda * near * std::pow(ratio, si) + (1 - lambda) * (near + si * (far - near));
		float sub_far = sub_near * 1.005f;

		frustums[i].near = sub_near;
		frustums[i - 1].far = sub_far;
	}
	frustums[CSM_MAX_SPLITS - 1].far = far;

	// 创建帧缓冲
	glGenFramebuffers(1, &shadowFBO);

	// 创建多层深度贴图
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glGenTextures(1, &depthMaps);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 
				0, 
				GL_DEPTH_COMPONENT32F, 
				SHADOW_WIDTH, 
				SHADOW_HEIGHT, 
				CSM_MAX_SPLITS, 
				0, 
				GL_DEPTH_COMPONENT, 
				GL_FLOAT, 
				NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ShadowMap::setup(Camera* camera, glm::vec3& lightDir) {
	splitFrunstum(camera);
	generateLightSpaceMatrix(lightDir);
}

/**
 * @brief 传输渲染深度贴图需要的信息
 * @param shader 着色器
*/
void ShadowMap::transmit(Shader* shader) {
	for (int i = 0; i < CSM_MAX_SPLITS; i++) {
		shader->setMat4("lightSpaceMatrix[" + std::to_string(i) + "]", lightSpaceMatrix[i]);
	}
	shader->setMat4("model", glm::mat4(1.0f));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

/**
 * @brief 传输真正渲染时候的信息
 * @param shader 着色器
*/
void ShadowMap::transmitRenderData(Shader* shader) {
	shader->setVec3("farBounds", glm::vec3(farBounds[0], farBounds[1], farBounds[2]));

	for (int i = 0; i < CSM_MAX_SPLITS; i++) {
		std::string str = "directSpaceMatrix[" + std::to_string(i) + "]";
		shader->setMat4(str.c_str(), lightSpaceMatrix[i]);
	}

	glActiveTexture(GL_TEXTURE0 + 2);
	shader->setInt("texture_cascadeMap", 2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps);
}

/**
 * @brief 分割眼视锥体，计算出每个分出来的视锥体的八个点
 * @param camera 
*/
void ShadowMap::splitFrunstum(Camera* camera) {
	glm::vec3& center = camera->Position;
	glm::vec3& view_dir = camera->Front;
	glm::vec3& up = camera->Up;
	glm::vec3& right = camera->Right;

	frustumCenter = center + view_dir * ((far - near) / 2.0f);

	for (int i = 0; i < CSM_MAX_SPLITS; i++) {
		Frustum& frustum = frustums[i];

		// 计算近、远平面的中点
		glm::vec3 sub_near_center = center + view_dir * frustum.near;
		glm::vec3 sub_far_center = center + view_dir * frustum.far;

		// 计算近、远平面的宽、高的一半
		float near_height = std::tan(glm::radians(fov) / 2.0f) * frustum.near;
		float near_width = near_height * aspect;
		float far_height = std::tan(glm::radians(fov) / 2.0f) * frustum.far;
		float far_width = far_height * aspect;

		// 计算近平面的四个点
		frustum.m_points[0] = sub_near_center - up * near_height - right * near_width;
		frustum.m_points[1] = sub_near_center + up * near_height - right * near_width;
		frustum.m_points[2] = sub_near_center + up * near_height + right * near_width;
		frustum.m_points[3] = sub_near_center - up * near_height + right * near_width;

		// 计算远平面的四个点
		frustum.m_points[4] = sub_far_center - up * far_height - right * far_width;
		frustum.m_points[5] = sub_far_center + up * far_height - right * far_width;
		frustum.m_points[6] = sub_far_center + up * far_height + right * far_width;
		frustum.m_points[7] = sub_far_center - up * far_height + right * far_width;
	}
}

/**
 * @brief 生成光空间转换矩阵 和 相机空间中的分片位置
 * @param viewMat 
*/
void ShadowMap::generateLightSpaceMatrix(glm::vec3& lightDir) {
	glm::mat4 viewMat = glm::lookAt(frustumCenter + lightDir, frustumCenter, glm::vec3(0.0, 1.0, 0.0));

	for (int i = 0; i < CSM_MAX_SPLITS; i++) {
		Frustum& frustum = frustums[i];

		glm::vec3 aabb_max(-5000.0f, -5000.0f, 0.0f);
		glm::vec3 aabb_min(5000.0f, 5000.0f, 0.0f);

		// 找分块的视锥体的Z的最大最小值
		glm::vec4 pos = viewMat* glm::vec4(frustum.m_points[0], 1.0f);
		aabb_max.z = pos.z;
		aabb_min.z = pos.z;
		for (int j = 1; j < 8; j++) {
			pos = viewMat * glm::vec4(frustum.m_points[j], 1.0f);
			aabb_max.z = std::max(aabb_max.z, pos.z);
			aabb_min.z = std::min(aabb_min.z, pos.z);
		}
		aabb_max.z += 100.f;

		// 生成一个正交投影矩阵
		glm::mat4 ortho = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -aabb_max.z, -aabb_min.z);
		glm::mat4 pv = ortho * viewMat;

		for (int j = 0; j < 8; j++) {
			pos = pv * glm::vec4(frustum.m_points[j], 1.0f);
			pos.x /= pos.w;
			pos.y /= pos.w;

			aabb_max.x = std::max(aabb_max.x, pos.x);
			aabb_min.x = std::min(aabb_min.x, pos.x);
			aabb_max.y = std::max(aabb_max.y, pos.y);
			aabb_min.y = std::min(aabb_min.y, pos.y);
		}
		// 调整投影矩阵
		glm::vec2 scale(2.0f / (aabb_max.x - aabb_min.x), 2.0f / (aabb_max.y - aabb_min.y));
		glm::vec2 offset(-0.5f * (aabb_max.x + aabb_min.x) * scale.x, -0.5f * (aabb_max.y + aabb_min.y) * scale.y);
		glm::mat4 adjust = glm::mat4(1.0f);
		adjust[0][0] = scale.x;
		adjust[1][1] = scale.y;
		adjust[0][3] = offset.x;
		adjust[1][3] = offset.y;
		adjust = glm::transpose(adjust);

		lightSpaceMatrix[i] = (adjust * ortho) * viewMat;
	}

	glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near, far);
	for (int i = 0; i < CSM_MAX_SPLITS; i++) {
		farBounds[i] = 0.5f * ((-frustums[i].far * projection[2][2] + projection[3][2]) / frustums[i].far) + 0.5f;
	}
}