#include "FallingNCradle.h"

void FallingNCradle::UpdateForce(P6Particle* particle, float time)
{
    MyVector pos = particle->Position;
    MyVector force = pos - anchorPoint;
    float mag = force.magnitude();

	if (mag < restLength) //If less than restLength just free fall
    {
        return;
    }

    //Chain is taut: clamp position and behave like rod
    MyVector direction = force.normalize();

    //Snap position to restLength from anchor
    particle->Position = anchorPoint + (direction * restLength);

    //Remove any velocity component pointing outward
    float outwardVel = particle->Velocity.dotProduct(direction);
    
    if (outwardVel > 0)
    {
        particle->Velocity -= direction * outwardVel;
    }
}
