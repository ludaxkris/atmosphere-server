#pragma once
#include "Game.h"
#include "connection.h"
#include "./GraphicsManager/GraphicsManager.h"
//#include <windows.h>

class GameHandler
{
	CRITICAL_SECTION m_CriticalSection;


public:


	void* NICKS_GRAPIHCS;
	//IDirect3DDevice9*	pd3dDevice;
	GraphicsManager*	gManager;


	int debug;
	//arrays of each game class that people are currently playing
	//current game class that people are loading up
	Game** Game_solo;
	Game** Game_1v1;
	Game** Game_2v2;
	Game** Game_4FFA;

	//cur games that need to be filled with players still
	int Cur_solo;
	int Cur_1v1;
	int Cur_2v2;
	int Cur_4FFA;

	//returns 1 if we start the game
	//returns 0 if we need more people before we start the game.
	Game* joinGame(int gameType, SOCKET* curClient, int* pPlayerID);

	//returns 1 for sucess or 0 for failure
	int disconnect(Game* game, SOCKET* clientSocket);

	GameHandler(void);
	~GameHandler(void);
};
