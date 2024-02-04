// header.h : include file for standard system include files,
// or project specific include files
//
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


// C++
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <iomanip>

// My lib
#include "Lib/MD5.h"
#include "Lib/SHA-256.h"
#include "Lib/AES_Encryption.h"
#include "Lib/RC4_Encryption.h"
#include "Lib/XOR_Encryption.h"