#include "FallingNCradle.h"

void FallingNCradle::UpdateForce(P6Particle* particle, float time)
{
    MyVector pos = particle->Position;
    MyVector force = pos - anchorPoint;
    float mag = force.magnitude();

    if (mag <= restLength)
    {
        // Particle hasn't reached chain length yet; let gravity do its thing
        return;
    }

    // Once the chain is taut, prevent further movement away from the anchor
    MyVector direction = force.normalize();
    particle->Position = anchorPoint + (direction * restLength);

    // Also reset velocity perpendicular to the rod to avoid energy buildup
    particle->Velocity = particle->Velocity - direction * particle->Velocity.dotProduct(direction);
}
