// ֳכאגםûי DLL-פאיכ.

#include "stdafx.h"

#include "SBAPPLib.h"

const wchar_t* PipeName = TEXT("\\\\.\\pipe\\SB_NamedPipe");

namespace SBAPPLib {
	sbapp_pipe::sbapp_pipe(HANDLE _hPipe, std::shared_ptr<someTestClassStorage> _objectStorage) :hPipe(_hPipe), objectsStorage(_objectStorage)
	{
		pipe_id = rand() % 65535;
	}

	BOOL sbapp_pipe::send_int(const int value)
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_command_id::pci_send_data);
		chLen += sizeof(proto_command_id);

		chLen += prepare_int(chBuf + chLen, value);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info)
			fprintf(stdout, "%i: Sent int: %i, packet size : %i \n",pipe_id , value, dwWritten);
		else 
			fprintf(stdout, "%i: Sent int: %i  \n", pipe_id, value);
		return ret;
	}
	
	BOOL sbapp_pipe::send_count_request()
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_command_id::pci_get_objcts_count);
		chLen += sizeof(proto_command_id);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info)
			fprintf(stdout, "%i: Sent objects count request, packet size : %i \n", pipe_id, dwWritten);
		else 
			fprintf(stdout, "%i: Sent objects count request  \n", pipe_id);

		return ret;
	}
	
	BOOL sbapp_pipe::send_object_request(int object_id)
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_command_id::pci_get_object);
		chLen += sizeof(proto_command_id);

		chLen += prepare_int(chBuf + chLen, object_id);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info)
			fprintf(stdout, "%i: Sent objects request for id %i, packet size : %i \n", pipe_id, object_id, dwWritten);
		else 
			fprintf(stdout, "%i: Sent objects request for id %i \n", pipe_id, object_id);
		return ret;
	}

	BOOL sbapp_pipe::send_function_call(int object_id, const std::string & function_name)
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_command_id::pci_call_object_function);
		chLen += sizeof(proto_command_id);

		chLen += prepare_int(chBuf + chLen, object_id);
		chLen += prepare_string(chBuf + chLen, function_name);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info) 
			fprintf(stdout, "%i: Sent funciont call request for id %i, function %s, packet size : %i \n", pipe_id, object_id, function_name.c_str(), dwWritten);
		else 
			fprintf(stdout, "%i: Sent funciont call request for id %i, function %s \n", pipe_id, object_id, function_name.c_str());

		return ret;
	}

	int sbapp_pipe::get_int(char *chBuf, size_t byte_read, int &processed)
	{
		int ret = 0;
		if (chBuf[0] == static_cast<int>(proto_value_type::pvt_int))
		{
			ret = *((int*)(chBuf + 1));
			processed += 1 + sizeof(int);
		} else {
			
		}

		return ret;
	}
	BOOL sbapp_pipe::send_string(const std::string & value)
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;		

		chBuf[0] = static_cast<char>(proto_command_id::pci_send_data);
		chLen += sizeof(proto_command_id);

		chLen += prepare_string(chBuf + chLen, value);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info)
			fprintf(stdout, "%i: Sent string: \"%s\", packet size : %i \n", pipe_id, value.c_str(), dwWritten);
		else 
			fprintf(stdout, "%i: Sent string: \"%s\" \n", pipe_id, value.c_str());
		return ret;
	}


	std::string  sbapp_pipe::get_string(char *chBuf, size_t byte_read, int &processed)
	{
		std::string ret = "";
		if (chBuf[0] == static_cast<int>(proto_value_type::pvt_string))
		{
			size_t str_size = *((size_t*)(chBuf + 1));

			ret = std::string(chBuf + 1 + sizeof(size_t), str_size);
			processed += 1 + sizeof(size_t) + str_size + 1;
		}

		return ret;
	};

	char * sbapp_pipe::next_field(char *chBuf)
	{
		char * ret = nullptr;
		switch (static_cast<proto_value_type> (chBuf[0]))
		{
		case proto_value_type::pvt_int:
			ret = chBuf + 1 + sizeof(int);
			break;
		case proto_value_type::pvt_string:
		{
			size_t str_size = *((size_t*)(chBuf + 1));
			ret = chBuf + 1 + sizeof(size_t) + str_size + 1;
		}
			break;
		case proto_value_type::pvt_object:
			static_assert(true, "many objects in data packet is not supported yet");
			break;
		default:
			static_assert(true, "unsuported data type id");
		}
		return ret;
	}

	bool sbapp_pipe::send_object(const std::shared_ptr<someTestClass> value)
	{
		DWORD dwWritten;
		char  chBuf[BUFSIZE];
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_command_id::pci_send_data);
		chLen += sizeof(proto_command_id);

		chLen += prepare_object(chBuf + chLen, value);

		BOOL ret = WriteFile(hPipe, chBuf, chLen, &dwWritten, NULL);
		FlushFileBuffers(hPipe);

		if (print_pocket_info)
			fprintf(stdout, "%i: Sent object packet size : %i , ", pipe_id, dwWritten);
		else 
			fprintf(stdout, "%i: Sent object: ", pipe_id);
		value->print();

		return ret;
	}

	std::shared_ptr<someTestClass> sbapp_pipe::get_object(char *chBuf, size_t byte_read, int &processed)
	{
		std::shared_ptr<someTestClass> ret = std::make_shared<someTestClass>();
		if (chBuf[0] == static_cast<int>(proto_value_type::pvt_object))
		{
			processed += 1;
			chBuf = chBuf + 1;
			
			ret->unpack_from_pipe(chBuf, processed);

		}

		return ret;
	};

	int  sbapp_pipe::prepare_int(char* chBuf, const int value)
	{
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_value_type::pvt_int);
		chLen = sizeof(proto_value_type);

		int * data_pointer = (int*)&chBuf[chLen];
		data_pointer[0] = value;
		chLen += sizeof(int);

		return chLen;
	}

	int  sbapp_pipe::prepare_string(char* chBuf, const std::string& value)
	{
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_value_type::pvt_string);
		chLen =  sizeof(proto_value_type);

		size_t * data_size_pointer = (size_t*)&chBuf[chLen];
		data_size_pointer[0] = value.size();
		chLen += sizeof(size_t);

		char* data_pointer = chBuf + chLen;
		memcpy(data_pointer, value.c_str(), value.size());
		chLen += value.size() + 1;
		chBuf[chLen - 1] = 0x00;

		return chLen;
	}


	int sbapp_pipe::prepare_object(char* chBuf, const std::shared_ptr<someTestClass> value)
	{
		int chLen = 0;

		chBuf[0] = static_cast<char>(proto_value_type::pvt_object);
		chLen += sizeof(proto_value_type);

		chLen += value->pack_for_pipe(chBuf + chLen);

		return chLen;
	}

	BOOL sbapp_pipe::get_command_from_pipe()
	{
		BOOL fSuccess = FALSE;
		DWORD cbBytesRead = 0;
		char pchRequest[BUFSIZE];
		char pchReply[BUFSIZE];
		DWORD cbReplyBytes = 0, cbWritten = 0;

		fSuccess = ReadFile(hPipe, pchRequest, BUFSIZE * sizeof(char), &cbBytesRead, NULL);
		if (print_pocket_info)
			fprintf(stdout, "%i: Recieved packet : size %i , id %i \n", pipe_id, cbBytesRead, cbBytesRead > 0 ? pchRequest[0] : 0);

		if (!fSuccess || cbBytesRead == 0)
		{
			fSuccess = FALSE;
		} else {
			char * chBuf = pchRequest;
			while (true)
			{
				if (print_pocket_info)
					fprintf(stdout, "%i: chBuf at [0][1] = %02X %02X\n", pipe_id, chBuf[0], chBuf[1]);
				int processed = 0;
				fSuccess = process_incomming_command(chBuf, cbBytesRead, processed, pchReply, cbReplyBytes);
				if (print_pocket_info)
					fprintf(stdout, "processed bytes = %i\n", processed);
				if (fSuccess && cbReplyBytes > 0)
				{
					fSuccess = WriteFile(hPipe, pchReply, cbReplyBytes, &cbWritten, NULL);
					FlushFileBuffers(hPipe);
				}
				if (processed >0 && processed < cbBytesRead)
				{
					chBuf += processed;
					cbBytesRead -= processed;
				} else { break; }
			}
		}
		
		return fSuccess;
	}

	BOOL sbapp_pipe::process_incomming_command(char *pchRequest, size_t cbBytesRead, int & processed ,char* pchReply, DWORD &cbReplyBytes)
	{
		BOOL fSuccess = TRUE;
		processed = 0;
		pchReply[0] = 0;
		cbReplyBytes = 0;

		switch (static_cast<SBAPPLib::proto_command_id>(pchRequest[0]))
		{
		case SBAPPLib::proto_command_id::pci_send_data:
			{
				processed = 1;
				auto ret = process_incomming_data(pchRequest + 1, cbBytesRead - 1, processed);
				if (ret)
				{
					pchReply[0] = 0;
					cbReplyBytes = 1;
				}
			}
			break;
		case SBAPPLib::proto_command_id::pci_get_objcts_count:
			pchReply[0] = static_cast<char>(proto_command_id::pci_send_data);
			cbReplyBytes += sizeof(proto_command_id);

			cbReplyBytes += prepare_int(pchReply + cbReplyBytes, objectsStorage->getCount());
			processed = 1;
			break;
		case SBAPPLib::proto_command_id::pci_failed:
			pchReply[0] = static_cast<char>(proto_command_id::pci_empty_packet);
			cbReplyBytes += sizeof(proto_command_id);
			processed = 1;
			fprintf(stdout, "%i: Recieved Failed as an answer\n", pipe_id);
			break;
		case SBAPPLib::proto_command_id::pci_empty_packet:
			pchReply[0] = static_cast<char>(proto_command_id::pci_empty_packet);
			cbReplyBytes = 0;
			processed = 1;
			if (print_pocket_info)
				fprintf(stdout, "%i: Recieved empty packet as an answer\n", pipe_id);
			break;
		case SBAPPLib::proto_command_id::pci_get_object:
		{
			processed = 1;
			int recieved_int = SBAPPLib::sbapp_pipe::get_int(pchRequest + 1, cbBytesRead - 1, processed);
			fprintf(stdout, "%i: Recieved request for object id:\"%i\"\n", pipe_id, recieved_int);

			std::shared_ptr<someTestClass> found_object = std::static_pointer_cast<someTestClass> ( objectsStorage->getObject(recieved_int));

			if (found_object != nullptr)
			{
				pchReply[0] = static_cast<char>(proto_command_id::pci_send_data);
				cbReplyBytes += sizeof(proto_command_id);
				cbReplyBytes += prepare_object(pchReply + cbReplyBytes, found_object);
			}
			else {
				pchReply[0] = static_cast<char>(proto_command_id::pci_failed);
				cbReplyBytes += sizeof(proto_command_id);
			}
		}
			break;
		case SBAPPLib::proto_command_id::pci_call_object_function:
		{
			processed = 1;
			char * chBuf = pchRequest + 1;
			int recieved_id = SBAPPLib::sbapp_pipe::get_int(chBuf, cbBytesRead - 1, processed);
			chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);
			std::string function_name = SBAPPLib::sbapp_pipe::get_string(chBuf, cbBytesRead - 1, processed);

			fprintf(stdout, "%i: Recieved function call for object id:\"%i\", function %s\n", pipe_id, recieved_id, function_name.c_str());

			std::shared_ptr<someTestClass> work_object = std::static_pointer_cast<someTestClass> (objectsStorage->getObject(recieved_id));

			if (work_object != nullptr)
			{
				bool call_ret = work_object->call_function(function_name);
				if (call_ret)
				{
					pchReply[0] = static_cast<char>(proto_command_id::pci_empty_packet);
					cbReplyBytes += sizeof(proto_command_id);
				}
				else {
					pchReply[0] = static_cast<char>(proto_command_id::pci_failed);
					cbReplyBytes += sizeof(proto_command_id);
				}
			}
			else {
				pchReply[0] = static_cast<char>(proto_command_id::pci_failed);
				cbReplyBytes += sizeof(proto_command_id);
			}
		}
			break;
		}
		
		return fSuccess;
	}
	
	bool sbapp_pipe::process_incomming_data(char *chBuf, size_t byte_read, int &processed)
	{
		bool ret = false;
		switch (static_cast<SBAPPLib::proto_value_type>(chBuf[0]))
		{
		case SBAPPLib::proto_value_type::pvt_int:
		{
			int recieved_int = SBAPPLib::sbapp_pipe::get_int(chBuf, byte_read, processed);
			fprintf(stdout, "%i: Recieved int :\"%i\"\n", pipe_id, recieved_int);
			ret = true;
		}
		break;
		case SBAPPLib::proto_value_type::pvt_string:
		{
			std::string revieved_string = SBAPPLib::sbapp_pipe::get_string(chBuf, byte_read, processed);
			fprintf(stdout, "%i: Recieved str :\"%s\"\n", pipe_id, revieved_string.c_str());
			ret = true;
		}
		break;
		case SBAPPLib::proto_value_type::pvt_object:
		{
			std::shared_ptr<someTestClass> revieved_object = SBAPPLib::sbapp_pipe::get_object(chBuf, byte_read, processed);
			fprintf(stdout, "%i: Recieved : ", pipe_id);
			revieved_object->print();
			if (objectsStorage != nullptr)
			{
				ret = true; // we server so we can answer to client about recieving 
				objectsStorage->add(revieved_object);
			}
		}
		break;
		};

		return ret;
	}

	const wchar_t * sbapp_pipe::get_pipe_name()
	{
		return PipeName;
	}
}