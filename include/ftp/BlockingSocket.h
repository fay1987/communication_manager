////////////////////////////////////////////////////////////////////////////////
//
// From David J. Kruglinski (Inside Visual C++).
//
////////////////////////////////////////////////////////////////////////////////

// removed baseclass CObject
// removed baseclass CException
// removed all LPCTSTR LPTSTR

// needs winsock.h or afxsock.h in the precompiled headers and the following
// statement in InitInstance
// if(!AfxSocketInit())
// {
//    AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
//    return FALSE;
// }

#ifndef INC_BLOCKINGSOCKET_H
#define INC_BLOCKINGSOCKET_H

#include <string>
#include "Definements.h"
#include "utl/datatype.h"
#include "utl/string.h"

namespace nsSocket
{

typedef const struct sockaddr* LPCSOCKADDR;

#ifdef __unix
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr *LPSOCKADDR;
typedef struct sockaddr_in *LPSOCKADDR_IN;
#endif

///////////////////////////////////////////////////////////////////////////////////////
//***************************** CBlockingSocketException  ***************************//
///////////////////////////////////////////////////////////////////////////////////////
class FTPCLIENT_DLL_PORT CBlockingSocketException
{
public:
   // Constructor
   CBlockingSocketException(LPTSTR pchMessage);
   
public:
   ~CBlockingSocketException() {}
   virtual bool GetErrorMessage(char* lpstrError, hUInt32 nMaxError, hUInt32* pnHelpContext = NULL);
   virtual BFS::CString GetErrorMessage(hUInt32* pnHelpContext = NULL);
   
private:
   int			m_nError;
   BFS::CString m_strMessage;
};

///////////////////////////////////////////////////////////////////////////////////////
//*********************************** CSockAddr  ************************************//
///////////////////////////////////////////////////////////////////////////////////////
class FTPCLIENT_DLL_PORT CSockAddr
{
public:
   // constructors
   CSockAddr()
   {
      m_addr.sin_family        = AF_INET; // specifies address family
      m_addr.sin_port          = 0;       // ip port
      m_addr.sin_addr.s_addr   = 0;       // ip address
   }
   
   CSockAddr(const SOCKADDR& sa)
            { memcpy(&m_addr, &sa, sizeof(SOCKADDR)); }
   
   CSockAddr(const SOCKADDR_IN& sin) 
            { memcpy(&m_addr, &sin, sizeof(SOCKADDR_IN)); }
   
   CSockAddr(ULONG ulAddr, hUInt32 ushPort = 0) // parms are host byte ordered
            { 
               m_addr.sin_family      = AF_INET;          // specifies address family
               m_addr.sin_port        = htons(ushPort);   // ip port
               m_addr.sin_addr.s_addr = htonl(ulAddr);    // ip address
            }
   
   CSockAddr(const char* pchIP, hUInt32 ushPort = 0) // dotted IP addr string
            { 
               m_addr.sin_family = AF_INET;               // specifies address family
               m_addr.sin_port = htons(ushPort);          // ip port
               m_addr.sin_addr.s_addr = inet_addr(pchIP); // ip address
            } // already network byte ordered
   
   // Return the address in dotted-decimal format
   BFS::CString DottedDecimal()
      { return inet_ntoa(m_addr.sin_addr); }

   // Get port and address (even though they're public)
   hUInt32 Port() const
            { return ntohs(m_addr.sin_port); }
   
   ULONG IPAddr() const
            { return ntohl(m_addr.sin_addr.s_addr); }
   
   // operators added for efficiency
   CSockAddr& operator=(const SOCKADDR& sa)
            {
               memcpy(&m_addr, &sa, sizeof(SOCKADDR));
               return *this;
            }

   CSockAddr& operator=(const SOCKADDR_IN& sin)
            { 
               memcpy(&m_addr, &sin, sizeof(SOCKADDR_IN));
               return *this; 
            }

   operator SOCKADDR()
            { return *((LPSOCKADDR) this); }

