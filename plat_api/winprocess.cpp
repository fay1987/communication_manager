#include "winprocess.h"
#include "sys/sysmacro.h"

Win95_enumerator::Win95_enumerator():kernel(GetModuleHandle("KERNEL32.DLL")) 
{
	if(kernel == NULL)
	{
		printf("kernel is not loaded\n");
		return;
	}
	CreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress((HINSTANCE)kernel,"CreateToolhelp32Snapshot"); 
	Process32First = (PROCESSWALK)GetProcAddress((HINSTANCE)kernel,"Process32First"); 
	Process32Next  = (PROCESSWALK)GetProcAddress((HINSTANCE)kernel,"Process32Next"); 
	if (CreateToolhelp32Snapshot==NULL || Process32First==NULL || Process32Next==NULL)	return;
	proc.dwSize = sizeof(proc);
}

int Win95_enumerator::real_show() 
{
	if(CreateToolhelp32Snapshot==NULL || Process32First==NULL || Process32Next==NULL)	return -1;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	printf("    name           pid\n");
	Process32First(snapshot, &proc);
	printf("     %s       %x\n",proc.szExeFile, proc.th32ProcessID);
	while (Process32Next(snapshot, &proc) == TRUE)
		printf("     %s       %x\n",proc.szExeFile, proc.th32ProcessID);
	CloseHandle(snapshot);
	return 0;
}

/*
* 函数功能:根据进程名获取进程ID
* 函数说明:支持不添加后缀的ID获取(如"CCC","CCC.exe"都支持)
* 函数返回:-1--错误
*/
int Win95_enumerator::GetPid(const char *name,bool isExcludeSelf/*=false*/)
{
	char file_name[MAX_PATH];
	char src_name[MAX_PATH];
	strcpy(src_name,name);
	if(CreateToolhelp32Snapshot==NULL || Process32First==NULL || Process32Next==NULL)	return -1;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	Process32First(snapshot, &proc);
	strcpy(file_name,proc.szExeFile);
	char *p1;
	char *p=strstr(file_name,".");
	if(p) *p=0;
	p = strstr(src_name,".");
	if(p) *p=0;
	p=file_name;
	for(;;)
	{
		p1=strstr(p+1,"\\");
		if(p1)
		{
			p=p1;
			continue;
		}
		p1=strstr(p+1,"/");
		if(p1)
		{
			p=p1;
			continue;
		}
		break;
	}
	if(!_stricmp(p,src_name))
	{
		CloseHandle(snapshot);
		return proc.th32ProcessID;
	}
	while (Process32Next(snapshot, &proc) == TRUE)
	{
		strcpy(file_name,proc.szExeFile);
		p=strstr(file_name,".");
		if(p) *p=0;
		p=file_name;
		for(;;)
		{
			p1=strstr(p+1,"\\");
			if(p1)
			{
				p=p1;
				continue;
			}
			p1=strstr(p+1,"/");
			if(p1)
			{
				p=p1;
				continue;
			}
			break;
		}
		if(!_stricmp(p+1,src_name))
		{
			//过滤自身的pid
			if ( isExcludeSelf && (::GetCurrentProcessId () == proc.th32ProcessID) ) continue; 
			printf("filename=%s pid=%x\n",src_name,proc.th32ProcessID);
			CloseHandle(snapshot);
			return proc.th32ProcessID;
		}
	}
	CloseHandle(snapshot);
	return -1;
}

NT_process_enumerator::NT_process_enumerator()
{
	psapi = LoadLibrary("PSAPI.DLL");
	vdmdbg = LoadLibrary("VDMDBG.DLL");
	if(psapi==NULL || vdmdbg==NULL)	return;
	VDMEnumTaskWOWEx = (VDMENUMTASKWOWEX)GetProcAddress((HINSTANCE)vdmdbg, "VDMEnumTaskWOWEx");
	EnumProcesses =(ENUMPROCESSES)GetProcAddress((HINSTANCE)psapi, "EnumProcesses");
	GetModuleFileName = (GETMODULEFILENAME)GetProcAddress((HINSTANCE)psapi, "GetModuleFileNameExA");
	GetModuleBaseName = (GETMODULEBASENAME)GetProcAddress((HINSTANCE)psapi, "GetModuleBaseNameA");
	EnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress((HINSTANCE)psapi, "EnumProcessModules");
	if(VDMEnumTaskWOWEx==NULL || EnumProcesses==NULL || GetModuleFileName==NULL || GetModuleBaseName==NULL || EnumProcessModules==NULL)	return;
}

NT_process_enumerator::~NT_process_enumerator()
{
	if(psapi)	FreeLibrary((HINSTANCE)psapi);
	if(vdmdbg)	FreeLibrary((HINSTANCE)vdmdbg);	
}

