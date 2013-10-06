#include "ConfigManager.h"


ConfigManager::ConfigManager() {
	controllerXML = "./config/controller.config";
	atmosphereXML = "./config/atmosphere.config";
	objManagerXML = "./config/ObjManager.config";
}


///////////////////////////////////////////////////////////////////////////////
//Controller related functions

ControllerConfig::ControllerConfig() {
}

ControllerConfig::~ControllerConfig() {
	for(unsigned int i=0; i<this->stateNames.size(); ++i ) {
		delete [] (this->stateNames.at(i));
		this->stateNames.at(i) = NULL;
	}
}

ControllerConfig * ConfigManager::buildController() {
	//Create new ControllerConfig object
	ControllerConfig * controllerConf = new ControllerConfig();

	TiXmlDocument doc(controllerXML);
	controllerConf->loadOkay = doc.LoadFile();

	//This should be XMLException
	if (!controllerConf->loadOkay)
		throw std::exception();
	else {		
		TiXmlHandle root( &doc );
		TiXmlElement * statesNode = root.FirstChild("Controller").FirstChildElement("States").ToElement();

		//Set each state
		TiXmlNode * pChild = statesNode->FirstChildElement("state");
		for ( ; pChild != NULL; pChild = statesNode->IterateChildren("state", pChild)) 
		{			
			TiXmlElement * node = pChild->ToElement();

			//Set name
			char * stateValue = new char[40];
			strcpy(stateValue, node->Attribute("name"));			
			controllerConf->stateNames.push_back(stateValue);

			//Set min/max values
			int minValue;
			int maxValue;
			node->QueryIntAttribute("min", &minValue);
			node->QueryIntAttribute("max", &maxValue);

			controllerConf->stateMin.push_back(minValue);
			controllerConf->stateMax.push_back(maxValue);
		}
	}

	return controllerConf;
}

///////////////////////////////////////////////////////////////////////////////
//Atmosphere related functions

AtmosphereConfig::AtmosphereConfig() {
	this->serverIP = new char[20];
	this->serverPort = new char[10];
}

AtmosphereConfig::~AtmosphereConfig() {
	delete [] serverIP;
	serverIP = NULL;
	delete [] serverPort;
	serverPort = NULL;
}

AtmosphereConfig * ConfigManager::buildAtmosphere() {
	//Create new AtmosphereConfig object
	AtmosphereConfig * atmosphereConf = new AtmosphereConfig();

	TiXmlDocument doc(atmosphereXML);
	atmosphereConf->loadOkay = doc.LoadFile();

	//This should be XMLException
	if (!atmosphereConf->loadOkay)
		throw std::exception();

	else {		
		TiXmlHandle root( &doc );
		TiXmlElement * rootNode = root.FirstChild("Atmosphere").ToElement();

		//Traverse into each node and copy the values into the return object.
		//serverIP		
		strcpy(atmosphereConf->serverIP, rootNode->FirstChild("serverIP")->FirstChild()->ToText()->Value());

		//serverPort		
		strcpy( atmosphereConf->serverPort, rootNode->FirstChild("serverPort")->FirstChild()->ToText()->Value() );

		//screenSizeWidth
		atmosphereConf->screenSizeWidth = atoi( rootNode->FirstChild("screenSizeWidth")->FirstChild()->ToText()->Value() );

		//screenSizeHeight
		atmosphereConf->screenSizeHeight = atoi( rootNode->FirstChild("screenSizeHeight")->FirstChild()->ToText()->Value() );
	}
	return atmosphereConf;
}

///////////////////////////////////////////////////////////////////////////////
//ObjManagerObjects related functions

ObjManagerObjects::ObjManagerObjects() {
	this->texture = new char[80];
	this->mesh = new char[80];
}

ObjManagerObjects::~ObjManagerObjects() {
	delete [] this->texture;
	this->texture = NULL;
	delete [] this->mesh;
	this->mesh = NULL;
}

///////////////////////////////////////////////////////////////////////////////
//ObjManager related functions

ObjManagerConfig::ObjManagerConfig() {
	this->worldColorMap = new char[80];
	this->worldNormalMap = new char[80];
	this->worldIrraTexture = new char[80];
	this->worldMesh = new char[80];
	this->skyboxCubeTexture = new char[80];
	this->skyboxMesh = new char[80];
}

