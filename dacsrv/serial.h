#ifndef	_PDT_DAC_SERIAL_H_
#define _PDT_DAC_SERIAL_H_

#include "ace/DEV_Addr.h"
#include "ace/DEV_Connector.h"
#include "ace/TTY_IO.h"
#include <ace/OS.h>
#include "utl/string.h"

//reactor ��ʽ���ܿ���ͨ���¼��ź���reactor����������ƽ̨�Ƚ��鷳��Ҳ��ά��
//proactor ��ʽû�г���
//�ɿ���ace_task�̳߳ط�ʽ��ר�Ŵ�����ͨѶ.
//CSerial����Ҫ�����ṩ���ڿ������رգ����ݷ��ͣ����ݽ��գ���ʵ�����£�
class CSerial
{
public:

	//У��ģʽ
	/** Specifies the parity mode. POSIX supports "none", "even" and
	"odd" parity. Additionally Win32 supports "mark" and "space"
	parity modes. */
	enum PARITY_MODE
	{
		PARITY_MODE_NONE		=	0,
		PARITY_MODE_ODD			=	1,
		PARITY_MODE_EVEN		=	2,
		PARITY_MODE_MARK		=	3,
		PARITY_MODE_SPACE		=	4
	};
public:
	CSerial();
	virtual ~CSerial();
public:
	virtual int		open(const char* remoteAddr);
	virtual bool	isOpen() const;
	virtual int		close();
public:
	int				read(hUChar* buf,int len);
	int				write_n(const hUChar* buf,int len);	//���ݷ���
	int				setRTS(hUChar rtsenb);				//RTS�ź�����

	PDT::CString	getPortStr(){return m_portStr;}	

protected:
	bool			parse(const char* remoteAddr);

private:
	const char*		parityMode(hUInt32 parity) const; 
private:
	ACE_TTY_IO			m_dev; 
	ACE_DEV_Connector	m_con;
	ACE_TTY_IO::Serial_Params m_serialParams;
	bool				m_isOpen;
	//���ڲ���
	PDT::CString		m_portStr;
	hUInt32				m_baud;
	PDT::CString		m_parityStr;
	hUInt32				m_databit;
	hUInt32				m_stopbit;
};

#endif
