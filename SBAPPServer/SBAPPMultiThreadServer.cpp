#include "stdafx.h"

#include <SBAPPLib.h>

DWORD WINAPI InstanceThread(LPVOID);

struct client_connection_data
{
	HANDLE hPipe;
	std::shared_ptr<someTestClassStorage> objectsStorage;
};

void multithreaded_mode_run()
{
	HANDLE hPipe;
	std::shared_ptr<someTestClassStorage> objectsStorage = std::make_shared<someTestClassStorage>();

	while (true)
	{
		hPipe = CreateNamedPipe(SBAPPLib::sbapp_pipe::get_pipe_name(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 1024 * 16, 1024 * 16, NMPWAIT_USE_DEFAULT_WAIT, nullptr);
		if (hPipe == INVALID_HANDLE_VALUE)
			break;

		if (ConnectNamedPipe(hPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			client_connection_data new_client_data;
			new_client_data.hPipe = hPipe;
			new_client_data.objectsStorage = objectsStorage;

			HANDLE hThread = NULL;
			DWORD  dwThreadId = 0;
			hThread = CreateThread(NULL, 0, InstanceThread, (LPVOID)&new_client_data, 0, &dwThreadId);

			if (hThread == NULL)
			{
				fprintf(stdout, "CreateThread failed, GLE=%d.\n", GetLastError());
				return;
			} else {
				CloseHandle(hThread);
			}

		} else {
			fprintf(stdout, "cannot conenct to pipe \n");
			CloseHandle(hPipe);
		}
	}
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
	BOOL fSuccess = FALSE;
	HANDLE hPipe = ((client_connection_data*)lpvParam)->hPipe;
	SBAPPLib::sbapp_pipe server_pipe(hPipe, ((client_connection_data*)lpvParam)->objectsStorage);

	fprintf(stdout, "Pipe client connected \n");
	while (1)
	{
		
		// this call get data packet from pipe process it and send reply to client 
		fSuccess = server_pipe.get_command_from_pipe();

		if (!fSuccess)
		{
			fprintf(stdout, "Pipe client disconnected \n");
			break;
		}
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return 1;
}