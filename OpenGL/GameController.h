#pragma once

#ifndef _GAMECONTROLLER_H_
#define _GAMECONTROLLER_H_

#include "StandardIncludes.h"
#include "Shader.h"
#include "Mesh.h"
#include "Font.h"
#include "Camera.h"
#include "TextController.h"

class Skybox;
class PostProcessor;

class GameController
{
public:
	static GameController& GetInstance()
	{
		static GameController instance;
		return instance;
	}

	void Initialize();
	void RunGame();
	void Load();

	Shader* GetShader(const char* shaderName)
	{
		auto itr = shaders.find(shaderName);
		assert(itr != shaders.end());
		return itr->second;
	}

	std::map<std::string, Shader*>::iterator& GetIt() { return it; }
	PostProcessor* GetProcessor() { return postProcessor; }
	std::map<std::string, Shader*>& GetEffectShaders() { return effectShaders; }

	Font* GetFont(const char* fontName)
	{
		auto itr = fonts.find(fontName);
		assert(itr != fonts.end());
		return itr->second;
	}

private:
	std::map<std::string, Shader*> shaders;
	std::map<std::string, Shader*> effectShaders;
	std::map<std::string, Shader*>::iterator it;
	std::map<std::string, Font*> fonts;

	PostProcessor* postProcessor = nullptr;
	Skybox* skybox = nullptr;
	std::list<Mesh*> meshes;
	std::list<Mesh*> lights;

	TextController* textController = nullptr;
	Camera* camera = nullptr;

	int meshCount = 100;
	GLuint vao;

	inline explicit GameController() = default;
	inline ~GameController() = default;
	inline explicit GameController(GameController const&) = delete;
	inline GameController& operator=(GameController const&) = delete;
};

#endif

