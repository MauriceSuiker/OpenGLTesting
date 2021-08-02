#ifndef DUNGEONSCENE_H
#define DUNGEONSCENE_H

#include "../Scene.h"
#include "../BSPTreeDungeon.h"
class Model;

class DungeonScene : public Scene
{
public:
	DungeonScene();
	~DungeonScene();

	void Update(float deltaTime);
	void Draw(Camera* camera); 
	void Load();
	void Unload();

private:
	BSPTreeDungeon _BSPTreeDungeon;
	glm::vec3 bstPositions[30][30];
	Shader*  lightingShader;
	Shader* lampShader;
	glm::vec3 pointLightPositions[4];

	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int lightVAO;

	Model* bed;
};

#endif