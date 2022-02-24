#pragma once
#include "Vector3.h"
#include <vector>
#include "MyMath.h"
class Hitbox
{
private:
	Vector3 hitboxcenterposition;
	Vector3 hitboxdimension;
	Vector3 hboffset;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float sphereofinfluenceradius;
public:
	Hitbox();
	~Hitbox();
	Vector3 gethitboxcenterposition();
	Vector3 gethitboxdimensions();
	Vector3 gethboffset();
	float getsphereofinfluenceradius();
	void sethitboxcenterdimensions(Vector3 hitboxdimensions, Vector3 hitboxcenterpos, Vector3 hitboxoffset = Vector3(0, 0, 0));
	void scalehitbox(Vector3 scale);
	void updatehitboxpos(Vector3 newposition);

	bool Checkforcollision(Hitbox otherhitbox, Hitbox prevhitbox, bool *x, bool *y, bool *z);
	bool Checkforcollision(Hitbox otherhitbox);


	//Vector3 PlayerCollision(unsigned count, Hitbox otherhitbox);
	//Vector3 CollisionCircleRect(float cx, float cy, float radius, float rx, float ry, float rw, float rh);
};

