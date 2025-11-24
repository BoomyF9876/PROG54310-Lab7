#pragma once

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "StandardIncludes.h"

class Camera
{
public:
	Camera(Resolution& _resolution);

	void Create(Resolution& _resolution, json::JSON& jsonData);
	void LookAt(const glm::vec3& _position, const glm::vec3& _lookAt, const glm::vec3& _up);
	void SetProjection(Resolution _resolution);

	void Rotate();

	glm::mat4 GetProjection() { return projection; }
	glm::vec3 GetPosition() { return position; }
	glm::mat4 GetView() { return view; }
private:
	glm::mat4 projection;
	glm::vec3 position = {};
	glm::mat4 view = {};

	glm::vec3 lookAt = {};
	glm::vec3 rotation = {};
	float angle = 0.0f;
	float rotationRate = 0.0f;
};

#endif

