#define STB_IMAGE_IMPLEMENTATION
#include "Model/World.h"
#include "Game/PhysicsWorld.h"
#include "Model/Player.h"

extern float skyboxVertices[108];
extern unsigned int loadCubemap(vector<std::string>);
extern std::vector<std::string> faces;

World::World(const char* world_obj) {
    sunLightDirection = glm::vec3(100.0f, 40.0f, 0.0f);
    shadowMappingShader = new Shader("../../../shader/ShadowMappingVert.vs", "../../../shader/ShadowMappingFrag.frag");
    modelShader = new Shader("../../../shader/ModelVert.vs", "../../../shader/ModelFrag.frag");
    skyboxShader = new Shader("../../../shader/SkyboxVert.vs", "../../../shader/SkyboxFrag.frag");
    model = new Model(world_obj);

    loadDepthMap();
    loadSkybox();
}

/**
 * @brief ������պ�
*/
void World::loadSkybox() {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, &skyboxVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    cubemapTexture = loadCubemap(faces);
}

/**
 * @brief ���������ͼ
*/
void World::loadDepthMap() {

    // ����֡�������
    glGenFramebuffers(1, &depthMapFBO);

    // ����2D����
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // ����֡���岢�������ͼ
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief �����ռ�ת������
*/
void World::calculateLightSpaceMatrix() {
    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 200.0f;
    lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
    glm::vec3 light_pos = camera->Position + glm::vec3(0.0f, 30.0f, 0.0f) + sunLightDirection;
    lightView = glm::lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
}

/**
 * @brief ��Ⱦ�����ͼ
*/
void World::renderDepthMap() {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // �����ռ�ת������
    calculateLightSpaceMatrix();

    // ������ɫ��
    shadowMappingShader->use();
    shadowMappingShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    // ��Ⱦ�����ͼ
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
 * @brief ��������Ⱦ
*/
void World::render() {
    // �����ӿ�
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ʹ�������ͼ��Ⱦ����
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    modelShader->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 600.0f);
    glm::mat4 view = camera->GetViewMatrix();

    // ����Uniform����
    modelShader->setMat4("model", glm::mat4(1.0f));
    modelShader->setMat4("view", view);
    modelShader->setMat4("projection", projection);
    modelShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

    modelShader->setVec3("viewPos", camera->Position);
    modelShader->setVec3("direction_light.direction", sunLightDirection);
    modelShader->setVec3("direction_light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    modelShader->setVec3("direction_light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    modelShader->setVec3("direction_light.specular", glm::vec3(0.7f, 0.7f, 0.7f));

    glActiveTexture(GL_TEXTURE0 + 2);
    modelShader->setInt("texture_shadowMap", 2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // ���Ƴ���
    renderObjects(modelShader);
    player->render(modelShader);

    // ������պ�����
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
    projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 600.0f);
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);

    // ��Ⱦ��պ�
    renderSkybox();
}

/**
 * @brief ��Ⱦ�����е�����ģ��
 * @param shader ��Ⱦʹ�õ���ɫ��
*/
void World::renderObjects(Shader* shader) {
    model->Draw(*shader);
}

/**
 * @brief ��Ⱦ��պ�
*/
void World::renderSkybox() {
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
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
    this->time = time % 1296000;

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(time / 3600.0f), glm::vec3(0.0, 0.0, 1.0));
    sunLightDirection = glm::vec3(rotation * glm::vec4(sunLightDirection, 1.0f));
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
    delete skyboxShader;

    delete model;
}