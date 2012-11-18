#ifndef _VAR_PROGRAM
#define _VAR_PROGRAM

#include "program.h"

#define ES_GP 32
#define ES_EVT 33

#define VAR_GP 7
#define VAR_VC 13
#define VAR_HS 14
#define VAR_HS_EWMA 21
#define VAR_HS_GARCH 23
#define VAR_VC_GARCH 15
#define VAR_VC_EWMA 17
#define VAR_VC_GARCH_T 27 
#define VAR_HS_GARCH_T 28
#define VAR_EVT 31
#define COL_VOL 34
#define COL_HURST 35
#define COL_INT_N 36

#define VAR_LEVEL 0.01
#define VAR_COL 9
#define PERCENT 0.6

class VaRProgram : public Program
{
public:
	VaRProgram();
	virtual float Fitness();
	virtual void Setup(std::string directory,vector<std::string> parameters);
	virtual Program *Copy(){Program *p=new VaRProgram;p->SetTrunk(Trunk->Copy());return p;};
	virtual void Write(std::string directory,unsigned int generation);
	~VaRProgram();

	static float sigma_tot,mu_tot,max_tot,min_tot;
	static float avg_vol;
	static float var_evt,x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,var_vc,var_hs,var_vc_garch,var_hs_garch,var_hs_garcht,var_vc_garcht,var_vc_ewma,var_hs_ewma,trend,curvature,mean,open,close,volume,high,low;
	static Matrix m,n,evt;
	static float garch_ssr;
	float ssr;
	float cash;
private:
protected:
};


#endif