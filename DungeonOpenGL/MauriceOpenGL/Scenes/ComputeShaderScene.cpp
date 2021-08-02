#include "ComputeShaderScene.h"
#include <glm/gtc/random.hpp>

ComputeShaderScene::ComputeShaderScene()
{
	// the vertex shader simply passes through data
	std::string vertex_source =
		"#version 430\n"
		"layout(location = 0) in vec4 vposition;\n"
		"void main() {\n"
		"   gl_Position = vposition;\n"
		"}\n";

	// the geometry shader creates the billboard quads
	std::string geometry_source =
		"#version 430\n"
		"layout(location = 0) uniform mat4 View;\n"
		"layout(location = 1) uniform mat4 Projection;\n"
		"layout (points) in;\n"
		"layout (triangle_strip, max_vertices = 4) out;\n"
		"out vec2 txcoord;\n"
		"void main() {\n"
		"   vec4 pos = View*gl_in[0].gl_Position;\n"
		"   txcoord = vec2(-1,-1);\n"
		"   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
		"   EmitVertex();\n"
		"   txcoord = vec2( 1,-1);\n"
		"   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
		"   EmitVertex();\n"
		"   txcoord = vec2(-1, 1);\n"
		"   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
		"   EmitVertex();\n"
		"   txcoord = vec2( 1, 1);\n"
		"   gl_Position = Projection*(pos+0.2*vec4(txcoord,0,0));\n"
		"   EmitVertex();\n"
		"}\n";

	// the fragment shader creates a bell like radial color distribution
	std::string fragment_source =
		"#version 330\n"
		"in vec2 txcoord;\n"
		"layout(location = 0) out vec4 FragColor;\n"
		"void main() {\n"
		"   float s = (1/(1+15.*dot(txcoord, txcoord))-1/16.);\n"
		"   FragColor = s*vec4(0.3,0.3,1.0,1);\n"
		"}\n";

	// program and shader handles
	GLuint shader_program, vertex_shader, geometry_shader, fragment_shader; 

	// we need these to properly pass the strings
	const char *source;
	int length;

	// create and compiler vertex shader 
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source = vertex_source.c_str();
	length = vertex_source.size();
	glShaderSource(vertex_shader, 1, &source, &length);
	glCompileShader(vertex_shader);
	if (!check_shader_compile_status(vertex_shader)) 
	{
		std::cout << "ERROR::vertex_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create and compiler geometry shader
	geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	source = geometry_source.c_str();
	length = geometry_source.size();
	glShaderSource(geometry_shader, 1, &source, &length);
	glCompileShader(geometry_shader);
	if (!check_shader_compile_status(geometry_shader)) 
	{
		std::cout << "ERROR::geometry_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create and compiler fragment shader
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source = fragment_source.c_str();
	length = fragment_source.size();
	glShaderSource(fragment_shader, 1, &source, &length);
	glCompileShader(fragment_shader);
	if (!check_shader_compile_status(fragment_shader)) 
	{
		std::cout << "ERROR::fragment_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create program
	shader_program = glCreateProgram();

	// attach shaders
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);

	// link the program and check for errors
	glLinkProgram(shader_program);
	check_program_link_status(shader_program);


	// straight forward implementation of the nbody kernel
	std::string acceleration_source =
		"#version 430\n"
		"layout(local_size_x=256) in;\n"

		"layout(location = 0) uniform float dt;\n"
		"layout(std430, binding=0) buffer pblock { vec4 positions[]; };\n"
		"layout(std430, binding=1) buffer vblock { vec4 velocities[]; };\n"

		"void main() {\n"
		"   int N = int(gl_NumWorkGroups.x*gl_WorkGroupSize.x);\n"
		"   int index = int(gl_GlobalInvocationID);\n"

		"   vec3 position = positions[index].xyz;\n"
		"   vec3 velocity = velocities[index].xyz;\n"
		"   vec3 acceleration = vec3(0,0,0);\n"
		"   for(int i = 0;i<N;++i) {\n"
		"       vec3 other = positions[i].xyz;\n"
		"       vec3 diff = position - other;\n"
		"       float invdist = 1.0/(length(diff)+0.001);\n"
		"       acceleration -= diff*0.1*invdist*invdist*invdist;\n"
		"   }\n"
		"   velocities[index] = vec4(velocity+dt*acceleration,0);\n"
		"}\n";

	// program and shader handles
	GLuint acceleration_program, acceleration_shader;

	// create and compiler vertex shader
	acceleration_shader = glCreateShader(GL_COMPUTE_SHADER);
	source = acceleration_source.c_str();
	length = acceleration_source.size();
	glShaderSource(acceleration_shader, 1, &source, &length);
	glCompileShader(acceleration_shader);
	if (!check_shader_compile_status(acceleration_shader)) {
		std::cout << "ERROR::acceleration_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create program
	acceleration_program = glCreateProgram();

	// attach shaders
	glAttachShader(acceleration_program, acceleration_shader);

	// link the program and check for errors
	glLinkProgram(acceleration_program);
	check_program_link_status(acceleration_program);

	// tiled version of the nbody shader that makes use of shared memory
   // to reduce global memory transactions
	std::string tiled_acceleration_source =
		"#version 430\n"
		"layout(local_size_x=256) in;\n"

		"layout(location = 0) uniform float dt;\n"
		"layout(std430, binding=0) buffer pblock { vec4 positions[]; };\n"
		"layout(std430, binding=1) buffer vblock { vec4 velocities[]; };\n"

		"shared vec4 tmp[gl_WorkGroupSize.x];\n"
		"void main() {\n"
		"   int N = int(gl_NumWorkGroups.x*gl_WorkGroupSize.x);\n"
		"   int index = int(gl_GlobalInvocationID);\n"
		"   vec3 position = positions[index].xyz;\n"
		"   vec3 velocity = velocities[index].xyz;\n"
		"   vec3 acceleration = vec3(0,0,0);\n"
		"   for(int tile = 0;tile<N;tile+=int(gl_WorkGroupSize.x)) {\n"
		"       tmp[gl_LocalInvocationIndex] = positions[tile + int(gl_LocalInvocationIndex)];\n"
		"       groupMemoryBarrier();\n"
		"       barrier();\n"
		"       for(int i = 0;i<gl_WorkGroupSize.x;++i) {\n"
		"           vec3 other = tmp[i].xyz;\n"
		"           vec3 diff = position - other;\n"
		"           float invdist = 1.0/(length(diff)+0.001);\n"
		"           acceleration -= diff*0.1*invdist*invdist*invdist;\n"
		"       }\n"
		"       groupMemoryBarrier();\n"
		"       barrier();\n"
		"   }\n"
		"   velocities[index] = vec4(velocity+dt*acceleration,0);\n"
		"}\n";

	// program and shader handles
	GLuint tiled_acceleration_program, tiled_acceleration_shader;

	// create and compiler vertex shader
	tiled_acceleration_shader = glCreateShader(GL_COMPUTE_SHADER);
	source = tiled_acceleration_source.c_str();
	length = tiled_acceleration_source.size();
	glShaderSource(tiled_acceleration_shader, 1, &source, &length);
	glCompileShader(tiled_acceleration_shader);
	if (!check_shader_compile_status(tiled_acceleration_shader)) {
		std::cout << "ERROR::tiled_acceleration_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create program
	tiled_acceleration_program = glCreateProgram();

	// attach shaders
	glAttachShader(tiled_acceleration_program, tiled_acceleration_shader);

	// link the program and check for errors
	glLinkProgram(tiled_acceleration_program);
	check_program_link_status(tiled_acceleration_program);

	// the integrate shader does the second part of the euler integration
	std::string integrate_source =
		"#version 430\n"
		"layout(local_size_x=256) in;\n"

		"layout(location = 0) uniform float dt;\n"
		"layout(std430, binding=0) buffer pblock { vec4 positions[]; };\n"
		"layout(std430, binding=1) buffer vblock { vec4 velocities[]; };\n"

		"void main() {\n"
		"   int index = int(gl_GlobalInvocationID);\n"
		"   vec4 position = positions[index];\n"
		"   vec4 velocity = velocities[index];\n"
		"   position.xyz += dt*velocity.xyz;\n"
		"   positions[index] = position;\n"
		"}\n";

	// program and shader handles
	GLuint integrate_program, integrate_shader;

	// create and compiler vertex shader
	integrate_shader = glCreateShader(GL_COMPUTE_SHADER);
	source = integrate_source.c_str();
	length = integrate_source.size();
	glShaderSource(integrate_shader, 1, &source, &length);
	glCompileShader(integrate_shader);
	if (!check_shader_compile_status(integrate_shader)) {
		std::cout << "ERROR::integrate_shader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// create program
	integrate_program = glCreateProgram();

	// attach shaders
	glAttachShader(integrate_program, integrate_shader);

	// link the program and check for errors
	glLinkProgram(integrate_program);
	check_program_link_status(integrate_program);

	const int particles = 8 * 1024;

	// randomly place particles in a cube
	std::vector<glm::vec4> positionData(particles);
	std::vector<glm::vec4> velocityData(particles);
	for (int i = 0; i < particles; ++i) {
		// initial position
		positionData[i] = glm::gaussRand(glm::vec4(0, 0, 0, 1), glm::vec4(1, 0.2, 1, 0));
		velocityData[i] = glm::vec4(0);
	}

	// generate positions_vbos and vaos
	GLuint vao, positions_vbo, velocities_vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positions_vbo);
	glGenBuffers(1, &velocities_vbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocities_vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)*particles, &velocityData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);

	// fill with initial data
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*particles, &positionData[0], GL_STATIC_DRAW);

	// set up generic attrib pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));

	const GLuint ssbos[] = { positions_vbo, velocities_vbo };
	//glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 2, ssbos);

	// physical parameters
	float dt = 1.0f / 60.0f;

	// setup uniforms
	glUseProgram(tiled_acceleration_program);
	glUniform1f(0, dt);

	glUseProgram(acceleration_program);
	glUniform1f(0, dt);

	glUseProgram(integrate_program);
	glUniform1f(0, dt);

	GLuint query;
	glGenQueries(1, &query);

	bool tiled = false;
	bool space_down = false;
}

ComputeShaderScene::~ComputeShaderScene()
{

}

void ComputeShaderScene::Update(float deltaTime)
{

}

void ComputeShaderScene::Draw(Camera* camera)
{

}

bool ComputeShaderScene::check_shader_compile_status(GLuint obj) {
	GLint status;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}

bool ComputeShaderScene::check_program_link_status(GLuint obj) {
	GLint status;
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(obj, length, &length, &log[0]);
		std::cerr << &log[0];
		return false;
	}
	return true;
}