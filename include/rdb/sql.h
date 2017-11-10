/*==============================================================================
* �ļ����� : field.h
* ģ�� : ���ݼ�¼
* ����ʱ�� : 2008-04-28 15:55:23
* ���� : ����ƽ
* �汾 : v1.0

* �޸ĺ�汾 : 1.1
* �޸�ժҪ : 
* �޸��� :  
* �޸����� : 2007��7��25��
==============================================================================*/
#ifndef	_PDT_DATABASE_DBSQL_H_
#define	_PDT_DATABASE_DBSQL_H_

#include "utl/variant.h"
#include "utl/string.h"
#include "ace/Vector_T.h"
#include "rdb/db_export.h"
//---------------------------------------------------------------------------------------
namespace	PDT
{
	typedef struct _column_info
	{
		CString		colName;  //�ֶ���
		CVariant	val;      //ֵ
		bool		useflag;  //��ʶ���ֶ��Ƿ�ʹ��
	}ColInfo;

	typedef struct _where_info
	{
		CString		name;	  //�ֶ����ƻ�"("��")"��"AND"
		int			cmp;      //�Ƚϲ�����
		CVariant	val;      //ֵ
		const	CVariant	*pval;    //ָ�����ڰ�
		bool		opflag;   //���ṹ�Ƿ��ǲ������ı�ʶ
		bool		bindflag; //�Ƿ�󶨱�ʶ

		_where_info()
		{
			val = CVariant::null;
			pval = NULL;
			opflag = false;
			bindflag = false;
		}
	}WhereInfo;
	
	//order by no,name(���ܻ���ֶ���ֶε�order by)
	typedef struct _order_info
	{
		CString		name;
		CString		sort;  //asc,dsc,��
		bool		useflag;
	}OrderInfo;

	typedef ACE_Vector<ColInfo>		VEC_COLINFO;
	typedef ACE_Vector<WhereInfo>	VEC_WHEREINFO;
	typedef ACE_Vector<OrderInfo>	VEC_ORDERINFO;

	class DB_EXPORT CSql
	{
	public:
		//��������
		enum	Operate
		{
			OP_None = 0,
			OP_Select = 1,
			OP_Insert = 2,
			OP_Update = 3,
			OP_Delete = 4,
			OP_CALL	= 5,			//yhh20090827
			OP_EXEC = 6				//yhh20090827
		};
		enum	Compare
		{
			CP_Equal = 0,		//����
			CP_NotEqual,		//������
			CP_Greater,			//����
			CP_GreaterEqual,	//���ڵ���
			CP_Less,			//С��
			CP_LessEqual		//С�ڵ���
		};
		enum	Logic
		{
			LG_Not = 0,			//��
			LG_And,				//��
			LG_Or				//��
		};
		enum	Sort
		{
			S_Defult = 0,		//Ĭ��
			S_Asc,				//����
			S_Desc				//����
		};

	public:
		CSql();
		virtual	~CSql();
	public:
		//�ͷŶ���
		virtual	void	destroy();
		//�����������
		virtual void	clear();
		//���ò������ͣ�insert��update��delete��select��
		virtual bool	setOperate(Operate op);
		//���ñ���
		virtual bool	setTableName(const char *tableName);
		//----------------------------------------------------------
		//SELECT DISTINCT
		virtual void	selectDistinct(bool dist=true);
		//����SELECT�ֶ�
		virtual bool	selectField(const char *fieldName);
		//ɾ��SELECT�ֶ�
		virtual bool	deSelectField(const char *fieldName);
		//��������SELECT�ֶ�
		virtual bool	selectAllField();
		//ɾ������SELECT�ֶ�
		virtual bool	deSelectAllField();
		//----------------------------------------------------------
		//����UPDATE�ֶ�
		virtual bool	updateField(const char *fieldName,const CVariant &value);
		//ɾ��UPDATE�ֶ�
		virtual bool	deUpdateField(const char *fieldName);
		//----------------------------------------------------------
		//����INSERT�ֶ�
		virtual bool	insertField(const char *fieldName,const CVariant &value);
		//ɾ��INSERT�ֶ�
		virtual bool	deInsertField(const char *fieldName);
		//----------------------------------------------------------
		//����WHERE�ֶ�
		virtual bool	whereField(const char *fieldName,const CVariant &value,Compare cmp=CP_Equal,bool bindFlag=false);
		//����WHERE������
		virtual bool	whereLeftBracket();
		//����WHERE������
		virtual bool	whereRightBracket();
		//����WHERE�߼�
		virtual bool	whereLogic(Logic logic=LG_And);
		//ɾ��WHERE��
		virtual bool	whereClear();
		//----------------------------------------------------------
		//����OEDER�ֶ�
		virtual bool	orderField(const char *fieldName,Sort sort=S_Defult);  //asc&desc������
		//ɾ��OEDER�ֶ�
		virtual bool	deOrderField(const char *fieldName);
		//----------------------------------------------------------
		//����GROUP�ֶ�
		virtual bool	groupField(const char *fieldName);
		//ɾ��GROUP�ֶ�
		virtual bool	deGroupField(const char *fieldName);
		//----------------------------------------------------------
		virtual CString sql() const;
		virtual const CString &tableName() const;
		virtual CSql::Operate operateType(const CString &sql) const;  
		virtual CSql::Operate operateType() const;
		virtual CString operateTypeName(const CString &sql) const;
		virtual CString operateTypeName() const;

	protected: //���º����밴��sql�﷨��������
		virtual CString makeSelectSql() const; //��select����
		virtual CString makeInsertSql() const; //��insert����
		virtual CString makeUpdateSql() const; //��update����
		virtual CString makeDeleteSql() const; //��delete����
		virtual CString makeWhereSql()  const; //��where �Ӿ�
		virtual CString makeOrderSql()  const; //��order by �Ӿ�

		virtual CString valueToString(const CVariant& value) const;
	protected:
		void init();		  //��մ�����Ӿ�
		bool addField(const char* fieldName,const CVariant& value=CVariant::null);
		bool removeField(const char* fieldName);
	protected:
		VEC_COLINFO m_vecColumn; //�ֶ���Ϣ
		VEC_WHEREINFO m_vecWhere;//where��Ϣ
		VEC_ORDERINFO m_vecOrder;//����order by ���ֶ���Ϣ
		CString		m_tableName; //����
		Operate		m_opType;    //��������
		CString		m_group;//���ڶ���group by "field"

		bool		m_distinctFlag;  //
		bool		m_selectAllFlag; //ȫѡ��־.
		
	};
};
//---------------------------------------------------------------------------------------

#endif
