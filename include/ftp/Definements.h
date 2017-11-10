#ifndef INC_DEFINEMENTS_H
#define INC_DEFINEMENTS_H

#include <string>
#include <set>

//yhh20091118----------------------begin 
#include <ctime>	
#ifdef _DEBUG		
#include <cassert>
#endif

#ifdef WIN32
#ifndef FTPCLIENT_DLL
#define FTPCLIENT_DLL_PORT	_declspec(dllimport)
#else
#define FTPCLIENT_DLL_PORT	_declspec(dllexport)
#endif
#else
#define FTPCLIENT_DLL_PORT
#endif
//---------------------------------end

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/timeb.h>
#include <sys/socket.h>	//wfp add
#include <netinet/in.h>	//wfp add
#include <arpa/inet.h>	//wfp add
 #include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

   typedef char* LPTSTR;
   typedef const char* LPCTSTR;
   typedef int   BOOL;
   typedef unsigned int UINT;
   typedef unsigned int* PUINT;
   typedef char TCHAR;
   typedef const char* LPCSTR;
   typedef unsigned long DWORD;
   typedef unsigned long ULONG;			//wfp add
   typedef unsigned short USHORT;
   typedef unsigned int		SOCKET;		//wfp add
   
#endif

#if defined(WIN32)
#define  EWOULDBLOCK    WSAEWOULDBLOCK
#define  EISCONN        WSAEISCONN
#define  EINPROGRESS    WSAEINPROGRESS
#define  SOCK_EINTR			WSAEINTR
#define	 EALREADY			WSAEALREADY
#elif defined(__unix)
#define  SOCK_EINTR		EINTR
#define  SOCKET				int
#define  SOCKET_ERROR	-1
#define INVALID_SOCKET  (SOCKET)(~0)
#endif

//#define WSAGetLastError() errno

#ifndef VERIFY
#define VERIFY
#endif

#ifndef ASSERT
#ifdef _DEBUG			//yhh20091118
#define ASSERT assert
#else
#define ASSERT
#endif
#endif

// definements for unicode support
#ifdef _UNICODE
   typedef std::wstring tstring;
   typedef std::wostream tostream;
   typedef std::wofstream tofstream;
   typedef std::wostringstream tostringstream;
   typedef std::wistringstream tistringstream;
   typedef std::wstringstream tstringstream;
   typedef std::wstreambuf tstreambuf;

   #define tcin  wcin
   #define tcout wcout
   #define tcerr wcerr
   #define tclog wclog
   #define tcstoul wcstoul
   #define tcschr wcschr
   #define tcsncmp wcsncmp
   #define tisdigit iswdigit
   #define tsprintf swprintf
   #define tcsncpy wcsncpy
   #define ttol wcstol
   #define _sntprintf _snwprintf

   #ifndef _T
   #define _T(x) L ## x
   #endif
#else
   typedef std::string tstring;
   typedef std::ostream tostream;
   typedef std::ofstream tofstream;
   typedef std::ostringstream tostringstream;
   typedef std::istringstream tistringstream;
   typedef std::stringstream tstringstream;
   typedef std::streambuf tstreambuf;

   #define tcin  cin
   #define tcout cout
   #define tcerr cerr
   #define tclog clog
   #define tcstoul strtoul
   #define tcschr strchr
   #define tcsncmp strncmp
   #define tisdigit isdigit
   #define tsprintf sprintf
   #define tcsncpy strncpy
   #define ttol strtol
   #define _sntprintf _snprintf

   #ifndef _T
   #define _T
   #endif
#endif

namespace nsHelper
{
#ifdef WIN32
   /// Class with static functions to get information about an error.
   class FTPCLIENT_DLL_PORT CError
   {
   public:
      static DWORD GetLastError() { return ::GetLastError(); }
   
      static tstring GetErrorDescription(int iErrorCode = GetLastError())
      {
         LPVOID lpMsgBuf=NULL;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, iErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);
         tstring strErrorDescription;
         if( lpMsgBuf )
         {
            strErrorDescription = reinterpret_cast<LPTSTR>(lpMsgBuf);
            LocalFree(lpMsgBuf);
         }
         return strErrorDescription;
      }
   };
#else
   /// Class with static functions to get information about an error.
   class CError
   {
   public:
      static DWORD GetLastError() { return static_cast<DWORD>(-1); }
   
      static tstring GetErrorDescription(int iErrorCode = GetLastError())
      {
         return _T("ToDo: GetErrorDescription not implemented yet!");
      }
   };
#endif

/// Class with static functions to do string conversions.
class FTPCLIENT_DLL_PORT CCnv
{
public:
   static long TStringToLong(const tstring& strIn)
   {
      TCHAR* pStopString = 0;
   #ifdef _UNICODE
      return wcstol(&*strIn.begin(), &pStopString, 10);
   #else
      return strtol(&*strIn.begin(), &pStopString, 10);
   #endif
   }

   static std::string ConvertToString(const tstring& strIn)
   {
      std::string strOut;
      return ConvertToString(strIn, strOut);
   }

   static std::string& ConvertToString(const tstring& strIn, std::string& strOut)
   {
   #ifdef _UNICODE
      if( strIn.size() == 0 )
      {
         strOut.clear();
      }
      else
      {
         strOut.resize(strIn.size());
         wcstombs(&*strOut.begin(), strIn.c_str(), strOut.size());
      }
   #else
      strOut = strIn;
   #endif
      return strOut;
   }

   static tstring ConvertToTString(const char* szIn)
   {
      tstring strOut;
      return ConvertToTString(szIn, strOut);
   }

   static tstring& ConvertToTString(const char* szIn, tstring& strOut)
   {
   #ifdef _UNICODE
      if( strlen(szIn) == 0 )
      {
         strOut.clear();
      }
      else
      {
         strOut.resize(strlen(szIn));
         mbstowcs(&*strOut.begin(), szIn, strOut.size());
      }
   #else
      strOut = szIn;
   #endif
      return strOut;
   }
};

/// Base class for implementing the notification stuff.
/// @remarks Inherit public (instead of private) because it wouldn't compile under Dev-C++
template <typename T, typename T2> class CObserverPatternBase : public std::set<T>
{
public:
   typedef typename std::set<T> base_type;
   typedef typename std::set<T>::iterator iterator;

   ~CObserverPatternBase()
   {
      for( iterator it=base_type::begin(); it!=base_type::end(); it=base_type::begin() )
         (*it)->Detach(static_cast<T2>(this));
   }

   bool Attach(T p)
   {
      if (find(p)!=base_type::end())
         return false;
      insert(p);
      p->Attach(static_cast<T2>(this));
      return true;
   }
   bool Detach(T p)
   {
      if (find(p)==base_type::end())
         return false;
      erase(p);
      p->Detach(static_cast<T2>(this));
      return true;
   }
};

/// @brief Calculates elapsed CPU time.
///
/// Is useful for calculating transferrates.
class FTPCLIENT_DLL_PORT CTimer
{
public:
   CTimer() : m_dfStart(clock()) {}

   /// Restarts the timer.
   void Restart() { m_dfStart = clock(); }

   /// Get the elapsed time in seconds.
   double GetElapsedTime() const
   {
      return (static_cast<double>(clock()) - m_dfStart)/CLOCKS_PER_SEC;
   }

private:
   double m_dfStart; ///< elapsed CPU time for process (start of measurement)
};
}

#endif // INC_DEFINEMENTS_H
