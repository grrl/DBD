#pragma once
#include "fvector.h"
#include "structs.h"
#include "d3d9.h"

/*
bool WorldToScreen(D3DXVECTOR3 in, D3DXVECTOR2 * out)
{
	D3DXMATRIX viewMatrix = localPlayer->GetCameraShit()->GetViewMatrix();
	D3DXMATRIX temp;

	D3DXMatrixTranspose(&temp, &viewMatrix);

	D3DXVECTOR3 translationVector = D3DXVECTOR3(temp._41, temp._42, temp._43);
	D3DXVECTOR3 right = D3DXVECTOR3(temp._21, temp._22, temp._23);
	D3DXVECTOR3 up = D3DXVECTOR3(temp._31, temp._32, temp._33);

	float w = D3DXVec3Dot(&translationVector, &in) + temp._44;

	if (w < 0.098f)
		return false;

	float y = D3DXVec3Dot(&up, &in) + temp._34;
	float x = D3DXVec3Dot(&right, &in) + temp._24;

	out->x = (res.x / 2) + x * ((res.x / 2) / tan(fov * 3.14 / 360.0f)) / w;
	out->y = (res.y / 2) + -y * ((res.x / 2) / tan(fov * 3.14 / 360.0f)) / w;

	return true;
}


void Engine::GetAxes(Classes::FRotator Rot, Classes::FVector& X, Classes::FVector& Y, Classes::FVector& Z)
{
	float	M[4][4];

	const float	SR = sin(Rot.Roll * M_PI / 180.f);
	const float	SP = sin(Rot.Pitch * M_PI / 180.f);
	const float	SY = sin(Rot.Yaw * M_PI / 180.f);
	const float	CR = cos(Rot.Roll * M_PI / 180.f);
	const float	CP = cos(Rot.Pitch * M_PI / 180.f);
	const float	CY = cos(Rot.Yaw * M_PI / 180.f);

	M[0][0] = CP * CY;
	M[0][1] = CP * SY;
	M[0][2] = SP;
	M[0][3] = 0.f;

	M[1][0] = SR * SP * CY - CR * SY;
	M[1][1] = SR * SP * SY + CR * CY;
	M[1][2] = -SR * CP;
	M[1][3] = 0.f;

	M[2][0] = -(CR * SP * CY + SR * SY);
	M[2][1] = CY * SR - CR * SP * SY;
	M[2][2] = CR * CP;
	M[2][3] = 0.f;

	M[3][0] = 0.0f;
	M[3][1] = 0.0f;
	M[3][2] = 0.0f;
	M[3][3] = 1.f;

	X.X = M[0][0]; X.Y = M[0][1]; X.Z = M[0][2];
	Y.X = M[1][0]; Y.Y = M[1][1]; Y.Z = M[1][2];
	Z.X = M[2][0]; Z.Y = M[2][1]; Z.Z = M[2][2];
}

Classes::FVector2D Engine::WorldToScreen(Classes::FVector WorldLocation, Classes::FVector2D out)
{
	Classes::FMinimalViewInfo POV = G::fpov;
	Classes::FRotator Rotation = POV.Rotation;

	Classes::FVector vAxisX, vAxisY, vAxisZ;
	GetAxes(Rotation, vAxisX, vAxisY, vAxisZ);

	Classes::FVector vDelta = WorldLocation - POV.Location;
	Classes::FVector vTransformed = Classes::FVector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.Z < 1.f)
		vTransformed.Z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = 1920 / 2.0f;
	float ScreenCenterY = 1080 / 2.0f;

	out.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(FovAngle * M_PI / 360.0f)) / vTransformed.Z;
	out.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(FovAngle * M_PI / 360.0f)) / vTransformed.Z;
	return out;

	//return true;
}
*/

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

D3DXMATRIX Matrix(FRotator rot, FRotator origin = FRotator(0, 0, 0))
{
	float radPitch = (rot.Pitch * float(M_PI) / 180.f);
	float radYaw = (rot.Yaw * float(M_PI) / 180.f);
	float radRoll = (rot.Roll * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.Pitch;
	matrix.m[3][1] = origin.Yaw;
	matrix.m[3][2] = origin.Roll;
	matrix.m[3][3] = 1.f;

	return matrix;
}

FVector WorldToScreen(FMinimalViewInfo Info, FVector WorldLocation) {

	FVector Screenlocation = FVector(0, 0, 0);
	FRotator Rotation = Info.Rotation;
	D3DMATRIX tempMatrix = Matrix(Rotation);

	FVector vAxisX, vAxisY, vAxisZ;

	vAxisX = FVector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = FVector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = FVector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	FVector vDelta = WorldLocation - Info.Location;
	FVector vTransformed = FVector(vDelta.Dot(vDelta, vAxisY), vDelta.Dot(vDelta, vAxisZ), vDelta.Dot(vDelta, vAxisX));

	if (vTransformed.Z < 1.f)
		vTransformed.Z = 1.f;

	//float FovAngle = Info.FOV;
	float FovAngle = Info.FOV;

	//std::cout << "Info.Location  " << Info.Location.X << " y" << Info.Location.Y << std::endl;

	float ScreenCenterX = windowWidth / 2;
	float ScreenCenterY = windowHeight / 2;

	Screenlocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
	Screenlocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

	return Screenlocation;
}