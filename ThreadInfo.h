#pragma once


#include "Game.h"

#include "GameHandler.h"
	/**************************************************************************
	*					ThreadInfo
	*
	*	Description: information passed to each thread
	*			used to bypass the void pointer when a thread is created
	*			but used a different method.  Still used because it doesn't
	*			look that bad and it isn't broken.
	*
	*
	**************************************************************************/
class ThreadInfo 
{

public:
	~ThreadInfo(void);

	SOCKET* ClientSocket;

	//mutex for locking all the threads when needed.
	GameHandler* GH;

    ThreadInfo(SOCKET* CS, GameHandler* inGH); 
};
