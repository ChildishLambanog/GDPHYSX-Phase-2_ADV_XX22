#pragma once
#include "ForceGenerator.h"
#include "MyVector.h"

using namespace P6;

class FallingNCradle : public ForceGenerator
{
	private:
		MyVector anchorPoint; //Point where the particle is anchored
		float restLength;	  //Maximum length of the chain
	public:
		FallingNCradle(MyVector pos, float _restLen) :
			anchorPoint(pos), restLength(_restLen) {
		}

		void UpdateForce(P6Particle* particle, float time) override;
};

