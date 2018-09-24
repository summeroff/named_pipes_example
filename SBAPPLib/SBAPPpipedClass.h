#pragma once

#include <string>

#ifdef PIPELIBRARY_EXPORTS  
#define PIPELIBRARY_API __declspec(dllexport)   
#else  
#define PIPELIBRARY_API __declspec(dllimport)   
#endif  

/*
	Some kind of interface. 
	Class have to implement this to be usable for pipe transmission
*/
class PIPELIBRARY_API pipedClass
{
public:
	virtual void print() const = 0;
	
	/*
		function may call some class methods by function_name
	*/
	virtual bool call_function(const std::string & function_name) = 0;
	
	/*
		function must puts all class members to a char buffer 
		return have to be size of data put in buffer
	*/
	virtual int pack_for_pipe(char* chBuf) = 0;

	/*
		function must members from buffer 
		- processed - have to be count of bites taken from buffer
	*/
	virtual int unpack_from_pipe(char * chBuf, int & processed) = 0;
};