ObjManagerConfig::~ObjManagerConfig() {
	//Player
	for(unsigned int i=0; i<this->players.size(); ++i) {
		delete this->players.at(i);
		this->players.at(i) = NULL;
	}

	//World
	delete [] worldColorMap;
	worldColorMap = NULL;
	delete [] worldNormalMap;
	worldNormalMap = NULL;
	delete [] worldIrraTexture;
	worldIrraTexture = NULL;
	delete [] worldMesh;
	worldMesh = NULL;

	//Base
	for(unsigned int i=0; i<this->bases.size(); ++i) {
		delete this->bases.at(i);
		this->bases.at(i) = NULL;
	}

	//AI
	for(unsigned int i=0; i<this->AI.size(); ++i) {
		delete this->AI.at(i);
		this->AI.at(i) = NULL;
	}

	//Skybox
	delete [] skyboxCubeTexture;
	skyboxCubeTexture = NULL;
	delete [] skyboxMesh;
	skyboxMesh = NULL;
}

ObjManagerConfig * ConfigManager::buildObjManager() {
	//Create new ObjManagerConfig object
	ObjManagerConfig * ObjManagerConf = new ObjManagerConfig();

	TiXmlDocument doc(objManagerXML);
	ObjManagerConf->loadOkay = doc.LoadFile();

	//This should be XMLException
	if (!ObjManagerConf->loadOkay)
		throw std::exception();

	else {		
		TiXmlHandle root( &doc );
		TiXmlElement * rootNode = root.FirstChild("ObjManager").ToElement();

		////////////////////////////////////
		//Players
		TiXmlElement * rootPlayer = rootNode->FirstChild("Player")->ToElement();

		TiXmlNode * pChild = rootPlayer->FirstChildElement();
		for ( ; pChild != NULL; pChild = rootPlayer->IterateChildren(pChild) ) 
		{			
			ObjManagerConf->players.push_back( ObjManagerConf->loadPlayer(pChild->ToElement()) );
		}

		////////////////////////////////////
		//World
		TiXmlElement * rootWorld = rootNode->FirstChild("World")->ToElement();
		strcpy(ObjManagerConf->worldColorMap, rootWorld->FirstChild("colorMap")->ToElement()->Attribute("file"));
		strcpy(ObjManagerConf->worldNormalMap, rootWorld->FirstChild("normalMap")->ToElement()->Attribute("file"));
		strcpy(ObjManagerConf->worldIrraTexture, rootWorld->FirstChild("irraTexture")->ToElement()->Attribute("file"));
		strcpy(ObjManagerConf->worldMesh, rootWorld->FirstChild("mesh")->ToElement()->Attribute("file"));

		//Position
		TiXmlElement * position = rootWorld->FirstChild("position")->ToElement();
		float positionX, positionY, positionZ;
		position->QueryFloatAttribute("x", &positionX);
		position->QueryFloatAttribute("y", &positionY);
		position->QueryFloatAttribute("z", &positionZ);
		ObjManagerConf->worldPosition = D3DXVECTOR3(positionX, positionY, positionZ);

		//Scale
		TiXmlElement * scale = rootWorld->FirstChild("scale")->ToElement();
		scale->QueryFloatAttribute("x", &ObjManagerConf->worldScaleX);
		scale->QueryFloatAttribute("y", &ObjManagerConf->worldScaleY);
		scale->QueryFloatAttribute("z", &ObjManagerConf->worldScaleZ);

		////////////////////////////////////
		//Base
		TiXmlElement * rootBase = rootNode->FirstChild("Base")->ToElement();

		pChild = rootBase->FirstChildElement();
		for ( ; pChild != NULL; pChild = rootBase->IterateChildren(pChild) ) 
		{			
			ObjManagerConf->bases.push_back( ObjManagerConf->loadBase(pChild->ToElement()) );
		}

		////////////////////////////////////
		//AI
		TiXmlElement * rootAI = rootNode->FirstChild("AI")->ToElement();

		pChild = rootAI->FirstChildElement();
		for ( ; pChild != NULL; pChild = rootAI->IterateChildren(pChild) ) 
		{			
			ObjManagerConf->AI.push_back( ObjManagerConf->loadAI(pChild->ToElement()) );
		}

		////////////////////////////////////
		//Skybox
		TiXmlElement * rootSkybox = rootNode->FirstChild("SkyBox")->ToElement();
		strcpy(ObjManagerConf->skyboxCubeTexture, rootSkybox->FirstChild("cubeTexture")->ToElement()->Attribute("file"));
		strcpy(ObjManagerConf->skyboxMesh, rootSkybox->FirstChild("mesh")->ToElement()->Attribute("file"));

		//Scale
		TiXmlElement * scaleSkybox = rootSkybox->FirstChild("scale")->ToElement();
		scaleSkybox->QueryFloatAttribute("x", &ObjManagerConf->skyboxScaleX);
		scaleSkybox->QueryFloatAttribute("y", &ObjManagerConf->skyboxScaleY);
		scaleSkybox->QueryFloatAttribute("z", &ObjManagerConf->skyboxScaleZ);
	}
	return ObjManagerConf;
}

