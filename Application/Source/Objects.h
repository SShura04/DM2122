#pragma once
#include "Vector3.h"
#include "Hitbox.h"
#include "Mtx44.h"
#include <iostream>
#include <vector>
class Objects
{
private:
	bool Issetup;
	Vector3 position; //xyz
	Vector3 rotationparentchild; //for both parent and child
	Vector3 rotationparentonly; //for only the parent
	Vector3 scaleparentonly;
	Vector3 scaleparentchild;
	std::vector<Objects*> Childobjects;
	Vector3 CollisionSize;
	Hitbox objecthitbox;
	unsigned model;
	bool lighting, renderobject;
public:
	Objects();
	~Objects();
	Vector3 getposition();
	Vector3 getrotationparentonly();
	Vector3 getrotationparentchild();
	Vector3 getscaleparentchild();
	Vector3 getscaleparentonly();
	bool getissetup();
	unsigned getmodel();
	bool getlighting();
	bool getrenderobject();
	Hitbox gethitbox();
	std::vector<Objects*> GetChildlist();

	void AddChildobject(Objects newchild);
	void Setposition(Vector3 newposition = Vector3(0, 0, 0));
	void moveposition(Vector3 movepara);
	void setrotationparentonly(Vector3 rotatepara);
	void moverotationparentonly(Vector3 rotatepara);
	void setrotationparentchild(Vector3 rotatepara);
	void moverotationparentchild(Vector3 rotatepara);
	void setscaleparentonly(Vector3 scalepara);
	void setscaleparentchild(Vector3 scalepara);
	void setproperties(Vector3 newposition, Vector3 newrotate, Vector3 newscale);
	void setproperties(Vector3 newposition, Vector3 newrotateparentchild, Vector3 newrotateparentonly, Vector3 newscaleparentchild, Vector3 newscaleparentonly);
	void setmesh(unsigned newmodel);
	void setlighting(bool setlight);
	void setrenderobject(bool setrenderobject);

	void sethitboxcollisionsize(Vector3 size);
	Vector3 gethitboxcollisionsize();
};