#include "ParticleLink.h"

float ParticleLink::CurrentLength()
{
	MyVector ret = particles[0]->Position - particles[1]->Position;
	return ret.magnitude();
}
