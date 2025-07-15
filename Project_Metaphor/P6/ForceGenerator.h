#ifndef P6_FORCE_GENERATOR_H
#define P6_FORCE_GENERATOR_H

#include "P6Particle.h"
#include "MyVector.h"

namespace P6 {
	class ForceGenerator
	{
	public:
		virtual void UpdateForce(P6Particle* p, float time)
		{
			p->AddForce(MyVector(0, 0, 0));
		}
	};
}

#endif 