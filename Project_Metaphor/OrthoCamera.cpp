#include "OrthoCamera.h"

OrthoCamera::OrthoCamera(glm::vec3 pos, float width, float height) : Camera(pos)
{
    float aspect = width / height;
    float orthoHeight = 1000.0f;
    float orthoWidth = orthoHeight * aspect;

    this->projection = glm::ortho(
        -orthoWidth,//left 
        orthoWidth,   //right
        -orthoHeight, //bot
        orthoHeight, //top
        0.1f,  //z near
        200.f //z far
    );
    this->WorldUp = glm::vec3(0.f, 1.0f, 0.f);
    this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::mat4 OrthoCamera::getProjection() {
	return this->projection;
}