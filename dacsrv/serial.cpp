#include "serial.h"

using namespace PDT;

CSerial::CSerial()
:m_dev(),m_con()
{
	m_isOpen = false;
}

CSerial::~CSerial()
{
	if ( m_isOpen ) close();
}

//win32写法：COM:Baud:Parity:DataBit:StopBit 如：com1:115200:1:8:1.5
//unix 写法：COM:Baud:Parity:DataBit:StopBit 如：ttyS1:115200:1:8:1.5
int CSerial::open(const char* remoteAddr)
{
	if (m_portStr.length() <= 0)
	{
		if ( !parse(remoteAddr) )
		{
			return -1;
		}
	}

	CString devAddr;
#if defined(WIN32)
	devAddr = CString("\\\\.\\") + m_portStr;	//wfp changed at 201104012
#else
	devAddr = "/dev/" + m_portStr;		//like "/dev/ttyS3"
#endif

	if ( m_con.connect(m_dev,ACE_DEV_Addr(devAddr.c_str())) == -1 )
	{
		close();
		return -1;
	}

	m_serialParams.baudrate	= m_baud;
	m_serialParams.databits	= m_databit;
	m_serialParams.stopbits = m_stopbit;
	m_serialParams.paritymode = m_parityStr.c_str();
	m_serialParams.readtimeoutmsec = 20;	//100ms
	this->setRTS(0);

	if ( m_dev.control (ACE_TTY_IO::SETPARAMS,&m_serialParams) == -1 )
	{
		close();
		return -1;
	}
	m_isOpen = true;

	return 0;
}

bool CSerial::isOpen() const
{
	return m_isOpen;
}

int CSerial::close(void )
{
	m_dev.close();
	m_isOpen = false;
	return 0;
}

int CSerial::read(hUChar* buf,int len)
{
	if ( buf == NULL ) return -1;
	if ( len <= 0) return -1;
	if ( !m_isOpen ) return -1;

	int bytesRead = m_dev.recv (buf,len);

	if ( bytesRead == -1 )
	{
		close();
		return -1;
	}
	return bytesRead;
}

int CSerial::write_n(const hUChar *buf, int len)
{
	if ( buf == NULL ) return -1;
	if ( len <= 0 ) return -1;

	if ( !m_isOpen ) return -1;

	hUInt32 bytesWritten = m_dev.send_n (buf,len);

	if ( bytesWritten != len )
	{
		setRTS(0);
		m_dev.close();
	}
	return bytesWritten;
}

//RTS控制需注意了，m_dev.control (ACE_TTY_IO::SETPARAMS,&m_SP) 方式设置ACE_TTY_IO::Serial_Params m_SP的方式不起作用，
//相反还导致串口数据无法发送！
int CSerial::setRTS(hUChar rtsenb)
{
#if defined(WIN32)
	m_serialParams.rtsenb = rtsenb;
	if (m_dev.control (ACE_TTY_IO::SETPARAMS,&m_serialParams) == -1)
		return -1;
#elif defined(__linux)
	int status;
	ACE_HANDLE handle = m_dev.get_handle( );
	ACE_OS::ioctl(handle, TIOCMGET, &status);
	switch( rtsenb)
	{
	case 0:
		status &= ~TIOCM_RTS;
		break;
	case 1:
		status |= TIOCM_RTS;
		break;
	}
	ACE_OS::ioctl(handle, TIOCMSET, &status);
#endif

	return 0;
}

bool CSerial::parse(const char* remoteAddr)
{
	if ( remoteAddr == NULL ) return false;

	PDT::CString addrString = remoteAddr;
	PDT::CString tmpString;
	int startPos = 0;

	//串口号
	int position = addrString.find(':');
	if ( position == CString::npos ) return false;
	tmpString = addrString.substr(startPos,position-startPos);
	m_portStr = tmpString;

	//波特率
	position += 1;
	startPos = position;
	position = addrString.find(':',position);
	if ( position == CString::npos ) return false;
	tmpString = addrString.substr(startPos,position-startPos);
	m_baud = tmpString.toUInt32();

	//校验位
	position += 1;
	startPos = position;
	position = addrString.find(':',position);
	if ( position == CString::npos ) return false;
	tmpString = addrString.substr(startPos,position-startPos);
	m_parityStr = parityMode( tmpString.toUInt32() );

	//数据位
	position += 1;
	startPos = position;
	position = addrString.find(':',position);
	if ( position == CString::npos ) return false;
	tmpString = addrString.substr(startPos,position-startPos);
	m_databit = tmpString.toUInt32();

	//停止位
	position += 1;
	startPos = position;
	tmpString = addrString.substr(startPos,addrString.length()-startPos);
	m_stopbit = tmpString.toUInt32();

	return true;
}

const char* CSerial::parityMode(hUInt32 parity) const
{
	switch (parity)
	{
	case PARITY_MODE_NONE:
		return "none";
	case PARITY_MODE_ODD:
		return "odd";
	case PARITY_MODE_EVEN:
		return "even";
	case PARITY_MODE_MARK:
		return "mark";
	case PARITY_MODE_SPACE:
		return "space";
	default:
		return "";
	}

	return "";
}




