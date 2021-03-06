#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Model/SkyBox.h"
#include "Model/Water.h"
#include "Model/Star.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"
#include "Model/Zombie.h"
#include "ShadowMap.h"
#include <GLFW/glfw3.h>

World::World(const char* world_obj) {
    time = 1800;

    // 初始化所有shader
    shadowMappingShader = new Shader("../../../shader/ShadowMappingVert.vs", "../../../shader/ShadowMappingFrag.frag");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");
    cascadedShadowShader = new Shader("../../../shader/CascadedShadowVert.vs", "../../../shader/ShadowMappingFrag.frag", "../../../shader/CascadedShadowGeo.gs");
    shadowMappingShaderPoint = new Shader("../../../shader/ShadowMappingVertPoint.vs", "../../../shader/ShadowMappingFragPoint.frag", "../../../shader/ShadowMappingGPoint.gs");

    // 加载模型
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
* @brief 生成深度贴图
*/
void World::loadDepthMap() {
    /*****************聚光的深度贴图********************/
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    // 创建帧缓冲对象
    glGenFramebuffers(1, &spotDepthMapFBO);

    // 创建2D纹理
    glGenTextures(1, &spotDepthMap);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SPOT_SHADOW_WIDTH, SPOT_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 创建帧缓冲并绑定深度贴图
    glBindFramebuffer(GL_FRAMEBUFFER, spotDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /*****************点光源的深度贴图********************/
// 创建帧缓冲对象
    glGenFramebuffers(1, &depthMapFBOPoint);
    // 创建2D纹理
    glGenTextures(1, &depthMapPoint);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapPoint);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 创建帧缓冲并绑定深度贴图
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapPoint, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief 计算光空间转换矩阵
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
    
    // 平行光
    shadowMap->setup(camera, light_pos);

    // 聚光
    lightProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightView = glm::lookAt(player->getPosition(), player->getPosition() + camera->Front, glm::vec3(0.0, 1.0, 0.0));
    spotSpaceMatrix = lightProjection * lightView;

    /*****************点光源*******************/
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
    //视图矩阵乘投影矩阵获得6个不同的光空间变换矩阵
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

/**
 * @brief 渲染深度贴图
*/
void World::renderDepthMap() {
    // 计算光空间转换矩阵
    calculateLightSpaceMatrix();

    /*****************渲染平行光的深度贴图*********************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // 设置着色器
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

    /*****************渲染聚光的深度贴图*********************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // 设置着色器
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", spotSpaceMatrix);
    shadowMappingShader->setMat4("model", glm::mat4(1.0f));

    // 渲染聚光的深度贴图
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

    /*****************点光源*******************/
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    //设置着色器
    shadowMappingShaderPoint->use();
    shadowMappingShaderPoint->setFloat("far_plane", far_plane);
    shadowMappingShaderPoint->setVec3("lightPos", lightPos);
    shadowMappingShaderPoint->setMat4("model", glm::mat4(1.0f));
    for (unsigned int i = 0; i < 6; ++i)
        shadowMappingShaderPoint->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    //渲染深度贴图
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(shadowMappingShaderPoint);
    player->render(shadowMappingShaderPoint);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
}

/**
 * @brief 完整的渲染
*/
void World::render() {
    // 计算此时时间
    int currentTime = time % DAY_TIME;
    bool isDay = (currentTime < DAY);

    // 重置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 使用深度贴图渲染场景
    modelShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    // 传递Uniform数据
    modelShader->setMat4("model", glm::mat4(1.0f));
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);
    modelShader->setMat4("spotSpaceMatrix", spotSpaceMatrix);
    modelShader->setVec3("viewPos", camera->Position);
    modelShader->setBool("dying", player->isDying());
    
    if (isDay){
        modelShader->setVec3("direction_light.direction", sun->GetLightDirection());

        // 线性计算白天光线变化
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
    modelShader->setVec3("point_light.position", lightPos);
    modelShader->setFloat("far_plane", far_plane);
    modelShader->setInt("time", time % DAY_TIME);

    shadowMap->transmitRenderData(modelShader);

    glActiveTexture(GL_TEXTURE0 + 3);
    modelShader->setInt("texture_spotShadowMap", 3);
    glBindTexture(GL_TEXTURE_2D, spotDepthMap);

    glActiveTexture(GL_TEXTURE0 + 4);
    modelShader->setInt("texture_shadowMap3", 4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMapPoint);

    // 绘制场景
    glEnable(GL_CULL_FACE);
    renderObjects(modelShader);
    player->render(modelShader);
    for (auto z:zombies) {
        z->render(modelShader);
    }
    glDisable(GL_CULL_FACE);

    // 传递天空盒数据
    glDepthFunc(GL_LEQUAL);
    skybox->skyboxShader->use();
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 1000.0f);
    skybox->skyboxShader->setMat4("view", view);
    skybox->skyboxShader->setMat4("projection", projection);
    // 渲染天空盒
    skybox->renderSkybox(time);

    // 白天太阳，晚上月亮
    if (isDay){
		sun->shader->use();
		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera->GetViewMatrix();
        sun->shader->setMat4("view", view);
        sun->shader->setMat4("projection", projection);
		// 渲染太阳
		renderSun();
    }
    else{
        moon->shader->use();
        projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        view = camera->GetViewMatrix();
        moon->shader->setMat4("view", view);
        moon->shader->setMat4("projection", projection);
        // 渲染月亮
        renderMoon();
    }

    // 水面
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
    water->waterShader->setVec3("lightDir", sun->GetLightDirection());
    water->waterShader->setInt("waveMapCount", 0);
    // 渲染水面
    water->renderWater();
}

/**
 * @brief 渲染世界中的所有模型
 * @param shader 渲染使用的着色器
*/
void World::renderObjects(Shader* shader) {
    model->Draw(*shader);
}

/**
 * @brief 渲染太阳
 * @param shader 渲染使用的着色器
*/
void World::renderSun() {
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi;
    sun->Render(angle);
}

/**
 * @brief 渲染月亮
 * @param shader 渲染使用的着色器
*/
void World::renderMoon() {
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(DAY_TIME) * 2 * _pi - _pi;
    moon->Render(angle);
}


/**
 * @brief 给世界添加玩家
*/
void World::addPlayer(Player* player) {
    this->player = player;
}

/**
 * @brief 给世界添加僵尸实体
 * @param  
*/
void World::addZombie(Zombie* zombie) {
    zombies.push_back(zombie);
}

void World::setCamera(Camera* camera) {
    this->camera = camera;
}

/**
 * @brief 更改世界时间
 * @param time 时间0-18000
*/
void World::setTime(unsigned int time) {
    this->time = time % DAY_TIME;
}

/**
 * @brief 获取世界时间
 * @return 时间
*/
unsigned int World::getTime() {
    return time;
}

/**
 * @brief 获取世界基础模型
 * @return 模型
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