#ifndef _ZJUALG_H_
#define _ZJUALG_H_


#define	ALG_CONFIG_FILE		"fcalc.ini"

#define Pw_rated 25.0//60.0
#define Pc_lim  15.0//80.0
#define Pb_Fe_Li_lim 20.0//70.0
#define Tf 100.0//10.0
#define Tf_max1 200.0//20.0    //discharge
#define Tf_max2 200.0//20.0    //charge
#define Tf2 1200.0//100.0
#define lim_1m 0.02
#define lim_30m 0.07


namespace PDT
{
	//����������
	typedef struct
	{
		float	sc_soc;				//In��������SOC
		float	lb_soc;				//In﮵��SOC
		float	sc_actually_p;		//In��������ʵ���й�����
		float	lb_actually_p;		//In﮵��ʵ���й�����
		float	pv_total_p;			//In������й����� PV
		float	wind_total_p;		//In������й����� Wind
	}InputRealTimeData,*InputRealTimeData_ptr;
	
	//���
	typedef struct
	{
		float	sc_out_p;			//Out���������й��������
		float	lb_out_p;			//Out﮵�ع����й��������
	}OutputRealTimeData,*OutputRealTimeData_ptr;

	enum AlgorithmType
	{
		ZJU		= 0;		//�㽭��ѧ�㷨
		DGS					//�繤���㷨
	};

	class CConfigInfo
	{
	public:
		void	init();
		int		PreparingData();
		//ȡֵ����
		float	sc_soc(){return input_rtdata_.sc_soc;}
		float	lb_soc(){return input_rtdata_.lb_soc;}
		float	sc_actually_p(){return input_rtdata_.sc_actually_p;}
		float	lb_actually_p(){return input_rtdata_.lb_actually_p;}
		float	pv_total_p(){return input_rtdata_.pv_total_p;}
		float	wind_total_p(){return input_rtdata_.wind_total_p;}
		//��ֵ����
		void	set_sc_out_p(float val);
		void	set_lb_out_p(float val);

	private:
		bool	initflag;

		int		Alg_type_;

		int		sc_soc_grp_;
		int		sc_soc_point_;
		int		lb_soc_grp_;
		int		lb_soc_point_;
		int		sc_actually_p_grp_;
		int		sc_actually_p_point_;
		int		lb_actually_p_grp_;
		int		lb_actually_p_point_;
		int		pv_total_p_grp_;
		int		pv_total_p_point_;
		int		wind_total_p_grp_;
		int		wind_total_p_point_;

		int		sc_out_p_grp_;
		int		sc_out_p_point_;
		int		lb_out_p_grp_;
		int		lb_out_p_point_;

		InputRealTimeData	input_rtdata_;
		OutputRealTimeData	ouput_data_;

		PDT::CCommInf	m_commInf;
		PDT::CDataInf	m_dataInf;
	};

	//��ʽ�����װ
	class ALGORITHM_EXPORT CComputePLB
	{
	public:
		CComputePLB();
		~CComputePLB();

	private:
		CConfigInfo	m_ci;

	public:
		int mainCompute();

		int init();
	};
}

#endif	//_ZJUALG_H_
