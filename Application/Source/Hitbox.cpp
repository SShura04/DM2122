#include "Hitbox.h"

Hitbox::Hitbox()
{
	hitboxcenterposition = Vector3(0, 0, 0);
}

Hitbox::~Hitbox()
{
}

Vector3 Hitbox::gethitboxcenterposition()
{
	return hitboxcenterposition;
}

Vector3 Hitbox::gethitboxdimensions()
{
	return hitboxdimension;
}

Vector3 Hitbox::gethboffset()
{
	return hboffset;
}

float Hitbox::getsphereofinfluenceradius()
{
	return sphereofinfluenceradius;
}

void Hitbox::sethitboxcenterdimensions(Vector3 hitboxdimensions, Vector3 hitboxcenterpos, Vector3 hitboxoffset)
{
	hitboxcenterposition = hitboxcenterpos;
	hitboxdimension = hitboxdimensions;
	hboffset = hitboxoffset;
	hitboxcenterposition += hboffset;
	//generate x min, x max, etc
	xmin = hitboxcenterpos.x - hitboxdimensions.x + hitboxoffset.x;
	ymin = hitboxcenterpos.y - hitboxdimensions.y + hitboxoffset.y;
	zmin = hitboxcenterpos.z - hitboxdimensions.z + hitboxoffset.z;
	xmax = hitboxcenterpos.x + hitboxdimensions.x + hitboxoffset.x;
	ymax = hitboxcenterpos.y + hitboxdimensions.y + hitboxoffset.y;
	zmax = hitboxcenterpos.z + hitboxdimensions.z + hitboxoffset.z;
	
	//calculate the sphere of influence radius
	sphereofinfluenceradius = (Vector3 (xmax, ymax, zmax) - Vector3 (xmin, ymin, zmin)).Length() * 0.5;
}


void Hitbox::scalehitbox(Vector3 scale)
{
	hitboxdimension.x = hitboxdimension.x * scale.x;
	hitboxdimension.y = hitboxdimension.y * scale.y;
	hitboxdimension.z = hitboxdimension.z * scale.z;

	sethitboxcenterdimensions(hitboxdimension, hitboxcenterposition, hboffset);
}

void Hitbox::updatehitboxpos(Vector3 newposition)
{
	hitboxcenterposition = newposition;

	sethitboxcenterdimensions(hitboxdimension, newposition, hboffset);
}

bool Hitbox::Checkforcollision(Hitbox otherhitbox, Hitbox prevHitbox, bool* x, bool* y, bool* z)
{
	*x = false; *y = false; *z = false;
	Vector3 distancebetweenhitboxes = otherhitbox.gethitboxcenterposition() - hitboxcenterposition;
	bool collide = false;
	//check if the other hitbox is close enough before checking for collisions
	if (distancebetweenhitboxes.Length() < (sphereofinfluenceradius + otherhitbox.getsphereofinfluenceradius()))
	{
		//since the hitbox does not rotate, checking against the axis is enough
		if ((xmin <= otherhitbox.xmin && xmax >= otherhitbox.xmin) || (xmin <= otherhitbox.xmax && xmax >= otherhitbox.xmax))
			collide = true;
		else
			collide = false;
		if (collide == true && ((ymin <= otherhitbox.ymin && ymax >= otherhitbox.ymin) || (ymin <= otherhitbox.ymax && ymax >= otherhitbox.ymax)))
			collide = true;
		else
			collide = false;
		if (collide == true && ((zmin <= otherhitbox.zmin && zmax >= otherhitbox.zmin) || (zmin <= otherhitbox.zmax && zmax >= otherhitbox.zmax)))
			collide = true;
		else
			collide = false;

		if (collide == true)
		{
			if (prevHitbox.gethitboxcenterposition().x >= otherhitbox.xmin && prevHitbox.gethitboxcenterposition().x <= otherhitbox.xmax)
				*x = true;
			else
				*x = false;
			if (prevHitbox.gethitboxcenterposition().y >= otherhitbox.ymin && prevHitbox.gethitboxcenterposition().y <= otherhitbox.ymax)
				*y = true;
			else
				*y = false;
			if (prevHitbox.gethitboxcenterposition().z >= otherhitbox.zmin && prevHitbox.gethitboxcenterposition().z <= otherhitbox.zmax)
				*z = true;
			else
				*z = false;

		}
		return collide;
	}
	return false;
}

bool Hitbox::Checkforcollision(Hitbox otherhitbox)
{
	Vector3 distancebetweenhitboxes = otherhitbox.gethitboxcenterposition() - hitboxcenterposition;
	bool collide = false;
	//check if the other hitbox is close enough before checking for collisions
	if (distancebetweenhitboxes.Length() < (sphereofinfluenceradius + otherhitbox.getsphereofinfluenceradius()))
	{
		//since the hitbox does not rotate, checking against the axis is enough
		if ((xmin <= otherhitbox.xmin && xmax >= otherhitbox.xmin) || (xmin <= otherhitbox.xmax && xmax >= otherhitbox.xmax))
			collide = true;
		else
			collide = false;
		if (collide == true && ((ymin <= otherhitbox.ymin && ymax >= otherhitbox.ymin) || (ymin <= otherhitbox.ymax && ymax >= otherhitbox.ymax)))
			collide = true;
		else
			collide = false;
		if (collide == true && ((zmin <= otherhitbox.zmin && zmax >= otherhitbox.zmin) || (zmin <= otherhitbox.zmax && zmax >= otherhitbox.zmax)))
			collide = true;
		else
			collide = false;
		return collide;
	}
	return false;
}
