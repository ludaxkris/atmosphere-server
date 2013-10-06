#include "stdafx.h"
#include "threadinfo.h"

    ThreadInfo::ThreadInfo(SOCKET* CS, GameHandler* inGH)
    {
		GH = inGH;
		ClientSocket = CS;
    }


ThreadInfo::~ThreadInfo(void)
{
}
