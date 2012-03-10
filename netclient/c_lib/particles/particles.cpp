#include "particles.hpp"

#include <c_lib/camera/camera.hpp>

void Particle::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}


Particle::Particle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
id(id),
ttl(0)
{
    vp = new VerletParticle(x,y,z,mx,my,mz, mass);
}

Particle::~Particle()
{
    delete vp;
}

EventParticle::EventParticle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
Particle(id, x,y,z,mx,my,mz, mass)
{}
