#include "CollectObjects.h"

CollectObjects::CollectObjects()
{
	x = 0;
	y = 0;
	z = 0;
}

void CollectObjects::SetXYZ(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

float CollectObjects::GetX()
{
	return x;
}

float CollectObjects::GetY()
{
	return y;
}

float CollectObjects::GetZ()
{
	return z;
}

