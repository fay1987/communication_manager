#ifndef _EXP_CALC_PARSER_H_
#define _EXP_CALC_PARSER_H_

#include "scd/exp_def.h"
#include <complex>
#include <map>
#include <time.h>

using namespace std;

#define PI 3.1415926535897932384626433832795

namespace PDT
{
	//¸´Êý
	class CCompl : public complex<double>
	{
	public:
		CCompl(const double &d1 = 0, const double &d2 = 0) : complex<double>(d1,d2) {}
		CCompl(const complex<double> &in) : complex<double>(in) {}

		operator double()
		{
			return real();
		}
	};
}

//±í´ïÊ½¼ÆËãÆ÷
#define ERR_MSG_LEN	1024
#define MAX_EXP_LEN	2048

#define CALC_ERR_RESULT		-9999

namespace PDT
{
	class EXPCALC_EXPORT CExpCalc 
	{
	private:
		char errText[ERR_MSG_LEN];
		bool m_haserror;

		int Funk(char *p, bool binary, CCompl &x, CCompl &y);
		//½×³Ë
		double Factorial(double x);
		CCompl GetNumber(char *p, int &move);
		//²âÊÔ·ûºÅ
		bool TestDelimiter(char c)
		{
			//wfp add at 20101013 (| &)
			return (c=='+' || c=='-' || c=='*' || c=='/' || c=='|' || c=='&' ||	
					c=='%' || c==',' || c==')' || c==0 );
		}

	public:
		bool gra,newVar,isVar;
		map<string,CCompl> variabler;
		map<string,CCompl>::iterator it;

		CCompl Calculate(char *psz);
		CCompl Calculate(char *p, int len);
		bool havingError(){return m_haserror;}
		char* error(){return errText;}
	};

	class EXPCALC_EXPORT CRandom
	{
	public:
		static unsigned int random();
		static unsigned int random(unsigned int i);
		static float random(float f);
		static unsigned int R_rnd();
		static void R_mize(unsigned int start = time(NULL));
	};
}

#endif
