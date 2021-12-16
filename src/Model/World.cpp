#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Model/SkyBox.h"
#include "Model/Water.h"
#include "Model/Star.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"
#include <GLFW/glfw3.h>

World::World(const char* world_obj) {
    time = 0;
    shadowMappingShader = new Shader("../../../shader/ShadowMappingVert.vs", "../../../shader/ShadowMappingFrag.frag");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");

    model = new Model(world_obj);
    skybox = new SkyBox();
    water = new Water();
    sun = new Star(glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), 15.0f, 300.0f);
    moon = new Star(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, 300.0f);
    loadDepthMap();
}

/**
/*
* @brief ���������ͼ
*/
void World::loadDepthMap() {
    /*****************ƽ�й�������ͼ********************/
    // ����֡�������
    glGenFramebuffers(1, &directDepthMapFBO);

    // ����2D����
    glGenTextures(1, &directDepthMap);
    glBindTexture(GL_TEXTURE_2D, directDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // ����֡���岢�������ͼ
    glBindFramebuffer(GL_FRAMEBUFFER, directDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*****************�۹�������ͼ********************/
    // ����֡�������
    glGenFramebuffers(1, &spotDepthMapFBO);

    // ����2D����
    glGenTextures(1, &spotDepthMap);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // ����֡���岢�������ͼ
    glBindFramebuffer(GL_FRAMEBUFFER, spotDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief �����ռ�ת������
*/
void World::calculateLightSpaceMatrix() {
    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 500.0f;
    lightProjection = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, near_plane, far_plane);
    glm::vec3 light_pos;
    if (time % DAY_TIME < DAY) {
        glm::vec3 light_dir = sun->GetLightDirection();
        light_dir *= 300;
        light_pos = light_dir;
    }
    else {
        glm::vec3 light_dir = moon->GetLightDirection();
        light_dir *= 300;
        light_pos = light_dir;
    }
    
    lightView = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    directSpaceMatrix = lightProjection * lightView;

    lightProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightView = glm::lookAt(camera->Position, camera->Position + camera->Front, glm::vec3(0.0, 1.0, 0.0));
    spotSpaceMatrix = lightProjection * lightView;
}

/**
 * @brief ��Ⱦ�����ͼ
*/
void World::renderDepthMap() {
    // �����ռ�ת������
    calculateLightSpaceMatrix();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ������ɫ��
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", directSpaceMatrix);

    // ��Ⱦƽ�й�������ͼ
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, directDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(shadowMappingShader);
    player->render(shadowMappingShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ������ɫ��
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", spotSpaceMatrix);

    // ��Ⱦ�۹�������ͼ
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, spotDepthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(shadowMappingShader);
    player->render(shadowMappingShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
}

/**
 * @brief ��������Ⱦ
*/
void World::render() {
    // �����ʱʱ��
    int currentTime = time % DAY_TIME;
    bool isDay = (currentTime < DAY);

    // �����ӿ�
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ʹ�������ͼ��Ⱦ����
    modelShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    // ����Uniform����
    modelShader->setMat4("model", glm::mat4(1.0f));
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);
    modelShader->setMat4("directSpaceMatrix", directSpaceMatrix);
    modelShader->setMat4("spotSpaceMatrix", spotSpaceMatrix);
    modelShader->setVec3("viewPos", camera->Position);
    
    if (isDay){
        modelShader->setVec3("direction_light.direction", sun->GetLightDirection());

        // ���Լ��������߱仯
        int noon = DAY / 2;
        float a_morning = 0.2f + (2.0f * currentTime) / DAY_TIME;
        float d_morning = 0.6f + (1.2f * currentTime) / DAY_TIME;
        float s_morning = 0.3f + (1.6f * currentTime) / DAY_TIME;
        float a_afternoon = 1.2f - (2.0f * currentTime) / DAY_TIME;
        float d_afternoon = 1.2f - (1.2f * currentTime) / DAY_TIME;
        float s_afternoon = 1.1f - (1.6f * currentTime) / DAY_TIME;
        if (currentTime < noon)
        {
            modelShader->setVec3("direction_light.ambient", glm::vec3(a_morning, a_morning, a_morning));
            modelShader->setVec3("direction_light.diffuse", glm::vec3(d_morning, d_morning, d_morning));
            modelShader->setVec3("direction_light.specular", glm::vec3(s_morning, s_morning, s_morning));
        }
        else{
            modelShader->setVec3("direction_light.ambient", glm::vec3(a_afternoon, a_afternoon, a_afternoon));
            modelShader->setVec3("direction_light.diffuse", glm::vec3(d_afternoon, d_afternoon, d_afternoon));
            modelShader->setVec3("direction_light.specular", glm::vec3(s_afternoon, s_afternoon, s_afternoon));
        }
    }
    else{
        modelShader->setVec3("direction_light.direction", moon->GetLightDirection());
        modelShader->setVec3("direction_light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        modelShader->setVec3("direction_light.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
        modelShader->setVec3("direction_light.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    }

    // modelShader->setVec3("spot_light.position", camera->Position);
    modelShader->setVec3("spot_light.position", player->getPosition());
    modelShader->setVec3("spot_light.direction", camera->Front);
    modelShader->setVec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
    modelShader->setVec3("spot_light.diffuse", 0.9f, 0.9f, 0.9f);
    modelShader->setVec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
    modelShader->setFloat("spot_light.constant", 1.0f);
    modelShader->setFloat("spot_light.linear", 0.027);
    modelShader->setFloat("spot_light.quadratic", 0.0028);
    modelShader->setFloat("spot_light.cutOff", glm::cos(glm::radians(12.5f)));
    modelShader->setFloat("spot_light.outerCutOff", glm::cos(glm::radians(15.0f)));

    glActiveTexture(GL_TEXTURE0 + 2);
    modelShader->setInt("texture_shadowMap1", 2);
    glBindTexture(GL_TEXTURE_2D, directDepthMap);

    glActiveTexture(GL_TEXTURE0 + 3);
    modelShader->setInt("texture_shadowMap2", 3);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);

    // ���Ƴ���
    glEnable(GL_CULL_FACE);
    renderObjects(modelShader);
    player->render(modelShader);
    glDisable(GL_CULL_FACE);

    // ������պ�����
    glDepthFunc(GL_LEQUAL);
    skybox->skyboxShader->use();
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 1000.0f);
    skybox->skyboxShader->setMat4("view", view);
    skybox->skyboxShader->setMat4("projection", projection);
    // ��Ⱦ��պ�
    skybox->renderSkybox();

    // ����̫������������
    if (isDay){
		sun->shader->use();
		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera->GetViewMatrix();
        sun->shader->setMat4("view", view);
        sun->shader->setMat4("projection", projection);

		// ��Ⱦ̫��
		renderSun();
    }
    else{
        moon->shader->use();
        projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        view = camera->GetViewMatrix();
        moon->shader->setMat4("view", view);
        moon->shader->setMat4("projection", projection);

        // ��Ⱦ����
        renderMoon();
    }

    // ˮ��
    water->waterShader->use();
    view = camera->GetViewMatrix();
    projection = glm::perspective(glm::radians(camera->Zoom),(float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 1000.0f);
    // Set vertex shader data
    water->waterShader->setMat4("view", view);
    water->waterShader->setMat4("projection", projection);
    water->waterShader->setMat4("model", glm::mat4(1.0f));
    water->waterShader->setFloat("time", (float)glfwGetTime());
    // Set fragment shader data
    water->waterShader->setVec3("viewPos", camera->Position);
    water->waterShader->setVec3("deepWaterColor", glm::vec3(0.1137f, 0.2745f, 0.4392f));
    // shader.setVec3("lightDir", glm::vec3(-1.0f, -1.0f, 2.0f));
    water->waterShader->setVec3("lightDir", sun->GetLightDirection());
    water->waterShader->setInt("waveMapCount", 0);
    // ��Ⱦˮ��
    water->renderWater();
}

/**
 * @brief ��Ⱦ�����е�����ģ��
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderObjects(Shader* shader) {
    model->Draw(*shader);
}

/**
 * @brief ��Ⱦ̫��
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderSun()
{
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi;
    sun->Render(angle);
}

/**
 * @brief ��Ⱦ����
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderMoon()
{
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi - _pi;
    moon->Render(angle);
}


/**
 * @brief ���������ʵ��
*/
void World::addEntity(Player* player) {
    this->player = player;
}

void World::setCamera(Camera* camera) {
    this->camera = camera;
}

/**
 * @brief ��������ʱ��
 * @param time ʱ��0-18000
*/
void World::setTime(unsigned int time) {
    this->time = time % DAY_TIME;
}

/**
 * @brief ��ȡ����ʱ��
 * @return ʱ��
*/
unsigned int World::getTime() {
    return time;
}

/**
 * @brief ��ȡ�������ģ��
 * @return ģ��
*/
Model* World::getBaseModel() {
    return model;
}

World::~World() {
    delete shadowMappingShader;
    delete modelShader;
    delete model;
    delete sun;
    delete skybox;
    delete water;
}