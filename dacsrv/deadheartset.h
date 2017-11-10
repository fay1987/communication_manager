#ifndef	_BFS_UTL_DEADHEART_SET_H_
#define	_BFS_UTL_DEADHEART_SET_H_

#include "utl/datatype.h"
#include "sys/blog.h"
#include "config.h"

#define MAX_HEART_NUM	32			//最多支持的个数
#define MAX_DEAD_HEART	300			//最大dead心跳(秒)
#define LOG_DEAD_HEAR	1530		//错误日志

typedef ACE_Vector<int>	NO_VECTOR;
typedef struct _dead_heart_info_ 
{
	bool			valid;			//有效标志
	hUInt32			count;			
}DEADHEART_INFO;
class  CDeadHeartSet
{
public:
	CDeadHeartSet(){memset(&m_heartInfos,0,sizeof(m_heartInfos));}
	virtual ~CDeadHeartSet(){}

	int				createHeart()
	{
		int pos = validPos();
		if (pos != -1)
		{
			m_heartInfos[pos].valid = true;
			m_heartInfos[pos].count = 0;
			noVecArray[pos].clear();
		}
		return pos;
	}
	void			deleteHeart(int heartid)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;
		m_heartInfos[heartid].valid = false;
	}
	void			addAll()
	{
		for (int i=0;i<MAX_HEART_NUM;i++)
		{
			if ( m_heartInfos[i].valid )
				++m_heartInfos[i].count;
		}
	}
	void			clearCount(int heartid)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;

		if ( !m_heartInfos[heartid].valid ) return;

		m_heartInfos[heartid].count = 0;
	}
	void			addCount(int heartid)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;

		if ( !m_heartInfos[heartid].valid ) return;

		++m_heartInfos[heartid].count;
	}
	bool			isDead()
	{
		for (int i=0;i<MAX_HEART_NUM;i++)
		{
			if ( m_heartInfos[i].valid && m_heartInfos[i].count > MAX_DEAD_HEART )
			{
				showLink(i);
				return true;
			}
		}
		return false;
	}
	void			removeLink(int heartid)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;
		noVecArray[heartid].clear();
	}
	void			addLink(int heartid,int linkNo)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;
		noVecArray[heartid].push_back(linkNo);
	}
private:
	void			showLink(int heartid)
	{
		if ( heartid < 0 || heartid >= MAX_HEART_NUM )  return;
		if ( !CONFIG::instance()->deadCheck() ) return;
		for (hUInt32 i=0;i<noVecArray[heartid].size();i++)
		{
			errprint(LOG_DEAD_HEAR,"规约超时,通道序号:%d",noVecArray[heartid][i]);
		}
	}
	int				validPos()
	{
		for (int i=0;i<MAX_HEART_NUM;i++)
		{
			if ( !m_heartInfos[i].valid ) return i;
		}

		return -1;
	}
	DEADHEART_INFO	m_heartInfos[MAX_HEART_NUM];
	NO_VECTOR		noVecArray[MAX_HEART_NUM];
};

#endif //_BFS_UTL_DEADHEART_SET_H_
