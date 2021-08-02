#include "TestShadowScene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

TestShadowScene::TestShadowScene()
{
	lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
	simpleDepthShader = new Shader("Resources/Shaders/DepthShader.vs", "Resources/Shaders/DepthShader.fs");
	shader = new Shader("Resources/Shaders/Shader.vs", "Resources/Shaders/Shader.fs");

	diffuseMap = LoadTexture("Resources/container2.png");

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader->use();
	shader->setInt("diffuseTexture", 0);
	shader->setInt("shadowMap", 1);


	// Skybox things
	skyboxShader = new Shader("Resources/Shaders/Skybox.vs", "Resources/Shaders/Skybox.fs");

	std::vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};
	cubemapTexture = LoadCubemap(faces);

	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
}

TestShadowScene::~TestShadowScene()
{
	//
}

void TestShadowScene::Update(float deltaTime)
{
	if (movingLeft)
	{
		BoxX = BoxX + (1.0f * deltaTime);
		if (BoxX >= 3)
		{
			movingLeft = false;
		}
	}
	else
	{
		BoxX = BoxX - (1.0f * deltaTime);
		if (BoxX <= -3)
		{
			movingLeft = true;
		}
	}
}

void TestShadowScene::Draw(Camera* camera)
{
	glDisable(GL_CULL_FACE);

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	// render scene from light's point of view
	simpleDepthShader->use();
	simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	RenderScene(simpleDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	// set light uniforms
	shader->setVec3("viewPos", camera->Position);
	shader->setVec3("lightPos", lightPos);
	shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderScene(shader);


	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->use();
	view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("projection", projection);
	// skybox cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	RenderSkybox();
	glDepthFunc(GL_LESS); // set depth function back to default
}

void TestShadowScene::RenderScene(const Shader* shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	shader->setMat4("model", model);
	RenderPlane();

	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(BoxX, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMat4("model", model);
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader->setMat4("model", model);
	RenderCube();
}