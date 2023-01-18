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

    // 初始化所有shader
    shadowMappingShader = new Shader("../../../shader/shadow/ShadowMappingVert.vs", "../../../shader/shadow/ShadowMappingFrag.frag");
    cascadedShadowShader = new Shader("../../../shader/shadow/CascadedShadowVert.vs", "../../../shader/shadow/ShadowMappingFrag.frag", "../../../shader/shadow/CascadedShadowGeo.gs");
    shadowMappingPointShader = new Shader("../../../shader/shadow/ShadowMappingVertPoint.vs", "../../../shader/shadow/ShadowMappingFragPoint.frag", "../../../shader/shadow/ShadowMappingGPoint.gs");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");
    hdrShader = new Shader("../../../shader/HDRVert.vs", "../../../shader/HDRFrag.frag");

    // 加载模型
    model = new Model(world_obj);
    skybox = new SkyBox();
    water = new Water();
    shadowMap = new ShadowMap();

    loadDepthMap();

    // 构建HDR帧缓冲
    glGenFramebuffers(1, &hdrFBO);

    // 构建颜色缓冲
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 构建深度缓冲
    glGenRenderbuffers(1, &hdrDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

    // 绑定缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrDepth);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glm::vec3 light_pos = getLightPosition();
    
    // 平行光
    shadowMap->setup(camera, light_pos);

    // 聚光
    lightProjection = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightView = glm::lookAt(player->getPosition(), player->getPosition() + camera->Front, glm::vec3(0.0, 1.0, 0.0));
    spotSpaceMatrix = lightProjection * lightView;

    /*****************点光源*******************/
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
    // 视图矩阵乘投影矩阵获得6个不同的光空间变换矩阵
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
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
    shadowMappingPointShader->use();
    shadowMappingPointShader->setFloat("far_plane", farPlane);
    shadowMappingPointShader->setVec3("lightPos", pointLightPosition);
    shadowMappingPointShader->setMat4("model", glm::mat4(1.0f));
    for (unsigned int i = 0; i < 6; ++i)
        shadowMappingPointShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    //渲染深度贴图
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
 * @brief 完整的渲染
*/
void World::render() {
    // 计算此时时间
    int currentTime = time % DAY_TIME;
    bool isDay = (currentTime < DAY);

    glm::vec3 ligh_pos = getLightPosition();
    glm::vec3 ligh_dir = getLightDirection();

    // 重置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 先渲染到浮点帧缓冲中
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
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
    modelShader->setVec3("direction_light.direction", ligh_dir);
    
    if (isDay){
        // 线性计算白天光线变化——经验公式
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
    skybox->skyboxShader->setVec3("viewPos", camera->Position);
    skybox->skyboxShader->setVec3("lightPos", ligh_pos);
    skybox->skyboxShader->setInt("time", time);
    // 渲染天空盒
    skybox->renderSkybox(time);

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
    water->waterShader->setVec3("lightDir", ligh_dir);
    water->waterShader->setInt("waveMapCount", 0);
    // 渲染水面
    water->renderWater();

    // 再将浮点帧缓冲中的颜色缓冲经转换后输出
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
 * @brief 输出一个满屏的四边形
*/
void World::renderQuad() {
    static GLuint quadVAO = 0;
    static GLuint quadVBO;

    if (quadVAO == 0) {
        float quadVertices[] = {
            // 坐标        // 纹理坐标
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
 * @brief 渲染世界中的所有模型
 * @param shader 渲染使用的着色器
*/
void World::renderObjects(Shader* shader) {
    model->Draw(*shader);
}

/**
 * @brief 获取当前平行光光源位置
 * @return shader 渲染使用的着色器
*/
glm::vec3 World::getLightPosition() {
    return getLightDirection() *= 120;
}

/**
 * @brief 获取当前平行光光源方向
 * @return shader 渲染使用的着色器
*/
glm::vec3 World::getLightDirection() {
    constexpr GLfloat PI = glm::pi<GLfloat>();
    float angle = (float)time / (float)DAY_TIME * 2 * PI;
    if (time > DAY) angle -= PI;
    return glm::vec3(cos(angle), sin(angle), 0.0f);
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