#include "SceneManager.h"
#include "Scene.h"

#include <iostream>


void SceneManager::Update(float deltaTime)
{
	if (currentScene != nullptr)
	{
		currentScene->Update(deltaTime);
	} 
}

void SceneManager::Draw(Camera* camera)
{
	if (currentScene != nullptr)
	{
		currentScene->Draw(camera);
	}
}

void SceneManager::ChangeScene(Scene* scene)
{
	if (currentScene != nullptr)
	{
		currentScene->Unload();
	}
	scene->Load();
	currentScene = scene;
}