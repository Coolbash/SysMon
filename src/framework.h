// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
//#include <malloc.h>
#include <memory.h>

//windows unicode support:
#include <tchar.h>
#include <string>
#include <sstream>
using tstring = std::basic_string<TCHAR>;
using tostringstream = std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> >;
