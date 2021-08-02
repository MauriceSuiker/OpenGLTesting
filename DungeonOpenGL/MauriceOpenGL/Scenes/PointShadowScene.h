#ifndef POINTSHADOWSCENE_H
#define POINTSHADOWSCENE_H

#include "../Scene.h"

class PointShadowScene : public Scene
{
public:
	PointShadowScene();
	~PointShadowScene();

	void Update(float deltaTime);
	void Draw(Camera* camera);

private: 
	glm::vec3 pointLightPos;
	Shader* pointShader; 
	Shader* depthShader;
	Shader* lampShader;
	unsigned int pointDepthMapFBO;
	unsigned int depthCubemap;
	unsigned int woodTexture; 
	unsigned int lightVAO;

	void RenderScene(const Shader* shader);
};

#endif