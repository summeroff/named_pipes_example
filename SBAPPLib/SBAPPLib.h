// SBAPPLib.h

#pragma once
#include "Stdafx.h"
#include <string>
#include <list>
#include <memory>

#include "someTestClass.h"

const int BUFSIZE = 512;			//code disigned to work with 512 buffers. and will not work with big amount of data. also no check of size in code. 

const bool overlapped_mode = true;   //switch on overlapped mode to use pipe in asynconouse mode
const bool print_pocket_info = false; //print more debug info about processed packet 

namespace SBAPPLib {
	/*
		simple protocol for piped communication
		first byte of packet have to by command_id 
		after it will be data for that command 

		data must have 1 byte as type id
		after it can go data of fixed size for that type( as for sending int )
		or 4 bytes containing size of data and then data themself ( as for sending string)
		it is posible for many data packets to go one after another ( as in sending object or in calling function by object id and function name )
	*/
	enum class proto_command_id : char {
		pci_empty_packet = 0,
		
		pci_send_data = 2,  
		pci_get_objcts_count = 3,
		pci_get_object = 4,
		pci_call_object_function = 5,

		pci_in_next_packet = 8,
		pci_failed = 16,
		pci_unknown = 32
	};

	//type of data sent in buffer 
	enum class proto_value_type : char {
		pvt_int = 8, // 
		pvt_string = 9,
		pvt_object = 10, //packed object of a supported class 
		pvt_unknown = 32
	};

	class PIPELIBRARY_API sbapp_pipe
	{
	public:
		sbapp_pipe(HANDLE _hPipe, std::shared_ptr<someTestClassStorage> _objectStorage);

		static const wchar_t* get_pipe_name();

		//api for sending some data 
		BOOL send_int(const int value);
		BOOL send_string(const std::string & value);
		bool send_object(const std::shared_ptr<someTestClass> value);
		BOOL send_count_request();
		BOOL send_object_request(int object_id);
		BOOL send_function_call(int object_id, const std::string & function_name);

		// get data from pipe and try to process it. 
		BOOL get_command_from_pipe();

		//function for retriving data from buffer
		BOOL process_incomming_command(char *pchRequest, size_t cbBytesRead, int & processed , char* pchReply, DWORD &cbReplyBytes);
		bool process_incomming_data(char *chBuf, size_t byte_read, int& processed);

		static int get_int(char *chBuf, size_t byte_read, int &processed);
		static std::string  get_string(char *chBuf, size_t byte_read, int &processed);
		static std::shared_ptr<someTestClass> get_object(char * chBuf, size_t byte_read, int &processed);

	public:
		static int prepare_object(char* chBuf, const std::shared_ptr<someTestClass> value);
		static int prepare_int(char* chBuf, const int value);
		static int prepare_string(char* chBuf, const std::string& value);
		static char * next_field(char *chBuf);

	private:
		HANDLE hPipe;
		std::shared_ptr<someTestClassStorage> objectsStorage;
		int pipe_id;

	};
 
	

}
