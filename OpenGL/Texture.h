#pragma once

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "StandardIncludes.h"

class Texture
{
	int width = -1, height = -1, channels = -1;
	GLuint texture;
public:
	~Texture();

	GLuint GetTexture() { return texture; }

	void LoadTexture(std::string _filename);
};

#endif