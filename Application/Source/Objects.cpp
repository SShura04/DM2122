#include "Objects.h"

Objects::Objects()
{
	Issetup = false;
	lighting = true;
	rotationparentonly = Vector3(0, 0, 0);
	rotationparentchild = Vector3(0, 0, 0);
	position = Vector3(0, 0, 0);
	scaleparentonly = Vector3(1, 1, 1);
	scaleparentchild = Vector3(1, 1, 1);
	model = 0;
	renderobject = true;
}

Objects::~Objects()
{
	std::vector<Objects*> ::iterator it = Childobjects.begin();
	while (Childobjects.size() > 0)
	{
		delete Childobjects[0];
		Childobjects.erase(it);
		it = Childobjects.begin();
	}
}

Vector3 Objects::getposition()
{
	return position;
}

Vector3 Objects::getrotationparentonly()
{
	return rotationparentonly;
}

Vector3 Objects::getrotationparentchild()
{
	return rotationparentchild;
}

Vector3 Objects::getscaleparentonly()
{
	return scaleparentonly;
}

Vector3 Objects::getscaleparentchild()
{
	return scaleparentchild;
}

bool Objects::getissetup()
{
	return Issetup;
}

unsigned Objects::getmodel()
{
	return model;
}

bool Objects::getlighting()
{
	return lighting;
}

bool Objects::getrenderobject()
{
	return renderobject;
}

Hitbox Objects::gethitbox()
{
	return objecthitbox;
}

std::vector<Objects*> Objects::GetChildlist()
{
	return Childobjects;
}


void Objects::AddChildobject(Objects newchild)
{
	Objects* tempobj = new Objects;
	*tempobj = newchild;
	Childobjects.push_back(tempobj);
}

void Objects::Setposition(Vector3 newposition)
{
	position = newposition;
	objecthitbox.updatehitboxpos(newposition);
}

void Objects::moveposition(Vector3 movepara)
{
	position = position + movepara;
}

void Objects::setrotationparentonly(Vector3 rotatepara)
{
	rotationparentonly = rotatepara;
}

void Objects::moverotationparentonly(Vector3 rotatepara)
{
	rotationparentonly += rotatepara;
}

void Objects::setrotationparentchild(Vector3 rotatepara)
{
	rotationparentchild = rotatepara;
}

void Objects::moverotationparentchild(Vector3 rotatepara)
{
	rotationparentchild += rotatepara;
}

void Objects::setscaleparentonly(Vector3 scalepara)
{
	scaleparentonly = scalepara;
}

void Objects::setscaleparentchild(Vector3 scalepara)
{
	scaleparentchild = scalepara;
}

void Objects::setproperties(Vector3 newposition, Vector3 newrotate, Vector3 newscale)
{
	position = newposition;
	rotationparentonly = newrotate;
	scaleparentonly = newscale;
	Issetup = true;
}

void Objects::setproperties(Vector3 newposition, Vector3 newrotateparentchild, Vector3 newrotateparentonly, Vector3 newscaleparentchild, Vector3 newscaleparentonly)
{
	position = newposition;
	rotationparentchild = newrotateparentchild;
	rotationparentonly = newrotateparentonly;
	scaleparentchild = newscaleparentchild;
	scaleparentonly = newscaleparentonly;
	Issetup = true;
}

void Objects::setmesh(unsigned newmodel)
{
	model = newmodel;
}

void Objects::setlighting(bool setlight)
{
	lighting = setlight;
}

void Objects::setrenderobject(bool setrenderobject)
{
	renderobject = setrenderobject;
}

void Objects::sethitboxcollisionsize(Vector3 size)
{
	CollisionSize = size;
	Vector3 tempsize = size * 0.5;
	objecthitbox.sethitboxcenterdimensions(tempsize, position);
}

Vector3 Objects::gethitboxcollisionsize()
{
	return CollisionSize;
}
