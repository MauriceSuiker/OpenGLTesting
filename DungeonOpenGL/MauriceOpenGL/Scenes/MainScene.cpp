#include "MainScene.h"
#include "../Model.h"

MainScene::MainScene()
{
	
	lightingShader = new Shader("Resources/Shaders/Colors.vs", "Resources/Shaders/Colors.fs");

	pointLightPositions[0] = glm::vec3(-6.5f, 1.0f, 1.0f);
	pointLightPositions[1] = glm::vec3(1.0f, 0.3f, 3.0f);
	pointLightPositions[2] = glm::vec3(1.0f, 0.3f, 5.0f);
	pointLightPositions[3] = glm::vec3(-7.0f, 0.3f, 5.0f);
	lampShader = new Shader("Resources/Shaders/Lamp.vs", "Resources/Shaders/Lamp.fs");


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

MainScene::~MainScene()
{
	//
}

void MainScene::Load()
{
	sponza = new Model("Resources/sponza/sponza.obj");
}

void MainScene::Unload()
{

}

void MainScene::Update(float deltaTime)
{

}

void MainScene::Draw(Camera* camera)
{
	lightingShader->use();
	lightingShader->setVec3("viewPos", camera->Position);
	lightingShader->setFloat("material.shininess", 32.0f);

	// directional light
	lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	lightingShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	lightingShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	lightingShader->setVec3("pointLights[0].position", pointLightPositions[0]);
	lightingShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[0].constant", 1.0f);
	lightingShader->setFloat("pointLights[0].linear", 0.09);
	lightingShader->setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	lightingShader->setVec3("pointLights[1].position", pointLightPositions[1]);
	lightingShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[1].constant", 1.0f);
	lightingShader->setFloat("pointLights[1].linear", 0.09);
	lightingShader->setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	lightingShader->setVec3("pointLights[2].position", pointLightPositions[2]);
	lightingShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[2].constant", 1.0f);
	lightingShader->setFloat("pointLights[2].linear", 0.09);
	lightingShader->setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	lightingShader->setVec3("pointLights[3].position", pointLightPositions[3]);
	lightingShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("pointLights[3].constant", 1.0f);
	lightingShader->setFloat("pointLights[3].linear", 0.09);
	lightingShader->setFloat("pointLights[3].quadratic", 0.032);
	// spotLight
	lightingShader->setVec3("spotLight.position", camera->Position);
	lightingShader->setVec3("spotLight.direction", camera->Front);
	lightingShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	lightingShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	lightingShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	lightingShader->setFloat("spotLight.constant", 1.0f);
	lightingShader->setFloat("spotLight.linear", 0.09);
	lightingShader->setFloat("spotLight.quadratic", 0.032);
	lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	lightingShader->setMat4("projection", projection);
	lightingShader->setMat4("view", view);

	lightingShader->use();
	lightingShader->setMat4("projection", projection);
	lightingShader->setMat4("view", view);

	glm::mat4 model;

	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 3.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	lightingShader->setMat4("model", model);
	sponza->Draw(lightingShader);


	// also draw the lamp object(s)
	lampShader->use();
	lampShader->setMat4("projection", projection);
	lampShader->setMat4("view", view);

	// we now draw as many light bulbs as we have point lights.
	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.1f)); // Make it a smaller cube
		lampShader->setMat4("model", model);
		RenderCube();
	}

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