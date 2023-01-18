#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Model/SkyBox.h"
#include "Model/Water.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"
#include "Model/Zombie.h"
#include "ShadowMap.h"
#include <GLFW/glfw3.h>

World::World(const char* world_obj) {
    time = 00;

    // ��ʼ������shader
    shadowMappingShader = new Shader("../../../shader/shadow/ShadowMappingVert.vs", "../../../shader/shadow/ShadowMappingFrag.frag");
    cascadedShadowShader = new Shader("../../../shader/shadow/CascadedShadowVert.vs", "../../../shader/shadow/ShadowMappingFrag.frag", "../../../shader/shadow/CascadedShadowGeo.gs");
    shadowMappingPointShader = new Shader("../../../shader/shadow/ShadowMappingVertPoint.vs", "../../../shader/shadow/ShadowMappingFragPoint.frag", "../../../shader/shadow/ShadowMappingGPoint.gs");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");
    hdrShader = new Shader("../../../shader/HDRVert.vs", "../../../shader/HDRFrag.frag");

    // ����ģ��
    model = new Model(world_obj);
    skybox = new SkyBox();
    water = new Water();
    shadowMap = new ShadowMap();

    loadDepthMap();

    // ����HDR֡����
    glGenFramebuffers(1, &hdrFBO);

    // ������ɫ����
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ������Ȼ���
    glGenRenderbuffers(1, &hdrDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

    // �󶨻���
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    /*****************���Դ�������ͼ********************/
    // ����֡�������
    glGenFramebuffers(1, &depthMapFBOPoint);
    // ����2D����
    glGenTextures(1, &depthMapPoint);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapPoint);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // ����֡���岢�������ͼ
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapPoint, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief �����ռ�ת������
*/
void World::calculateLightSpaceMatrix() {
    glm::mat4 lightProjection, lightView;
    glm::vec3 light_pos = getLightPosition();
    
    // ƽ�й�
    shadowMap->setup(camera, light_pos);

    // �۹�
    lightProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightView = glm::lookAt(player->getPosition(), player->getPosition() + camera->Front, glm::vec3(0.0, 1.0, 0.0));
    spotSpaceMatrix = lightProjection * lightView;

    /*****************���Դ*******************/
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
    // ��ͼ�����ͶӰ������6����ͬ�Ĺ�ռ�任����
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
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
    for (auto z : zombies) {
        z->render(cascadedShadowShader);
    }
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
    for (auto z : zombies) {
        z->render(shadowMappingShader);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    /*****************���Դ*******************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    //������ɫ��
    shadowMappingPointShader->use();
    shadowMappingPointShader->setFloat("far_plane", farPlane);
    shadowMappingPointShader->setVec3("lightPos", pointLightPosition);
    shadowMappingPointShader->setMat4("model", glm::mat4(1.0f));
    for (unsigned int i = 0; i < 6; ++i)
        shadowMappingPointShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    //��Ⱦ�����ͼ
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(shadowMappingPointShader);
    player->render(shadowMappingPointShader);
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

    glm::vec3 ligh_pos = getLightPosition();
    glm::vec3 ligh_dir = getLightDirection();

    // �����ӿ�
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ����Ⱦ������֡������
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
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
    modelShader->setVec3("direction_light.direction", ligh_dir);
    
    if (isDay){
        // ���Լ��������߱仯�������鹫ʽ
        int noon = DAY / 2;
        float a_morning = 0.2f + (3.2f * currentTime) / DAY_TIME; // 1.0
        float d_morning = 0.6f + (3.2f * currentTime) / DAY_TIME; // 1.4
        float s_morning = 0.4f + (4.8f * currentTime) / DAY_TIME; // 1.6
        float a_afternoon = 2.05f - (4.0f * currentTime) / DAY_TIME;
        float d_afternoon = 2.6f - (4.8f * currentTime) / DAY_TIME;
        float s_afternoon = 3.25f - (6.4f * currentTime) / DAY_TIME;
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
        modelShader->setVec3("direction_light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
        modelShader->setVec3("direction_light.diffuse", glm::vec3(0.2f, 0.2f, 0.2f));
        modelShader->setVec3("direction_light.specular", glm::vec3(0.1f, 0.1f, 0.1f));
    }

    modelShader->setVec3("spot_light.position", player->getPosition());
    modelShader->setVec3("spot_light.direction", camera->Front);
    modelShader->setVec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
    if (player->getFlashMode()) {
        modelShader->setVec3("spot_light.diffuse", 0.7f, 0.7f, 0.7f);
        modelShader->setVec3("spot_light.specular", 0.8f, 0.8f, 0.8f);
    }
    else {
        modelShader->setVec3("spot_light.diffuse", 0.0f, 0.0f, 0.0f);
        modelShader->setVec3("spot_light.specular", 0.0f, 0.0f, 0.0f);
    }
    modelShader->setFloat("spot_light.constant", 1.0f);
    modelShader->setFloat("spot_light.linear", 0.027);
    modelShader->setFloat("spot_light.quadratic", 0.0028);
    modelShader->setFloat("spot_light.cutOff", glm::cos(glm::radians(12.5f)));
    modelShader->setFloat("spot_light.outerCutOff", glm::cos(glm::radians(15.0f)));

    modelShader->setVec3("point_light.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
    modelShader->setVec3("point_light.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    modelShader->setVec3("point_light.specular", glm::vec3(0.8f, 0.8f, 0.8f));
    modelShader->setVec3("point_light.position", pointLightPosition);
    modelShader->setFloat("far_plane", farPlane);
    modelShader->setInt("time", time % DAY_TIME);

    shadowMap->transmitRenderData(modelShader);

    glActiveTexture(GL_TEXTURE3);
    modelShader->setInt("texture_spotShadowMap", 3);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);

    glActiveTexture(GL_TEXTURE4);
    modelShader->setInt("texture_shadowMap3", 4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapPoint);

    // ���Ƴ���
    glEnable(GL_CULL_FACE);
    renderObjects(modelShader);
    player->render(modelShader);
    for (auto z:zombies) {
        z->render(modelShader);
    }
    glDisable(GL_CULL_FACE);

    // ������պ�����
    glDepthFunc(GL_LEQUAL);
    skybox->skyboxShader->use();
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 1000.0f);
    skybox->skyboxShader->setMat4("view", view);
    skybox->skyboxShader->setMat4("projection", projection);
    skybox->skyboxShader->setVec3("viewPos", camera->Position);
    skybox->skyboxShader->setVec3("lightPos", ligh_pos);
    skybox->skyboxShader->setInt("time", time);
    // ��Ⱦ��պ�
    skybox->renderSkybox(time);

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
    water->waterShader->setVec3("lightDir", ligh_dir);
    water->waterShader->setInt("waveMapCount", 0);
    // ��Ⱦˮ��
    water->renderWater();

    // �ٽ�����֡�����е���ɫ���徭ת�������
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hdrShader->use();
    glActiveTexture(GL_TEXTURE0);
    hdrShader->setInt("hdrBuffer", 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    hdrShader->setFloat("exposure", 1.0f);
    hdrShader->setBool("dying", player->isDying());
    renderQuad();
}

/**
 * @brief ���һ���������ı���
*/
void World::renderQuad() {
    static GLuint quadVAO = 0;
    static GLuint quadVBO;

    if (quadVAO == 0) {
        float quadVertices[] = {
            // ����        // ��������
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

/**
 * @brief ��Ⱦ�����е�����ģ��
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderObjects(Shader* shader) {
    model->Draw(*shader);
}

/**
 * @brief ��ȡ��ǰƽ�й��Դλ��
 * @return shader ��Ⱦʹ�õ���ɫ��
*/
glm::vec3 World::getLightPosition() {
    return getLightDirection() *= 120;
}

/**
 * @brief ��ȡ��ǰƽ�й��Դ����
 * @return shader ��Ⱦʹ�õ���ɫ��
*/
glm::vec3 World::getLightDirection() {
    constexpr GLfloat PI = glm::pi<GLfloat>();
    float angle = (float)time / (float)DAY_TIME * 2 * PI;
    if (time > DAY) angle -= PI;
    return glm::vec3(cos(angle), sin(angle), 0.0f);
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
    delete shadowMappingPointShader;
    delete modelShader;
    delete hdrShader;

    delete shadowMap;
    delete model;
    delete skybox;
    delete water;

    for (Zombie* zombie : zombies) {
        delete zombie;
    }
}