int NT_process_enumerator::real_show()
{
	DWORD process_ids[max_num];
	DWORD num_processes;
	if(VDMEnumTaskWOWEx==NULL || EnumProcesses==NULL || GetModuleFileName==NULL || GetModuleBaseName==NULL || EnumProcessModules==NULL)	return -1;
	int success = EnumProcesses(process_ids,sizeof(process_ids),&num_processes);
	num_processes /= sizeof(process_ids[0]);
	if ( !success )
	{
		printf("Enumerating Processes\n");
		return -1;
	}  
	printf("         name        pid\n");
	for ( unsigned i=0; i<num_processes; i++)
	{
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,process_ids[i]);
		HMODULE modules[max_num];
		DWORD num_modules;
		char file_name[MAX_PATH];
		EnumProcessModules(process,modules,sizeof(modules),&num_modules);
		num_modules /= sizeof(modules[0]);
		if (GetModuleFileName(process,modules[0],file_name,sizeof(file_name))) 
		{
			GetModuleBaseName(process,modules[0],file_name,sizeof(file_name));
			printf("        %s        %d\n",file_name, process_ids[i]);
		}
		CloseHandle(process);
	}
	return 0;
}

/*
 * 函数功能:根据进程名获取进程ID
 * 函数说明:支持不添加后缀的ID获取(如"CCC","CCC.exe"都支持)
 * 函数返回:-1--错误
 */
int NT_process_enumerator::GetPid(const char *name,bool isExcludeSelf/*=false*/)
{
	DWORD process_ids[max_num];
	DWORD num_processes;
	if(VDMEnumTaskWOWEx==NULL || EnumProcesses==NULL || GetModuleFileName==NULL || GetModuleBaseName==NULL || EnumProcessModules==NULL)	return -1;
	int success = EnumProcesses(process_ids,sizeof(process_ids),&num_processes);
	num_processes /= sizeof(process_ids[0]);
	if ( !success )
	{
		printf("Enumerating Processes\n");
		return -1;
	}
	for ( unsigned i=0; i<num_processes; i++)
	{
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,process_ids[i]);
		HMODULE modules[max_num];
		DWORD num_modules;
		char file_name[MAX_PATH];
		char src_name[MAX_PATH];
		strcpy(src_name,name);
		EnumProcessModules(process,modules,sizeof(modules),&num_modules);
		num_modules /= sizeof(modules[0]);
		if (GetModuleFileName(process,modules[0],file_name,sizeof(file_name)))
		{
			GetModuleBaseName(process,modules[0],file_name,sizeof(file_name));
			char *p=strstr(file_name,".");
			if(p) *p=0;
			p = strstr(src_name,".");
			if(p) *p=0;
			if(!_stricmp(file_name,src_name))
			{
				//过滤自身的pid
				if ( isExcludeSelf && (::GetCurrentProcessId () == process_ids[i]) ) continue; 
				//printf("filename=%s pid=%d\n",src_name,process_ids[i]);
				CloseHandle(process);
				return process_ids[i];
			}			
		}
		CloseHandle(process);
	}
	return -1;
}

int NT_process_enumerator::GetProcName(int pid,char *procname,int flag)
{
	if(VDMEnumTaskWOWEx==NULL || EnumProcesses==NULL || GetModuleFileName==NULL || GetModuleBaseName==NULL || EnumProcessModules==NULL)	return -1;
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
	if(process)
	{
		HMODULE hMod;
		DWORD cbNeeded;
		if(EnumProcessModules(process,&hMod,sizeof(hMod),&cbNeeded))
		{
			char proc_name[MAX_PATH];
			if(flag == TRUE)	//包含路径的文件名
				GetModuleFileName(process,hMod,proc_name,sizeof(proc_name));
			else				//不包含路径的文件名
				GetModuleBaseName(process,hMod,proc_name,sizeof(proc_name));
			strncpy(procname,proc_name,SYS_PROCNAME_LEN);		//wfp changed at 20101118
			return 0;
		}
	}
	return -1;
}

int WinKillProc(int pid)
{
	HANDLE process;
	process = OpenProcess(PROCESS_TERMINATE, 0, pid);
	if(!process) return -1;
	TerminateProcess(process, (unsigned)-1);
	return 0;
}

static process_enumerator *processes=NULL;

int WinKillProc(const char *name)
{
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	GetVersionEx(&info);
	if(!processes)
	{
		if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
			processes = new Win95_enumerator();
		else if ( info.dwPlatformId == VER_PLATFORM_WIN32_NT )
			processes = new NT_process_enumerator();
	}
	if(!processes)	return -1;
	return processes->Kill(name);
}

int WinGetProcName(int pid,char *procname,int flag)
{
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	GetVersionEx(&info);
	if(!processes)
	{
		if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
			processes = new Win95_enumerator();
		else if ( info.dwPlatformId == VER_PLATFORM_WIN32_NT )
			processes = new NT_process_enumerator();
	}
	if(!processes)	return -1;
	return processes->GetProcName(pid,procname,flag);
}

int WinFindProc(const char* name,bool isExcludeSelf/*=false*/)
{
	if ( name == NULL ) return -1;

	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	GetVersionEx(&info);
	if(!processes)
	{
		if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
			processes = new Win95_enumerator();
		else if ( info.dwPlatformId == VER_PLATFORM_WIN32_NT )
			processes = new NT_process_enumerator();
	}
	if(!processes)	return -1;
	if ( processes->GetPid(name,isExcludeSelf) == -1 ) return -1;
	return 0;
}
