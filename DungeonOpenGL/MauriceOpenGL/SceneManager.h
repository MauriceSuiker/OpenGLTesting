#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

class Scene;
class Camera;

class SceneManager
{
public:
	SceneManager()
	{

	}
	~SceneManager()
	{

	} 

	void Update(float deltaTime);
	void Draw(Camera* camera);

	void ChangeScene(Scene* scene);

private:
	Scene* currentScene;
};

#endif