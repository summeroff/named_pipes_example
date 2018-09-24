#pragma once

#include <string>
#include <list>
#include <mutex>

namespace SBAPPLib {
	class sbapp_pipe;
}

#ifdef PIPELIBRARY_EXPORTS  
#define PIPELIBRARY_API __declspec(dllexport)   
#else  
#define PIPELIBRARY_API __declspec(dllimport)   
#endif  

#include "SBAPPpipedClass.h"

class PIPELIBRARY_API someTestClass : public pipedClass
{
	friend SBAPPLib::sbapp_pipe;
public:
	std::string Name;
	std::string City;
	int cost;

	void resetCost() { cost = count * points; }
	void setInfo(int _count);
private:
	int count;
	int points;

	void generatePoints() { points = rand() % 256; };

public:
	virtual void print() const;
	virtual bool call_function(const std::string & function_name) ;
	virtual int pack_for_pipe(char* chBuf);
	virtual int unpack_from_pipe(char * chBuf, int & processed);
};

class PIPELIBRARY_API someTestClassStorage
{
public:
	int getCount() {
		std::lock_guard<std::mutex> lock(changeList);
		return someTestObjectsList.size();
	}

	std::shared_ptr<pipedClass> getObject(int id) {
		std::lock_guard<std::mutex> lock(changeList);
		std::shared_ptr<pipedClass> work_object;

		for (auto iter = someTestObjectsList.begin(); iter != someTestObjectsList.end(); iter++)
		{
			id--;
			if (id < 0)
			{
				work_object = *iter;
			}
		}
		return work_object;
	}
	void add(std::shared_ptr<pipedClass> new_item) {
		std::lock_guard<std::mutex> lock(changeList);
		someTestObjectsList.push_back(new_item);
	}

protected:
	std::list<std::shared_ptr<pipedClass>> someTestObjectsList;
	std::mutex changeList;
};