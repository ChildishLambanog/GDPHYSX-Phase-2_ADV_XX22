#pragma once
#include "ForceGenerator.h"
#include "MyVector.h"

using namespace P6;

class Bungee : public ForceGenerator
{
	private:
		MyVector anchorPoint; //Point where the particle is anchored
		float springConstant; //Spring constant of the bungee
		float restLength;     //Maximum length of the bungee
	public:
		Bungee(MyVector pos, float _springConst, float _restLen) :
			anchorPoint(pos), springConstant(_springConst), restLength(_restLen) {}

		void UpdateForce(P6Particle* particle, float time) override;
};

