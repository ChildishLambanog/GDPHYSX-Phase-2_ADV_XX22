#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include"MyVector.h"
using namespace P6;
class LineRender
{
	private:
		MyVector P1, P2;
		glm::mat4 projectionMatrix;

	public:
		LineRender(MyVector p1, MyVector p2, glm::mat4 projectionMatrix) : P1(p1), P2(p2), projectionMatrix(projectionMatrix) {}

		void Update(MyVector p1, MyVector p2, glm::mat4 projectionMatrix, int camType);
		void Draw(GLuint shaderProg);
};


