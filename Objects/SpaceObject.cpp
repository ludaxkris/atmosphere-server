#include "SpaceObject.h"
#include "strsafe.h"

SpaceObject::SpaceObject(void)
{
	this->radius = 0;
}

void SpaceObject::initObject(float _scale, float _height)
{
	scale = _scale;
	height = _height;
	zradian = 0;
	D3DXMatrixIdentity(&matRotXY);
	matTransform = matRotXY;
	matTransform._31 = scale;
	matTransform._32 = height;
	matTransform._33 = zradian;
	sharedInfo->matTransform = matTransform;
	position = D3DXVECTOR3(0,0,height);
}

void SpaceObject::initObject(float _scale, float _height, D3DXVECTOR3 normal)
{
}

void SpaceObject::initRotXY(D3DXMATRIX _matRotXY)
{
	matRotXY = _matRotXY;
}

D3DXMATRIX SpaceObject::getRotXY()
{
	return matRotXY;
}

void SpaceObject::UpdatePosition(D3DXVECTOR2 direction)
{
	float speed;
	if(isPlayerShip()) {
		float mag = D3DXVec2Length(&direction);
		speed = SPEED_RATIO*mag*0.2;
	}
	else if(isProjectile()) {
		speed = PROJ_SPEED;
	}
	zradian = atanf(direction.y/direction.x);
	if(direction.y >= 0 && direction.x >= 0)
		zradian += 3*D3DX_PI/2;
	else if(direction.y >= 0 && direction.x <= 0)
		zradian += D3DX_PI/2;
	else if(direction.y <= 0 && direction.x <= 0)
		zradian += D3DX_PI/2;
	else
		zradian += 3*D3DX_PI/2;

	D3DXVECTOR3 xaxis = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 zaxis = D3DXVECTOR3(0,0,1);
	D3DXVECTOR3 right;

	D3DXMATRIX matRightReorient;
	D3DXMatrixRotationAxis(&matRightReorient, &zaxis, zradian);
	D3DXVec3TransformCoord(&right, &xaxis, &matRightReorient);

	D3DXMATRIX matRotXYInc;
	D3DXMatrixRotationAxis(&matRotXYInc, &right, speed);
	matRotXY = matRotXYInc * matRotXY;

	matTransform = matRotXY;
	matTransform._31 = scale;
	matTransform._32 = height;
	matTransform._33 = zradian;

	sharedInfo->matTransform = matTransform;

	D3DXMATRIX matHeight;
	D3DXMatrixTranslation(&matHeight, 0, 0, height);
	D3DXMATRIX matPosition = matHeight * matRotXY;
	position.x = matPosition._41;
	position.y = matPosition._42;
	position.z = matPosition._43;
}

D3DXVECTOR3 SpaceObject::getPosition()
{
	return position;
}