#pragma once
#include "Vector3.h"
#include "Mtx44.h"
#include <iostream>
#include "Hitbox.h"

class Objects
{
private:
	bool Issetup;
	Vector3 position; //xyz
	Vector3 rotation; //pitch roll yaw
	Vector3 scale;
	Hitbox objecthitbox; //hitbox

	Vector3 CollisionSize;
	unsigned model;
public:
	Objects();
	~Objects();
	Vector3 getposition();
	Vector3 getrotation();
	Vector3 getscale();
	Hitbox getobjecthitbox();
	bool getissetup();
	unsigned getmodel();

	void Setposition(Vector3 newposition = Vector3(0, 0, 0));
	void moveposition(Vector3 movepara);
	void setrotation(Vector3 rotatepara);
	void setscale(Vector3 scalepara);
	void setproperties(Vector3 newposition, Vector3 newrotate, Vector3 newscale);
	void Setuphitbox(Vector3 hitboxdimensions, Vector3 hitboxoffset);
	void setmesh(unsigned newmodel);


	//Test
	void sethitboxcollisionsize(Vector3 size);
	Vector3 gethitboxcollisionsize();
};


