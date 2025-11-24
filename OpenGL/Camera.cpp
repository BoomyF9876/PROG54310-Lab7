#include "Camera.h"
#include "EngineTime.h"

Camera::Camera(Resolution& _resolution)
{
	projection = glm::perspective(
		glm::radians(45.0f),
		(float)_resolution.width / (float)_resolution.height,
		0.1f,
		1000.0f
	);

	view = glm::lookAt(
		glm::vec3(1, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
}

//void Camera::SetProjection(Resolution _resolution) {
//	projection = glm::perspective(glm::radians(fov),
//		(float)_resolution.width / (float)_resolution.height,
//		near_plane,
//		far_plane);
//}

void Camera::LookAt(const glm::vec3& _position, const glm::vec3& _lookAt, const glm::vec3& _up)
{
	position = _position;
    lookAt = _lookAt;
	view = glm::lookAt(_position, _lookAt, _up);
}

void Camera::Create(Resolution& _resolution, json::JSON& jsonData)
{
    float _fov = 45.0f;
    float _near = 0.1f;
    float _far = 1000.0f;
    float _rotationRate = 1.0f;

    glm::vec3 CameraPosition{ 1, 0, 0 };
    glm::vec3 CameraLookAt{ 0, 0, 0 };

    if (jsonData.hasKey("Position"))
    {
        json::JSON& jsonCameraObject = jsonData["Position"];
        if (jsonCameraObject.hasKey("x")) CameraPosition.x = jsonCameraObject["x"].ToFloat();
        if (jsonCameraObject.hasKey("y")) CameraPosition.y = jsonCameraObject["y"].ToFloat();
        if (jsonCameraObject.hasKey("z")) CameraPosition.z = jsonCameraObject["z"].ToFloat();
    }

    if (jsonData.hasKey("LookAt"))
    {
        json::JSON& jsonCameraObject = jsonData["LookAt"];
        if (jsonCameraObject.hasKey("x")) CameraLookAt.x = jsonCameraObject["x"].ToFloat();
        if (jsonCameraObject.hasKey("y")) CameraLookAt.y = jsonCameraObject["y"].ToFloat();
        if (jsonCameraObject.hasKey("z")) CameraLookAt.z = jsonCameraObject["z"].ToFloat();
    }

    if (jsonData.hasKey("fov")) _fov = jsonData["fov"].ToFloat();
    if (jsonData.hasKey("near")) _near = jsonData["near"].ToFloat();
    if (jsonData.hasKey("far")) _far = jsonData["far"].ToFloat();
    if (jsonData.hasKey("rotationRate")) rotationRate = jsonData["rotationRate"].ToFloat();

    projection = glm::perspective(
        glm::radians(_fov),
        (float)_resolution.width / (float)_resolution.height,
        _near,
        _far
    );

    LookAt(CameraPosition, CameraLookAt, glm::vec3(0, 1, 0));
}

void Camera::Rotate()
{
    if (rotationRate != 0.0f)
    {
        float distance = glm::distance(glm::vec2(position.x, position.z), glm::vec2(lookAt.x, lookAt.z));
        
        angle += rotationRate * Time::Instance().DeltaTime();
        position.x = lookAt.x + cos(glm::radians(angle)) * distance;
        position.z = lookAt.z + sin(glm::radians(angle)) * distance;

        view = glm::lookAt(position, lookAt, glm::vec3(0, 1, 0));
    }
}