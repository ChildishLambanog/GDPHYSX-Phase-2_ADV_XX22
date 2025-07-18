#include "FallingNCradle.h"

//void FallingNCradle::UpdateForce(P6Particle* particle, float time)
//{
//    MyVector pos = particle->Position;
//    MyVector force = pos - anchorPoint;
//    float mag = force.magnitude();
//
//    if (mag < restLength) {
//        // Still slack, let gravity pull freely
//        return;
//    }
//
//    // When rope/rod is taut, enforce the rest length
//    MyVector direction = force.normalize();
//    particle->Position = anchorPoint + (direction * restLength);
//
//    // Zero out velocity perpendicular to the rod
//    particle->Velocity -= direction * particle->Velocity.dotProduct(direction);
//}

void FallingNCradle::UpdateForce(P6Particle* particle, float time)
{
    //MyVector pos = particle->Position;
    //MyVector force = pos - anchorPoint;
    //float mag = force.magnitude();

    //// Allow particle to naturally fall under gravity
    //if (mag < restLength)
    //{
    //    return; // Do nothing, rod is slack
    //}

    //// Once the particle reaches/exceeds restLength,
    //// "attach" it to a rigid rod
    //MyVector direction = force.normalize();
    //particle->Position = anchorPoint + (direction * restLength);

    //// Remove velocity component away from rod to avoid snapping
    //particle->Velocity -= direction * particle->Velocity.dotProduct(direction);

    MyVector toParticle = particle->Position - anchorPoint;
    float dist = toParticle.magnitude();

    if (dist == 0.0f) return;

    if (dist > restLength)
    {
        // Constrain position to the circle of restLength
        MyVector dir = toParticle * (1.0f/ dist);
        particle->Position = anchorPoint + dir * restLength;

        // Now constrain velocity to be tangential
        MyVector radial = dir * (particle->Velocity.dotProduct(dir));
        particle->Velocity -= radial; // remove radial component
    }
}
