#pragma once

#define CHECK_VALID( _v ) 0

class Vector3
{
public:
	float x, y, z;

	Vector3() {};

	Vector3(float x, float y, float z)
	{
		this->x;
		this->y;
		this->z;
	}

	Vector3 operator + (Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator - (Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	float Dot(const Vector3& vOther) const
	{
		const Vector3& a = *this;

		return(a.x*vOther.x + a.y*vOther.y + a.z*vOther.z);
	}
};
