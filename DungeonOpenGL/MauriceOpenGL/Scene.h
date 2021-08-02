#ifndef SCENE_H
#define SCENE_H

#include "Resources/Shaders/Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>



class Scene
{
public:

	virtual void Update(float deltaTime) = 0;
	virtual void Draw(Camera* camera) = 0;
	virtual void Load()
	{
		//
	}
	virtual void Unload()
	{
		//
	}

	void RenderCube();
	void RenderPlane();
	unsigned int LoadTexture(const char *path);
	unsigned int LoadCubemap(std::vector<std::string> faces);
	void RenderSkybox();

	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

private:
	unsigned int cubeVAO = 0;
	unsigned int planeVAO = 0;
	unsigned int skyboxVAO = 0; 
	unsigned int cubeVBO, planeVBO, skyboxVBO;
};

#endif