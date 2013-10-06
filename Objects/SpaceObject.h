#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "SharedInfo.h"

#define SPEED_RATIO 0.05f
#define PROJ_SPEED 0.01f

class SpaceObject 
{
public:
	//Everyone's stuff
	/////////////////////////////////////
	SpaceObject(void);
	SharedInfoSpaceObject * sharedInfo;
	/////////////////////////////////////

	//dan's and arjay's collision detection
	//////////////////////////////////////////////
	float radius;
	//////////////////////////////////////////////


	//Arjay's system stuff
	////////////////////////////////////////////
	int index;
	
	//bool to say if it's taken or not... do later
	int getIndex(){return index;}
	void setIndex(int ind){index = ind;}

	virtual bool isAIShip(){return false;}
	virtual bool isPlayerShip(){return false;}
	virtual bool isAsteroid(){return false;}
	virtual bool isBase(){return false;}
	virtual bool isProjectile(){return false;}
	virtual bool isWorld(){return false;}
	////////////////////////////////////////////
	
	//Nick's graphics stuff
	///////////////////////////////////////////
	LPD3DXMESH				mesh;			
	void SpaceObject::initObject(float _scale, float _height, D3DXVECTOR3 normal);
	///////////////////////////////////////////
	
	//Daniel's physics stuff
	////////////////////////////////////////////
	void initObject(float _scale, float _height);
	void initRotXY(D3DXMATRIX _matRotXY);
	D3DXMATRIX getRotXY();
	void UpdatePosition(D3DXVECTOR2 direction);
	D3DXVECTOR3 getPosition();
	////////////////////////////////////////////

protected:
	//Daniel's physics stuff
	//////////////////////////////////////
	float					scale;
	float					height;
	float					zradian;
	D3DXMATRIX				matRotXY;
	D3DXMATRIX				matTransform;
	D3DXVECTOR3				position;
	//////////////////////////////////////

private:

};