   operator LPSOCKADDR()
            { return (LPSOCKADDR) this; }

   operator LPSOCKADDR_IN()
            { return (LPSOCKADDR_IN) this; }
private:
	struct sockaddr_in	m_addr;
};

///////////////////////////////////////////////////////////////////////////////////////
//********************************* CBlockingSocket  ********************************//
///////////////////////////////////////////////////////////////////////////////////////

class FTPCLIENT_DLL_PORT IBlockingSocket
{
public:
   virtual ~IBlockingSocket() {};
   virtual IBlockingSocket* CreateInstance() const = 0;
   virtual void Create(int nType = SOCK_STREAM) = 0;
   virtual void Connect(LPCSOCKADDR psa) const = 0;
   virtual void Bind(LPCSOCKADDR psa) const = 0;
   virtual void Listen() const = 0;
   virtual void Cleanup() = 0;
   virtual bool Accept(IBlockingSocket& s, LPSOCKADDR psa) const = 0;
   virtual void Close() = 0;
   virtual int  Write(const char* pch, int nSize, int nSecs) const = 0;
   virtual int  Receive(char* pch, int nSize, int nSecs) const = 0;
   virtual void GetSockAddr(LPSOCKADDR psa) const = 0;
   virtual operator SOCKET() const = 0; //+#
   virtual bool CheckReadability() const = 0;
   virtual CSockAddr   GetHostByName(const char* pchName, hUInt32 ushPort = 0) = 0;
   virtual const char* GetHostByAddr(LPCSOCKADDR psa) = 0;
};

// member functions truly block and must not be used in UI threads
// use this class as an alternative to the MFC CSocket class
class FTPCLIENT_DLL_PORT CBlockingSocket : public IBlockingSocket
{
public:
   CBlockingSocket() :
      m_hSocket(0) {}

   virtual IBlockingSocket* CreateInstance() const;

   void Cleanup();
   void Create(int nType = SOCK_STREAM);
   void Close();

   void Bind(LPCSOCKADDR psa) const;
   void Listen() const;
   void Connect(LPCSOCKADDR psa) const;
   bool Accept(IBlockingSocket& s, LPSOCKADDR psa) const;
   int  Send(const char* pch, int nSize, int nSecs) const;
   int  Write(const char* pch, int nSize, int nSecs) const;
   int  Receive(char* pch, int nSize, int nSecs) const;
   int  SendDatagram(const char* pch, int nSize, LPCSOCKADDR psa, int nSecs) const;
   int  ReceiveDatagram(char* pch, int nSize, LPSOCKADDR psa, int nSecs) const;
   void GetPeerAddr(LPSOCKADDR psa) const;
   void GetSockAddr(LPSOCKADDR psa) const;
   bool CheckReadability() const;

   CSockAddr   GetHostByName(const char* pchName, hUInt32 ushPort = 0);
   const char* GetHostByAddr(LPCSOCKADDR psa);

   operator SOCKET() const { return m_hSocket; }

private:
   SOCKET m_hSocket;
};

FTPCLIENT_DLL_PORT IBlockingSocket* CreateDefaultBlockingSocketInstance();

///////////////////////////////////////////////////////////////////////////////////////
//******************************** CHttpBlockingSocket ******************************//
///////////////////////////////////////////////////////////////////////////////////////
class FTPCLIENT_DLL_PORT CHttpBlockingSocket : public CBlockingSocket
{
public:
   enum {nSizeRecv = 1000}; // max receive buffer size (> hdr line length)

   CHttpBlockingSocket();
   ~CHttpBlockingSocket();

   int ReadHttpHeaderLine(char* pch, int nSize, int nSecs);
   int ReadHttpResponse(char* pch, int nSize, int nSecs);

private:
   char*  m_pReadBuf; // read buffer
   int    m_nReadBuf; // number of bytes in the read buffer
};

};

#endif // INC_BLOCKINGSOCKET_H
