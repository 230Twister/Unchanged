#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Model/SkyBox.h"
#include "Model/Water.h"
#include "Model/Star.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"
#include "ShadowMap.h"
#include <GLFW/glfw3.h>

World::World(const char* world_obj) {
    time = 0;
    shadowMappingShader = new Shader("../../../shader/ShadowMappingVert.vs", "../../../shader/ShadowMappingFrag.frag");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");
    cascadedShadowShader = new Shader("../../../shader/CascadedShadowVert.vs", "../../../shader/ShadowMappingFrag.frag", "../../../shader/CascadedShadowGeo.gs");

    model = new Model(world_obj);
    skybox = new SkyBox();
    water = new Water();
    sun = new Star(glm::vec4(1.0f, 1.0f, 0.7f, 1.0f), 15.0f, 300.0f);
    moon = new Star(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f, 300.0f);
    shadowMap = new ShadowMap();

    loadDepthMap();
}

/**
/*
* @brief ���������ͼ
*/
void World::loadDepthMap() {
    /*****************�۹�������ͼ********************/
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    // ����֡�������
    glGenFramebuffers(1, &spotDepthMapFBO);

    // ����2D����
    glGenTextures(1, &spotDepthMap);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SPOT_SHADOW_WIDTH, SPOT_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
    
    // ƽ�й�
    shadowMap->setup(camera, light_pos);

    // �۹�
    lightProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightView = glm::lookAt(player->getPosition(), player->getPosition() + camera->Front, glm::vec3(0.0, 1.0, 0.0));
    spotSpaceMatrix = lightProjection * lightView;
}

/**
 * @brief ��Ⱦ�����ͼ
*/
void World::renderDepthMap() {
    // �����ռ�ת������
    calculateLightSpaceMatrix();

    /*****************��Ⱦƽ�й�������ͼ*********************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ������ɫ��
    cascadedShadowShader->use();

    shadowMap->transmit(cascadedShadowShader);
    renderObjects(cascadedShadowShader);
    player->render(cascadedShadowShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    /*****************��Ⱦ�۹�������ͼ*********************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // ������ɫ��
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", spotSpaceMatrix);
    shadowMappingShader->setMat4("model", glm::mat4(1.0f));

    // ��Ⱦ�۹�������ͼ
    glViewport(0, 0, SPOT_SHADOW_WIDTH, SPOT_SHADOW_HEIGHT);
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

    shadowMap->transmitRenderData(modelShader);

    glActiveTexture(GL_TEXTURE0 + 3);
    modelShader->setInt("texture_spotShadowMap", 3);
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
    skybox->renderSkybox(time);

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
void World::renderSun() {
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi;
    sun->Render(angle);
}

/**
 * @brief ��Ⱦ����
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderMoon() {
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi - _pi;
    moon->Render(angle);
}


/**
 * @brief ������������
*/
void World::addPlayer(Player* player) {
    this->player = player;
}

/**
 * @brief ��������ӽ�ʬʵ��
 * @param  
*/
void World::addZombie(Zombie* zombie) {
    zombies.push_back(zombie);
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
    delete cascadedShadowShader;
    delete shadowMappingShader;
    delete modelShader;
    delete shadowMap;
    delete model;
    delete sun;
    delete skybox;
    delete water;

    for (Zombie* zombie : zombies) {
        delete zombie;
    }
}