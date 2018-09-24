#include "stdafx.h"

#include <SBAPPLib.h>

const DWORD CONNECTING_STATE = 0;
const DWORD READING_STATE = 1;
const DWORD WRITING_STATE = 2;
const int INSTANCES = 4;
const int PIPE_TIMEOUT = 5000;

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;

	char chRequest[BUFSIZE];
	DWORD cbRead;

	char chReply[BUFSIZE];
	DWORD cbToWrite;

	DWORD dwState;
	BOOL fPendingIO;

	std::shared_ptr<SBAPPLib::sbapp_pipe> server_pipe;
} PIPEINST, *LPPIPEINST;

BOOL connect_to_client(HANDLE hPipe, LPOVERLAPPED lpo);
VOID disconnect_and_reconnect(PIPEINST & Pipe);


void overlapped_mode_run()
{
	std::shared_ptr<someTestClassStorage> objectsStorage = std::make_shared<someTestClassStorage>();

	PIPEINST Pipe[INSTANCES];
	HANDLE hEvents[INSTANCES];
	DWORD i, dwWait, cbRet, dwErr;
	BOOL fSuccess;

	for (i = 0; i < INSTANCES; i++)
	{
		fprintf(stdout, "CreateEvent \n");
		hEvents[i] = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		if (hEvents[i] == nullptr)
		{
			fprintf(stdout, "CreateEvent failed with %d.\n", GetLastError());
			return;
		}

		ZeroMemory(&Pipe[i].oOverlap, sizeof(OVERLAPPED));
		Pipe[i].oOverlap.hEvent = hEvents[i];

		Pipe[i].hPipeInst = CreateNamedPipe(SBAPPLib::sbapp_pipe::get_pipe_name(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, INSTANCES, BUFSIZE * sizeof(char), BUFSIZE * sizeof(char), PIPE_TIMEOUT, nullptr);

		if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE)
		{
			fprintf(stdout, "CreateNamedPipe failed with %d.\n", GetLastError());
			return;
		}

		Pipe[i].server_pipe = std::make_shared<SBAPPLib::sbapp_pipe>(Pipe[i].hPipeInst, objectsStorage);
		Pipe[i].fPendingIO = connect_to_client(Pipe[i].hPipeInst, &Pipe[i].oOverlap);
		Pipe[i].dwState = Pipe[i].fPendingIO ? CONNECTING_STATE : READING_STATE;
	}

	while (true)
	{
		dwWait = WaitForMultipleObjects(INSTANCES, hEvents, FALSE, INFINITE);

		i = dwWait - WAIT_OBJECT_0;
		if (i < 0 || i >(INSTANCES - 1))
		{
			fprintf(stdout, "Index %i out of range.\n", i);
			return;
		}

		ResetEvent(hEvents[i]);

		if (Pipe[i].fPendingIO)
		{
			fprintf(stdout, "Index %i in pending for state %i.\n", i, Pipe[i].dwState);

			fSuccess = GetOverlappedResult(Pipe[i].hPipeInst, &Pipe[i].oOverlap, &cbRet, TRUE);
			switch (Pipe[i].dwState)
			{
			case CONNECTING_STATE:
				if (!fSuccess)
				{
					printf("Error %d.\n", GetLastError());
					return;
				}
				Pipe[i].dwState = READING_STATE;
				break;
			case READING_STATE:
				if (!fSuccess || cbRet == 0)
				{
					disconnect_and_reconnect(Pipe[i]);
					continue;
				}
				Pipe[i].cbRead = cbRet;
				Pipe[i].dwState = WRITING_STATE;
				break;
			case WRITING_STATE:
				if (!fSuccess || cbRet != Pipe[i].cbToWrite)
				{
					disconnect_and_reconnect(Pipe[i]);
					continue;
				}
				Pipe[i].dwState = READING_STATE;
				break;
			default:
			{
				fprintf(stdout, "Invalid pipe state.\n");
				return;
			}
			}
		}

		switch (Pipe[i].dwState)
		{
		case READING_STATE:
			fSuccess = ReadFile(Pipe[i].hPipeInst, Pipe[i].chRequest, BUFSIZE * sizeof(char), &Pipe[i].cbRead, &Pipe[i].oOverlap);
			fprintf(stdout, "Read Pipe[i].cbRead %i. fsuccess %i, %d \n", Pipe[i].cbRead, fSuccess, GetLastError());
			if (fSuccess && Pipe[i].cbRead != 0)
			{
				Pipe[i].fPendingIO = FALSE;
				Pipe[i].dwState = WRITING_STATE;
				continue;
			}

			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING))
			{
				Pipe[i].fPendingIO = TRUE;
				continue;
			}

			disconnect_and_reconnect(Pipe[i]);
			break;

		case WRITING_STATE:
		{
			int processed = 0;
			Pipe[i].server_pipe->process_incomming_command(Pipe[i].chRequest, Pipe[i].cbRead, processed, Pipe[i].chReply, Pipe[i].cbToWrite);
			if (Pipe[i].cbToWrite > 0)
			{
				fSuccess = WriteFile(Pipe[i].hPipeInst, Pipe[i].chReply, Pipe[i].cbToWrite, &cbRet, &Pipe[i].oOverlap);

				if (fSuccess && cbRet == Pipe[i].cbToWrite)
				{
					Pipe[i].fPendingIO = FALSE;
					Pipe[i].dwState = READING_STATE;
					continue;
				}

				dwErr = GetLastError();
				if (!fSuccess && (dwErr == ERROR_IO_PENDING))
				{
					Pipe[i].fPendingIO = TRUE;
					continue;
				}

				disconnect_and_reconnect(Pipe[i]);
			}
			else {
				Pipe[i].fPendingIO = FALSE;
				Pipe[i].dwState = READING_STATE;
			}
		}
		break;

		default:
		{
			fprintf(stdout, "Invalid pipe state.\n");
			return;
		}

		}
	}
}

VOID disconnect_and_reconnect(PIPEINST & Pipe)
{
	if (!DisconnectNamedPipe(Pipe.hPipeInst))
	{
		fprintf(stdout, "DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	Pipe.fPendingIO = connect_to_client(Pipe.hPipeInst, &Pipe.oOverlap);
	Pipe.dwState = Pipe.fPendingIO ? CONNECTING_STATE : READING_STATE;
}

BOOL connect_to_client(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;

	fConnected = ConnectNamedPipe(hPipe, lpo);

	if (fConnected)
	{
		fprintf(stdout, "ConnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}
	else {
		fprintf(stdout, "ConnectNamedPipe ok \n");
	}

	fprintf(stdout, "ConnectNamedPipe gle %i \n", GetLastError());

	switch (GetLastError())
	{
	case ERROR_IO_PENDING: fPendingIO = TRUE; break;
	case ERROR_PIPE_CONNECTED: if (SetEvent(lpo->hEvent)) break;
	default:
	{
		fprintf(stdout, "ConnectNamedPipe failed with %d.\n", GetLastError());
		return FALSE;
	}
	}

	return fPendingIO;
}
