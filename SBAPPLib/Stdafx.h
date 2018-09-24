// stdafx.h 

#pragma once

#ifdef PIPELIBRARY_EXPORTS  
#define PIPELIBRARY_API __declspec(dllexport)   
#else  
#define PIPELIBRARY_API __declspec(dllimport)   
#endif  

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>