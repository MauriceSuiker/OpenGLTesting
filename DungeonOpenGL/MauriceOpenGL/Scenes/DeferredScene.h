#ifndef DEFERREDSCENE_H
#define DEFERREDSCENE_H

#include "../Scene.h"
class Model;

class DeferredScene : public Scene
{
public:
	DeferredScene();
	~DeferredScene();

	void Update(float deltaTime);
	void Draw(Camera* camera); 
	void Load();

private:
	Shader* shaderGeometryPass;
	Shader* shaderLightingPass;
	Shader* shaderLightBox;
	Model* nanosuit;

	unsigned int gPosition, gNormal, gAlbedoSpec;
	unsigned int gBuffer;
	std::vector<glm::vec3> objectPositions;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	void renderQuad();
};

#endif