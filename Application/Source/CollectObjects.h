#ifndef CollectObjects_H
#define CollectObjects_H

class CollectObjects
{
private:
	float x, y, z;
public:
	CollectObjects();
	void SetXYZ(float x, float y, float z);
	float GetX();
	float GetY();
	float GetZ();
};

#endif