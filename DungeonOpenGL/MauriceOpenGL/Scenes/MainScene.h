#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "../Scene.h"
class Model;

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();

	void Update(float deltaTime);
	void Draw(Camera* camera);
	void Load(); 
	void Unload();

private:
	Model* sponza;
	Shader*  lightingShader;
	Shader* lampShader;
	Shader* skyboxShader;
	unsigned int cubemapTexture;
	glm::vec3 pointLightPositions[4];
};

#endif