#pragma once
#include <list>
#include "P6Particle.h"

#include "ForceRegistry.h"
#include "GravityForceGenerator.h"
#include "ParticleLink.h"

#include "ContactResolver.h"

namespace P6 {
	class PhysicsWorld
	{
		public:
			ForceRegistry forceRegistry;
			
			std::list<P6Particle*> Particles;

			void AddParticle(P6Particle* toAdd);
			void Update(float time);

			std::list<ParticleContact*> Contacts;

			void AddContact(P6Particle* p1, P6Particle* p2, float restitution, MyVector contactNormal, float depth);
			std::list<ParticleLink*> Links;
			void setGravity(float gravityStr);

			float baseGRAVITY = -9.8f; // Default gravity strength

			void gravMULT(float multiple) {
				this->baseGRAVITY *= multiple;
				setGravity(baseGRAVITY);
			}

			GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0, baseGRAVITY, 0)); //0, -9.8f, 0


		private:
			void UpdateParticleList();
		protected:
			ContactResolver contactResolver = ContactResolver(20);

			void GetOverlaps();
			void GenerateContacts();
	};
}


