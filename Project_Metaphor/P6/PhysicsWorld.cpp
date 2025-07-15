#include "PhysicsWorld.h"
#include <iostream>

void P6::PhysicsWorld::AddParticle(P6Particle* toAdd)
{
	this->Particles.push_back(toAdd);
	//forceRegistry.Add(toAdd, &Gravity); //comment when you want to remove gravity
}

void P6::PhysicsWorld::Update(float time)
{
	UpdateParticleList();

	forceRegistry.UpdateForces(time);

	for (std::list<P6Particle*>::iterator p = Particles.begin(); p != Particles.end(); p++)
	{
		(*p)->Update(time);
	}

	GenerateContacts();

	if (Contacts.size() > 0)
	{
		// Convert std::list to std::vector
		std::vector<P6::ParticleContact*> contactsVector(Contacts.begin(), Contacts.end());
		contactResolver.ResolveContacts(contactsVector, time);
	}
}

void P6::PhysicsWorld::AddContact(P6Particle* p1, P6Particle* p2, float restitution, MyVector contactNormal, float depth)
{
	ParticleContact* toAdd = new ParticleContact();
	toAdd->particles[0] = p1;
	toAdd->particles[1] = p2;

	toAdd->restitution = restitution;
	toAdd->contactNormal = contactNormal;
	toAdd->depth = depth;

	Contacts.push_back(toAdd);
}

void P6::PhysicsWorld::UpdateParticleList()
{
	Particles.remove_if([](P6Particle* p) { return p->IsDestroyed(); });
}

void P6::PhysicsWorld::GetOverlaps()
{
	for (int i = 0; i < Particles.size(); i++)
	{
		std::list<P6Particle*>::iterator a = std::next(Particles.begin(), i);

		for (int h = i + 1; h < Particles.size(); h++)
		{
			std::list<P6Particle*>::iterator b = std::next(Particles.begin(), h);
			MyVector mag2Vector = (*a)->Position - (*b)->Position;
			float mag2 = mag2Vector.SqMagnitude();
			

			float rad = (*a)->radius + (*b)->radius;
			float rad2 = rad * rad;

			if (mag2 <= rad2)
			{
				MyVector dir = mag2Vector.normalize();
				float r = rad2 - mag2;
				float depth = sqrt(r);

				float resitution = fmin((*a)->restitution, (*b)->restitution);
				AddContact(*a, *b, resitution, dir, depth);
				std::cout << "mag2: " << mag2 << std::endl;
			}

			
		}
	}
}

void P6::PhysicsWorld::GenerateContacts()
{
	Contacts.clear();

	GetOverlaps();

	for (std::list<ParticleLink*>::iterator i = Links.begin(); i != Links.end(); i++)
	{
		ParticleContact* contact = (*i)->GetContact();

		if (contact != nullptr)
		{
			Contacts.push_back(contact);
		}
	}
}
