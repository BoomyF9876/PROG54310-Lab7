#ifndef _POSTPROCESSOR_H_
#define _POSTPROCESSOR_H_

#include "StandardIncludes.h"
#include "Texture.h"

class Shader;

class PostProcessor
{
public:
	~PostProcessor();

	// Methods
	void Create(json::JSON& jsonData);
	void Start();
	void End();

private:
	// Methods
	void CreateVertices();
	void CreateBuffers();
	void BindVertices();

private:
	// Members
	GLuint framebuffer = 0;
	GLuint textureColorbuffer = 0;
	GLuint renderBufferObject = 0;
	GLuint vertexBuffer = 0;
	Shader* shader = nullptr;
};
#endif
