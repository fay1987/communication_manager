#ifndef _EXP_ANALYZE_H_
#define _EXP_ANALYZE_H_

#include "scd/exp_def.h"
#include <QMap>
#include <QString>
#include <QList>
#include "dac/comminf.h"
#include "dac/datainf.h"

//������ʽ����
#define EXP_PARA_LEFT		'{'
#define EXP_PARA_RIGHT		'}'
#define EXP_TYPE_NAME_SEP	':'

//��������
#define EXP_DATA_TYPE_YC	"yc"
#define EXP_DATA_TYPE_YX	"yx"
#define EXP_DATA_TYPE_KWH	"kwh"


//���ݻ�ȡ��ʽ
#define DATAPOOL_SRCMODE_MEMDB		1	//ʵʱ��ȡ���ݴ���
#define DATAPOOL_SRCMODE_WSERV		2

namespace PDT
{
	//��������
	typedef struct
	{
		QString type;
		QString obj_code;
		QString obj_desc;
		double data;
	}STREAM_DATA_OBJ;

	//��ʽ�����װ
	class EXPCALC_EXPORT CExpAnalyze
	{
	public:
		CExpAnalyze();
		CExpAnalyze(QString exp);
		~CExpAnalyze();

		QString m_error;
	private:
		//ԭʼ���ʽ
		QString m_exp_orig;
		//������ı��ʽ
		QString m_exp_real;
		//���ͺ���ı��ʽ
		QString m_exp_meaning;
		//������ı����б�
		QList<STREAM_DATA_OBJ> m_exp_codes;
	public:
		void setExp(QString exp){m_exp_orig=m_exp_meaning=exp;}
		QString exp(){return m_exp_orig;}

		QString exp_real(){return m_exp_real;}
		QString exp_meaning(){return m_exp_meaning;}
		QList<STREAM_DATA_OBJ>& exp_codes(){return m_exp_codes;}
	private:
		//������ʽ�е�ң�����
		bool procAnalog(QString& exp,bool bCalc,bool check=false);
		double getValue(QString type,QString code,bool& isValid);
		QString getParaDesc(QString type,QString code);
	public:
		//�������ʽ
		bool analyseExp(bool bCalc=true,bool check=false);
		//������ʽ
		double calc(bool* ok,bool check=false);

	private:
		PDT::CCommInf	m_commInf;
		PDT::CDataInf	m_dataInf;
	};
}

#endif
