#include "Objects.h"

Objects::Objects()
{
	Issetup = false;
	rotation = Vector3(0, 0, 0);
	position = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	model = 0;
}

Objects::~Objects()
{
}

Vector3 Objects::getposition()
{
	return position;
}

Vector3 Objects::getrotation()
{
	return rotation;
}

Vector3 Objects::getscale()
{
	return scale;
}

Hitbox Objects::getobjecthitbox()
{
	return objecthitbox;
}

bool Objects::getissetup()
{
	return Issetup;
}

unsigned Objects::getmodel()
{
	return model;
}


void Objects::Setposition(Vector3 newposition)
{
	position = newposition;
	Setuphitbox(objecthitbox.gethitboxdimensions(), objecthitbox.gethboffset());
}

void Objects::moveposition(Vector3 movepara)
{
	position = position + movepara;
	Setuphitbox(objecthitbox.gethitboxdimensions(), objecthitbox.gethboffset());
}

void Objects::setrotation(Vector3 rotatepara)
{
	rotation = rotatepara;
}

void Objects::setscale(Vector3 scalepara)
{
	scale = scalepara;
}

void Objects::setproperties(Vector3 newposition, Vector3 newrotate, Vector3 newscale)
{
	position = newposition;
	rotation = newrotate;
	scale = newscale;
}

void Objects::Setuphitbox(Vector3 hitboxdimensions, Vector3 hitboxoffset)
{
	Issetup = true;
	objecthitbox.sethitboxcenterdimensions(hitboxdimensions, position, hitboxoffset);
}

void Objects::setmesh(unsigned newmodel)
{
	model = newmodel;
}



void Objects::sethitboxcollisionsize(Vector3 size)
{
	CollisionSize = size;
}

Vector3 Objects::gethitboxcollisionsize()
{
	return CollisionSize;
}
