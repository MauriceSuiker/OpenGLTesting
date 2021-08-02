#ifndef TESTSCENE_H
#define TESTSCENE_H

#include "../Scene.h"
class Model;

class TestScene :public Scene
{
public:
	TestScene();
	~TestScene();

	void Update(float deltaTime);
	void Draw(Camera* camera);
	void Load();

private: 
	Model* nanosuit;
	Shader* shader;
	unsigned int VBO, VAO;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	unsigned int diffuseMap;
	unsigned int normalMap;

	Shader* TessShader;
	unsigned int displacement;
	float m_dispFactor;
	int num_floats; 

	glm::vec3 lightPos;

	void RenderQuad();
};

#endif