#include "LineRender.h"
void LineRender::Update(MyVector p1, MyVector p2, glm::mat4 projectionMatrix, int camType)
{

	this->P1 = p1;
	//this->P1.x = -this->P1.x;
	this->P2 = p2;
	//this->P2.x = -this->P2.x;
	this->projectionMatrix = projectionMatrix;


}

void LineRender::Draw(GLuint shaderProg)
{
	glUseProgram(shaderProg);

	glm::vec4 d1 = this->projectionMatrix * glm::vec4(
		P1.x, P1.y, P1.z, 1.0f
	);

	glm::vec4 d2 = this->projectionMatrix * glm::vec4(
		P2.x, P2.y, P2.z, 1.0f
	);

	glBegin(GL_LINES);
	glVertex3f(d1.x, d1.y, d1.z);
	glVertex3f(d2.x, d2.y, d2.z);
	glEnd();
}