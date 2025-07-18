#pragma once
#include "ForceGenerator.h"
#include "MyVector.h"
#include "ParticleLink.h"
#include "ParticleContact.h"

using namespace P6;

//class FallingNCradle : public ForceGenerator
//{
//	private:
//		MyVector anchorPoint; //Point where the particle is anchored
//		float restLength;	  //Maximum length of the chain
//	public:
//		FallingNCradle(MyVector pos, float _restLen) :
//			anchorPoint(pos), restLength(_restLen) {
//		}
//
//		void UpdateForce(P6Particle* particle, float time) override;
//};

class FallingNCradle : public ParticleLink
{
	public:
		MyVector anchorPoint; // Point where the particle is anchored
		float restLength;     // Maximum length of the cable
		float restitution = 0.0f;
		ParticleContact* GetContact() override;
		float CurrentLength();

		FallingNCradle(MyVector pos, float _restLen) :
			anchorPoint(pos), restLength(_restLen) {

		}

		void SetParticle(P6Particle* particle)
		{
			particles[0] = particle;
		}
};



