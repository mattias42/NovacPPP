// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// #ifndef VC_EXTRALEAN
// #define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
// #endif
// 
// // Modify the following defines if you have to target a platform prior to the ones specified below.
// // Refer to MSDN for the latest info on corresponding values for different platforms.
// #ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
// #define WINVER 0x0A00		// Windows 10.
// #endif
// 
// #ifndef _WIN32_WINNT		 // Allow use of features specific to Windows NT 4 or later.
// #define _WIN32_WINNT 0x0A00  // Windows 10
// #endif						
// 
// #ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
// #define _WIN32_WINDOWS 0x0A00  // Windows 10
// #endif

// #ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
// #define _WIN32_IE 0x0A00	// Change this to the appropriate value to target IE 5.0 or later.
// #endif

// #define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
// #define _AFX_ALL_WARNINGS

// Disable the extra security warnings on windows
#define _CRT_SECURE_NO_WARNINGS

// MFC Replacements, copy-paste from https://msdn.microsoft.com/en-us/library/aa383751(VS.85).aspx
#define CONST const
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef DWORD LCID;
typedef int BOOL;
typedef char TCHAR;
typedef char CHAR;
typedef CONST CHAR *LPCSTR;
typedef LPCSTR LPCTSTR;
typedef void* PVOID;
typedef void* LPVOID;
typedef PVOID HANDLE;
typedef unsigned int UINT;
typedef unsigned char BYTE;
#define MAX_PATH 260

#define TRUE true
#define FALSE false

// #define IN 
// #define WINAPI __stdcall


// MFC headers, commented out to remove MFC dependency
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT
//#include <afxdlgs.h>
