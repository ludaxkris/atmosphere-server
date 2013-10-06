#pragma once
#ifndef _CONFIGMANAGER_
#define _CONFIGMANAGER_

//#include "./../Common/dxstdafx.h"
// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "tinyxml.h"
#include <string.h>
#include <vector>
#include <exception>


/** 
 * Object for the controller class. 
 */
class ControllerConfig
{
public:	
	ControllerConfig();
	~ControllerConfig();
	int loadOkay;
	std::vector <char *> stateNames;
	std::vector <int> stateMin;
	std::vector <int> stateMax;
};

/** 
 * Object for the Atmosphere game. 
 */
class AtmosphereConfig
{
public:	
	AtmosphereConfig();
	~AtmosphereConfig();
	int loadOkay;
	char * serverIP;
	char * serverPort;
	int screenSizeWidth;
	int screenSizeHeight;
};


/**
 * Object used by ObjManagerConfig class, 
 * specifically for player, base, and AI.
 */
class ObjManagerObjects 
{
public:
	ObjManagerObjects();
	~ObjManagerObjects();
	D3DXVECTOR4 color;
	D3DXVECTOR3 position;
	float scaleX, scaleY, scaleZ;
	char * texture;
	char * mesh;
};


/** 
 * Object for the Object Manager Class. 
 */
class ObjManagerConfig
{
public:	
	ObjManagerConfig();
	~ObjManagerConfig();
	int loadOkay;

	std::vector <ObjManagerObjects *> players;
	std::vector <ObjManagerObjects *> bases;
	std::vector <ObjManagerObjects *> AI;

	//Used by world objects
	char * worldColorMap;
	char * worldNormalMap;
	char * worldIrraTexture;
	char * worldMesh;
	D3DXVECTOR3 worldPosition;
	float worldScaleX, worldScaleY, worldScaleZ;

	//Used by sky box
	float skyboxScaleX, skyboxScaleY, skyboxScaleZ;
	char * skyboxCubeTexture;
	char * skyboxMesh;

	ObjManagerObjects * loadPlayer(TiXmlElement * player);
	ObjManagerObjects * loadBase(TiXmlElement * base);
	ObjManagerObjects * loadAI(TiXmlElement * AI);
};

class ConfigManager
{
public:
	ConfigManager();
	
	//Functions called by other components.
	ControllerConfig * buildController();
	AtmosphereConfig * buildAtmosphere();
	ObjManagerConfig * buildObjManager();	

	//File names
	char * controllerXML;
	char * atmosphereXML;
	char * objManagerXML;
};


#endif; /* _CONFIGMANAGER_ */