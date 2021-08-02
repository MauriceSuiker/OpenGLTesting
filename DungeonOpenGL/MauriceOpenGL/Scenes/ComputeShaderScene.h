#ifndef COMPUTESHADERSCENE_H
#define COMPUTESHADERSCENE_H

#include "../Scene.h"

class ComputeShaderScene : public Scene
{
public:
	ComputeShaderScene();
	~ComputeShaderScene();

	void Update(float deltaTime);
	void Draw(Camera* camera); 

private:
	bool check_shader_compile_status(GLuint obj);
	bool check_program_link_status(GLuint obj);
};

#endif