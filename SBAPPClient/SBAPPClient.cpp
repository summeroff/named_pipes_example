// SBAPPClient.cpp: 

#include "stdafx.h"
#include <SBAPPLib.h>
#include "time.h"
#include "synchapi.h"

int main(void)
{
	srand(time(NULL));

	HANDLE hPipe;
	BOOL fSuccess = TRUE;
	
	//connect to pipe
	while (true)
	{
		hPipe = CreateFile(SBAPPLib::sbapp_pipe::get_pipe_name(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		
		if (hPipe != INVALID_HANDLE_VALUE)
			break;
		
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			fprintf(stdout, "Error: cannot open pipe :\"%s\", GLE = %d \n", SBAPPLib::sbapp_pipe::get_pipe_name(), GetLastError());
			return -1;
		}

		if (!WaitNamedPipe(SBAPPLib::sbapp_pipe::get_pipe_name(), 1000))
		{
			fprintf(stdout, "Could not open pipe: 1 second wait timed out\n");
		}
		
	}

	if (hPipe != INVALID_HANDLE_VALUE)
	{
		DWORD dwMode = PIPE_READMODE_BYTE | PIPE_TYPE_BYTE;
		fSuccess = SetNamedPipeHandleState( hPipe, &dwMode, NULL, NULL);

		printf("Connected to pipe. Start exchange data.\n");

		SBAPPLib::sbapp_pipe client_pipe(hPipe, nullptr);

		//send some data and commands to pipe 
		if (fSuccess) fSuccess = client_pipe.send_int(rand() % 123456);
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		if (fSuccess) fSuccess = client_pipe.send_string("SomeString");
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		std::shared_ptr<someTestClass> test = std::make_shared<someTestClass>();
		test->cost = 12345;
		test->Name = "TestName";
		test->City = "CityNameName";
		test->setInfo(333);

		if (fSuccess) fSuccess = client_pipe.send_object(test);
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		test->cost = 543210;
		test->Name = "AnotherName";
		test->City = "MoreCityName";
		test->setInfo(111);

		if (fSuccess) fSuccess = client_pipe.send_object(test);
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		printf("Sleep for 3sec before send next commands to server\n");
		Sleep(3000);

		if (fSuccess) fSuccess = client_pipe.send_count_request();
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		if (fSuccess) fSuccess = client_pipe.send_object_request(1);
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		if (fSuccess) fSuccess = client_pipe.send_function_call(1, "generatePoints");
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		if (fSuccess) fSuccess = client_pipe.send_function_call(1, "resetCost");
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();

		printf("Sleep for 3sec before send next commands to server\n");
		Sleep(3000);

		if (fSuccess) fSuccess = client_pipe.send_object_request(1);
		if (fSuccess && !overlapped_mode) fSuccess = client_pipe.get_command_from_pipe();
		
		//this only for overlapped mode. client get from pipe all server answers. 
		if (fSuccess && overlapped_mode)
		{
			printf("In overlapped mode , get all server answers.\n");
			Sleep(1000);
			if (fSuccess) fSuccess = client_pipe.get_command_from_pipe();
		}

		CloseHandle(hPipe);
	}
	else {
		fprintf(stdout, "Error: cannot open pipe :\"%s\", GLE = %d \n", SBAPPLib::sbapp_pipe::get_pipe_name(), GetLastError());
	}

	return 0;
}
