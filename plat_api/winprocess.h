#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <vdmdbg.h>

int WinKillProc(int pid);
int WinKillProc(const char *name);
int WinGetProcName(int pid,char *procname,int flag=FALSE);
int WinFindProc(const char* name,bool isExcludeSelf=false);

class process_enumerator
{
public:
	virtual int real_show()	{return 1;}
	virtual int GetPid(const char *name,bool isExcludeSelf=false)	{return 0;};
	virtual int GetProcName(int pid,char *procname,int flag=FALSE)	{return FALSE;}
	virtual int Kill(int pid)
	{
		if(pid == 0)	return FALSE;
		HANDLE process;
		process = OpenProcess(PROCESS_TERMINATE, 0, pid);
		if(process == NULL)	return FALSE;
		TerminateProcess(process,(unsigned)-1);
		return TRUE;
	};
	virtual int Kill(const char *name)	{return Kill(GetPid(name));};
};

//Windows NT Functions
typedef BOOL (WINAPI *ENUMPROCESSES)
(
 DWORD * lpidProcess,  // array to receive the process identifiers
 DWORD cb,             // size of the array
 DWORD * cbNeeded      // receives the number of bytes returned
 );

typedef BOOL (WINAPI *ENUMPROCESSMODULES)
(
 HANDLE hProcess,      // handle to the process
 HMODULE * lphModule,  // array to receive the module handles
 DWORD cb,             // size of the array
 LPDWORD lpcbNeeded    // receives the number of bytes returned
 );

typedef DWORD (WINAPI *GETMODULEFILENAME)
( 
 HANDLE hProcess,		// handle to the process
 HMODULE hModule,		// handle to the module
 LPTSTR lpstrFileName,	// array to receive filename
 DWORD nSize			// size of filename array.
 );

typedef DWORD (WINAPI *GETMODULEBASENAME)
( 
 HANDLE hProcess,		// handle to the process
 HMODULE hModule,		// handle to the module
 LPTSTR lpstrFileName,	// array to receive base name of module
 DWORD nSize			// size of module name array.
 );

typedef INT (WINAPI *VDMENUMTASKWOWEX)
(
 DWORD dwProcessId,	// ID of NTVDM process
 TASKENUMPROCEX fp,	// address of our callback function
 LPARAM lparam		// anything we want to pass to the callback function.
);

class NT_process_enumerator :public process_enumerator
{
	enum {max_num = 1024 };
	HANDLE psapi;
	HANDLE vdmdbg;
	ENUMPROCESSES       EnumProcesses;
	GETMODULEFILENAME   GetModuleFileName;
	ENUMPROCESSMODULES  EnumProcessModules;  
	VDMENUMTASKWOWEX	VDMEnumTaskWOWEx;
	GETMODULEBASENAME	GetModuleBaseName;
	virtual int	NT_process_enumerator::real_show();
public:
	NT_process_enumerator();
	~NT_process_enumerator();
	virtual int  GetPid(const char *,bool isExcludeSelf=false);
	virtual int GetProcName(int pid,char *procname,int flag=FALSE);
};

// Win95 functions
typedef BOOL (WINAPI *PROCESSWALK)
(
 HANDLE hSnapshot,    
 LPPROCESSENTRY32 lppe
 );

typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD dwFlags,DWORD th32ProcessID); 

class Win95_enumerator :public process_enumerator
{
	HANDLE kernel;
	HANDLE snapshot;
	PROCESSENTRY32 proc;
	CREATESNAPSHOT CreateToolhelp32Snapshot; 
	PROCESSWALK Process32First; 
	PROCESSWALK Process32Next; 
protected:
	virtual int real_show();
public:
	Win95_enumerator();
	~Win95_enumerator();
	virtual int GetPid(const char *,bool isExcludeSelf=false);	
	//virtual int GetProcName(int pid,char *procname,int flag=FALSE);
};