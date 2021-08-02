#include "DungeonScene.h"

DungeonScene::DungeonScene()
{
	lightingShader = new Shader("Resources/Shaders/Colors.vs", "Resources/Shaders/Colors.fs");
	lampShader = new Shader("Resources/Shaders/Lamp.vs", "Resources/Shaders/Lamp.fs");

	pointLightPositions[0] = glm::vec3(1.0f, 0.3f, 1.0f);
	pointLightPositions[1] = glm::vec3(28.0f, 0.3f, 1.0f);
	pointLightPositions[2] = glm::vec3(1.0f, 0.3f, 28.0f);
	pointLightPositions[3] = glm::vec3(28.0f, 0.3f, 28.0f);

	diffuseMap = LoadTexture("Resources/container2.png");
	specularMap = LoadTexture("Resources/container2_specular.png");

	lightingShader->use();
	lightingShader->setInt("material.diffuse", 0);
	lightingShader->setInt("material.specular", 1);
}

DungeonScene::~DungeonScene()
{

}

void DungeonScene::Load()
{
	_BSPTreeDungeon.Setup();

	for (int y = 0; y < _BSPTreeDungeon.columns; ++y)
	{
		for (int x = 0; x < _BSPTreeDungeon.rows; ++x)
		{
			if (_BSPTreeDungeon.tiles[x][y] == BSPTreeDungeon::Floor)
			{
				bstPositions[x][y] = glm::vec3(x, 0.0f, y);
			}
			else if (_BSPTreeDungeon.tiles[x][y] == BSPTreeDungeon::Bed)
			{
				bstPositions[x][y] = glm::vec3(x, 0.0f, y);
			}
			else
			{
				bstPositions[x][y] = glm::vec3(x, 0.0f, y);
			}
		}
	}
}

void DungeonScene::Unload()
{

}

void DungeonScene::Update(float deltaTime)
{

}

void DungeonScene::Draw(Camera* camera)
{
	glDisable(GL_CULL_FACE);

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

	glm::mat4 model = glm::mat4(1.0f);
	lightingShader->setMat4("model", model);

	// bind diffuse map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	

	//floor
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(20.0f, -1.0f, 20.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader->setMat4("model", model);
	RenderPlane();

	for (int y = 0; y < _BSPTreeDungeon.rows; ++y)
	{
		for (int x = 0; x < _BSPTreeDungeon.columns; ++x)
		{
			if (_BSPTreeDungeon.tiles[x][y] == BSPTreeDungeon::Wall)
			{
				glm::mat4 model;
				model = glm::translate(model, bstPositions[x][y]);
				model = glm::scale(model, glm::vec3(0.5f));
				lightingShader->setMat4("model", model);
				RenderCube();
			}
			else if (_BSPTreeDungeon.tiles[x][y] == BSPTreeDungeon::Bed)
			{
				model = glm::mat4();
				model = glm::translate(model, bstPositions[x][y]);
				model = glm::scale(model, glm::vec3(0.008f));
				lightingShader->setMat4("model", model);
			}
		}
	}

	// also draw the lamp object(s)
	lampShader->use();
	lampShader->setMat4("projection", projection);
	lampShader->setMat4("view", view);

	for (unsigned int i = 0; i < 4; i++)
	{
		model = glm::mat4();
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		lampShader->setMat4("model", model);
		RenderCube();
	}
}