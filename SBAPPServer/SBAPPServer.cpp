// SBAPPServer.cpp: 

#include "stdafx.h"

#include <SBAPPLib.h>

void multithreaded_mode_run();
void overlapped_mode_run();

int main(void)
{
	srand(time(NULL));

	if (overlapped_mode)
	{
		overlapped_mode_run();
	} else {
		multithreaded_mode_run();
	}
	return 0;
}
