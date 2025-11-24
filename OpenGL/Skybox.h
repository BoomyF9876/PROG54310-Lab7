#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Mesh.h"

class Skybox
{
public:
	~Skybox();

	void Create(json::JSON& jsonData);
	void Render(glm::mat4 _pv);

private:
	void SetShaderVariables(glm::mat4 _pv);
	void BindAttributes();

	Shader* shader = nullptr;
	Texture* texture = {};
	GLuint vertexBuffer = -1;
	std::vector<GLfloat> vertexData;
};

#endif
