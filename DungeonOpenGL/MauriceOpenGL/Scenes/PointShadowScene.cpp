#include "PointShadowScene.h"
#include <GLFW/glfw3.h>

PointShadowScene::PointShadowScene()
{
	pointLightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	pointShader = new Shader("Resources/Shaders/PointShader.vs", "Resources/Shaders/PointShader.fs");
	depthShader = new Shader("Resources/Shaders/PointDepth.vs", "Resources/Shaders/PointDepth.fs", "Resources/Shaders/PointDepth.gs");
	lampShader = new Shader("Resources/Shaders/Lamp.vs", "Resources/Shaders/Lamp.fs");

	woodTexture = LoadTexture("Resources/container.jpg");

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glGenFramebuffers(1, &pointDepthMapFBO);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	pointShader->use();
	pointShader->setInt("diffuseTexture", 0);
	pointShader->setInt("depthMap", 1);
}

PointShadowScene::~PointShadowScene()
{
	//
}

void PointShadowScene::Update(float deltaTime)
{
	pointLightPos.z = sin(glfwGetTime() * 0.5) * 3.0;
}

void PointShadowScene::Draw(Camera* camera)
{
	glEnable(GL_CULL_FACE);

	// 0. create depth cubemap transformation matrices
	// -----------------------------------------------
	float near_plane = 1.0f;
	float far_plane = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	// 1. render scene to depth cubemap
	// --------------------------------
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	depthShader->use();
	for (unsigned int i = 0; i < 6; ++i)
		depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	depthShader->setFloat("far_plane", far_plane);
	depthShader->setVec3("lightPos", pointLightPos);
	RenderScene(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 2. render scene as normal 
	// -------------------------
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pointShader->use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	pointShader->setMat4("projection", projection);
	pointShader->setMat4("view", view);
	// set lighting uniforms
	pointShader->setVec3("lightPos", pointLightPos);
	pointShader->setVec3("viewPos", camera->Position);
	pointShader->setInt("shadows", true);
	pointShader->setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	RenderScene(pointShader);

	lampShader->use();
	lampShader->setMat4("projection", projection);
	lampShader->setMat4("view", view);
	glBindVertexArray(lightVAO);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, pointLightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	lampShader->setMat4("model", model);
	RenderCube();
}

void PointShadowScene::RenderScene(const Shader* shader)
{
	// room cube
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(7.0f));
	shader->setMat4("model", model);
	glCullFace(GL_FRONT);
	shader->setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
	RenderCube();
	shader->setInt("reverse_normals", 0); // and of course disable it
	glCullFace(GL_BACK);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	shader->setMat4("model", model);
	RenderCube();
}