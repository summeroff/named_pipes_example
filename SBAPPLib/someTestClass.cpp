#include "Stdafx.h"
#include "someTestClass.h"
#include "SBAPPLib.h"

void someTestClass::print() const
{
	fprintf(stdout, "someClass[ cost:%i, count:%i, points:%i, Name:\"%s\", City:\"%s\" ] \n", cost, count, points, Name.c_str(), City.c_str());
}

void someTestClass::setInfo(int _count)
{
	count = _count;
	generatePoints();
}

bool someTestClass::call_function(const std::string & function_name)
{
	bool ret = true;
	if (function_name.compare("generatePoints") == 0)
	{
		generatePoints();
	}
	else if(function_name.compare("resetCost") == 0)
	{
		resetCost();
	}
	else if(function_name.compare("setInfo") == 0)
	{
		setInfo(0);
	}
	else {
		ret = false;
	}
	return ret;
}

int someTestClass::pack_for_pipe(char* chBuf)
{
	int chLen = 0;
	chLen += SBAPPLib::sbapp_pipe::prepare_int(chBuf + chLen, cost);
	chLen += SBAPPLib::sbapp_pipe::prepare_int(chBuf + chLen, count);
	chLen += SBAPPLib::sbapp_pipe::prepare_int(chBuf + chLen, points);
	chLen += SBAPPLib::sbapp_pipe::prepare_string(chBuf + chLen, Name);
	chLen += SBAPPLib::sbapp_pipe::prepare_string(chBuf + chLen, City);

	return chLen;
}

int someTestClass::unpack_from_pipe(char * chBuf, int & processed)
{
	cost = SBAPPLib::sbapp_pipe::get_int(chBuf, 0, processed);
	chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);
	count = SBAPPLib::sbapp_pipe::get_int(chBuf, 0, processed);
	chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);
	points = SBAPPLib::sbapp_pipe::get_int(chBuf, 0, processed);
	chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);

	Name = SBAPPLib::sbapp_pipe::get_string(chBuf, 0, processed);
	chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);
	City = SBAPPLib::sbapp_pipe::get_string(chBuf, 0, processed);
	chBuf = SBAPPLib::sbapp_pipe::next_field(chBuf);
	
	return 0;
}