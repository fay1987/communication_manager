#ifndef _EXP_ANALYZE_H_
#define _EXP_ANALYZE_H_

#include "scd/exp_def.h"
#include <QMap>
#include <QString>
#include <QList>
#include "dac/comminf.h"
#include "dac/datainf.h"

//参数格式符号
#define EXP_PARA_LEFT		'{'
#define EXP_PARA_RIGHT		'}'
#define EXP_TYPE_NAME_SEP	':'

//数据类型
#define EXP_DATA_TYPE_YC	"yc"
#define EXP_DATA_TYPE_YX	"yx"
#define EXP_DATA_TYPE_KWH	"kwh"


//数据获取方式
#define DATAPOOL_SRCMODE_MEMDB		1	//实时库取数据窗口
#define DATAPOOL_SRCMODE_WSERV		2

namespace PDT
{
	//对象及数据
	typedef struct
	{
		QString type;
		QString obj_code;
		QString obj_desc;
		double data;
	}STREAM_DATA_OBJ;

	//公式计算封装
	class EXPCALC_EXPORT CExpAnalyze
	{
	public:
		CExpAnalyze();
		CExpAnalyze(QString exp);
		~CExpAnalyze();

		QString m_error;
	private:
		//原始表达式
		QString m_exp_orig;
		//分析后的表达式
		QString m_exp_real;
		//解释含义的表达式
		QString m_exp_meaning;
		//分析后的变量列表
		QList<STREAM_DATA_OBJ> m_exp_codes;
	public:
		void setExp(QString exp){m_exp_orig=m_exp_meaning=exp;}
		QString exp(){return m_exp_orig;}

		QString exp_real(){return m_exp_real;}
		QString exp_meaning(){return m_exp_meaning;}
		QList<STREAM_DATA_OBJ>& exp_codes(){return m_exp_codes;}
	private:
		//处理表达式中的遥测参数
		bool procAnalog(QString& exp,bool bCalc,bool check=false);
		double getValue(QString type,QString code,bool& isValid);
		QString getParaDesc(QString type,QString code);
	public:
		//分析表达式
		bool analyseExp(bool bCalc=true,bool check=false);
		//计算表达式
		double calc(bool* ok,bool check=false);

	private:
		PDT::CCommInf	m_commInf;
		PDT::CDataInf	m_dataInf;
	};
}

#endif
