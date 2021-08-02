#ifndef TESTSHADOWSCENE_H
#define TESTSHADOWSCENE_H

#include "../Scene.h"

class TestShadowScene : public Scene
{
public:
	TestShadowScene();
	~TestShadowScene();

	void Update(float deltaTime);
	void Draw(Camera* camera);

private:
	glm::vec3 lightPos; 
	Shader* simpleDepthShader;
	Shader* shader;
	Shader* skyboxShader;
	unsigned int depthMapFBO;
	unsigned int diffuseMap;
	unsigned int depthMap;
	unsigned int cubemapTexture;

	float BoxX = 0.0f;
	bool movingLeft = true;

	void RenderScene(const Shader* shader);
};

#endif