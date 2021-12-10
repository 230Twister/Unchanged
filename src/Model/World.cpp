#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Model/SkyBox.h"
#include "Model/Water.h"
#include "Model/Sun.h"
#include "Model/Moon.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"
#include <GLFW/glfw3.h>

World::World(const char* world_obj) {
    time = 0;
    shadowMappingShader = new Shader("../../../shader/ShadowMappingVert.vs", "../../../shader/ShadowMappingFrag.frag");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");

    model = new Model(world_obj);
    skybox = new SkyBox;
    water = new Water;
    loadDepthMap();
    sun = new Sun;
    moon = new Moon;
}

/**
/*
* @brief 生成深度贴图
*/
void World::loadDepthMap() {

    // 创建帧缓冲对象
    glGenFramebuffers(1, &depthMapFBO);

    // 创建2D纹理
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // 创建帧缓冲并绑定深度贴图
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



/**
 * @brief 计算光空间转换矩阵
*/
void World::calculateLightSpaceMatrix() {
    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 200.0f;
    lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
    glm::vec3 light_pos;
    if (time % dayTime < day)
        light_pos = camera->Position + glm::vec3(0.0f, 30.0f, 0.0f) + sun->GetLightDirection();
    else
        light_pos = camera->Position + glm::vec3(0.0f, 30.0f, 0.0f) + moon->GetLightDirection();
    lightView = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

/**
 * @brief 渲染深度贴图
*/
void World::renderDepthMap() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // 计算光空间转换矩阵
    calculateLightSpaceMatrix();

    // 设置着色器
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // 渲染深度贴图
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderObjects(shadowMappingShader);
    player->render(shadowMappingShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
}

/**
 * @brief 完整的渲染
*/
void World::render() {
    // 计算此时时间
    int currentTime = time % dayTime;
    bool isDay = (currentTime < day);

    // 重置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 使用深度贴图渲染场景
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    modelShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera->GetViewMatrix();
    // 传递Uniform数据
    modelShader->setMat4("model", glm::mat4(1.0f));
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);
    modelShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    modelShader->setVec3("viewPos", camera->Position);
    
    if (isDay){
        modelShader->setVec3("direction_light.direction", sun->GetLightDirection());

        // 线性计算白天光线变化
        int noon = day / 2;
        float a_morning = 0.2f + (2.0f * currentTime) / dayTime;
        float d_morning = 0.6f + (1.2f * currentTime) / dayTime;
        float s_morning = 0.3f + (1.6f * currentTime) / dayTime;
        float a_afternoon = 1.2f - (2.0f * currentTime) / dayTime;
        float d_afternoon = 1.2f - (1.2f * currentTime) / dayTime;
        float s_afternoon = 1.1f - (1.6f * currentTime) / dayTime;
        if (currentTime < noon){
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

    modelShader->setVec3("spot_light.position", camera->Position);
    modelShader->setVec3("spot_light.direction", camera->Front);
    modelShader->setVec3("spot_light.ambient", 0.0f, 0.0f, 0.0f);
    modelShader->setVec3("spot_light.diffuse", 1.0f, 1.0f, 1.0f);
    modelShader->setVec3("spot_light.specular", 1.0f, 1.0f, 1.0f);
    modelShader->setFloat("spot_light.constant", 1.0f);
    modelShader->setFloat("spot_light.linear", 0.09);
    modelShader->setFloat("spot_light.quadratic", 0.032);
    modelShader->setFloat("spot_light.cutOff", glm::cos(glm::radians(12.5f)));
    modelShader->setFloat("spot_light.outerCutOff", glm::cos(glm::radians(15.0f)));

    glActiveTexture(GL_TEXTURE0 + 2);
    modelShader->setInt("texture_shadowMap", 2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // 绘制场景
    renderObjects(modelShader);
    player->render(modelShader);

    // 传递天空盒数据
    glDepthFunc(GL_LEQUAL);
    skybox->skyboxShader->use();
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 1000.0f);
    skybox->skyboxShader->setMat4("view", view);
    skybox->skyboxShader->setMat4("projection", projection);
    // 渲染天空盒
    skybox->renderSkybox();

    // 白天太阳，晚上月亮
    if (isDay){
		sun->sunShader->use();
		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera->GetViewMatrix();
        sun->sunShader->setMat4("view", view);
        sun->sunShader->setMat4("projection", projection);

		// 渲染太阳
		renderSun();
    }
    else{
        moon->moonShader->use();
        projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        view = camera->GetViewMatrix();
        moon->moonShader->setMat4("view", view);
        moon->moonShader->setMat4("projection", projection);

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
    // shader.setVec3("lightDir", glm::vec3(-1.0f, -1.0f, 2.0f));
    water->waterShader->setVec3("lightDir", glm::vec3(100.0f, 40.0f, 0.0f));
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
void World::renderSun()
{
    
    setTime(time + 1);
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(dayTime) * 2 * _pi;
    sun->Render(angle);
}

/**
 * @brief 渲染太阳
 * @param shader 渲染使用的着色器
*/
void World::renderMoon()
{
    setTime(time + 1);
    constexpr GLfloat _pi = glm::pi<GLfloat>();
    float angle = (float)(time) / (float)(dayTime) * 2 * _pi - _pi;
    moon->Render(angle);
}


/**
 * @brief 给世界添加实体
*/
void World::addEntity(Player* player) {
    this->player = player;
}

void World::setCamera(Camera* camera) {
    this->camera = camera;
}

/**
 * @brief 更改世界时间
 * @param time 时间0-18000
*/
void World::setTime(unsigned int time) {
    this->time = time % dayTime;
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
    delete shadowMappingShader;
    delete modelShader;
    delete model;
    delete sun;
    delete skybox;
    delete water;
}