/** 
 * Uses the Element node to read in the values needed for a player.
 * Then it builds the object, sets the variables, and returns that object.
 */
ObjManagerObjects * ObjManagerConfig::loadPlayer(TiXmlElement * player) {
	ObjManagerObjects * playerConf = new ObjManagerObjects();

	//Color
	TiXmlElement * color = player->FirstChild("color")->ToElement();
	float colorX, colorY, colorZ, colorW;
	color->QueryFloatAttribute("x", &colorX);
	color->QueryFloatAttribute("y", &colorY);
	color->QueryFloatAttribute("z", &colorZ);
	color->QueryFloatAttribute("w", &colorW);
	playerConf->color = D3DXVECTOR4(colorX, colorY, colorZ, colorW);

	//Position
	TiXmlElement * position = player->FirstChild("position")->ToElement();
	float positionX, positionY, positionZ;
	position->QueryFloatAttribute("x", &positionX);
	position->QueryFloatAttribute("y", &positionY);
	position->QueryFloatAttribute("z", &positionZ);
	playerConf->position = D3DXVECTOR3(positionX, positionY, positionZ);

	//Scale
	TiXmlElement * scale = player->FirstChild("scale")->ToElement();
	scale->QueryFloatAttribute("x", &playerConf->scaleX);
	scale->QueryFloatAttribute("y", &playerConf->scaleY);
	scale->QueryFloatAttribute("z", &playerConf->scaleZ);

	//Texture
	TiXmlElement * texture = player->FirstChild("texture")->ToElement();	
	strcpy(playerConf->texture, texture->Attribute("file"));

	//Mesh
	TiXmlElement * mesh = player->FirstChild("mesh")->ToElement();
	strcpy(playerConf->mesh, mesh->Attribute("file"));

	return playerConf;
}

/** 
 * Uses the Element node to read in the values needed for a base.
 * Then it builds the object, sets the variables, and returns that object.
 */
ObjManagerObjects * ObjManagerConfig::loadBase(TiXmlElement * base) {
	ObjManagerObjects * baseConf = new ObjManagerObjects();

	//Position
	TiXmlElement * position = base->FirstChild("position")->ToElement();
	float positionX, positionY, positionZ;
	position->QueryFloatAttribute("x", &positionX);
	position->QueryFloatAttribute("y", &positionY);
	position->QueryFloatAttribute("z", &positionZ);
	baseConf->position = D3DXVECTOR3(positionX, positionY, positionZ);

	//Scale
	TiXmlElement * scale = base->FirstChild("scale")->ToElement();
	scale->QueryFloatAttribute("x", &baseConf->scaleX);
	scale->QueryFloatAttribute("y", &baseConf->scaleY);
	scale->QueryFloatAttribute("z", &baseConf->scaleZ);

	//Texture
	TiXmlElement * texture = base->FirstChild("texture")->ToElement();	
	strcpy(baseConf->texture, texture->Attribute("file"));

	//Mesh
	TiXmlElement * mesh = base->FirstChild("mesh")->ToElement();
	strcpy(baseConf->mesh, mesh->Attribute("file"));

	return baseConf;
}

/** 
 * Uses the Element node to read in the values needed for an AI player.
 * Then it builds the object, sets the variables, and returns that object.
 */
ObjManagerObjects * ObjManagerConfig::loadAI(TiXmlElement * AI) {
	ObjManagerObjects * AIConf = new ObjManagerObjects();

	//Position
	TiXmlElement * position = AI->FirstChild("position")->ToElement();
	float positionX, positionY, positionZ;
	position->QueryFloatAttribute("x", &positionX);
	position->QueryFloatAttribute("y", &positionY);
	position->QueryFloatAttribute("z", &positionZ);
	AIConf->position = D3DXVECTOR3(positionX, positionY, positionZ);

	//Scale
	TiXmlElement * scale = AI->FirstChild("scale")->ToElement();
	scale->QueryFloatAttribute("x", &AIConf->scaleX);
	scale->QueryFloatAttribute("y", &AIConf->scaleY);
	scale->QueryFloatAttribute("z", &AIConf->scaleZ);

	//Texture
	TiXmlElement * texture = AI->FirstChild("texture")->ToElement();	
	strcpy(AIConf->texture, texture->Attribute("file"));

	//Mesh
	TiXmlElement * mesh = AI->FirstChild("mesh")->ToElement();
	strcpy(AIConf->mesh, mesh->Attribute("file"));

	return AIConf;
}