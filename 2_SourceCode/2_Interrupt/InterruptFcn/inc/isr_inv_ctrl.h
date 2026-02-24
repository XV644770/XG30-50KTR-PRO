#ifndef SOLAR_INVCTRL_H              			// Prevents multiple inclusion of this file.
#define SOLAR_INVCTRL_H
#define RECTRL_POINTS		384//144//96//Switch_Freq/Reat_Freq

typedef struct
{
	int16	wVolt_d;					// the d value of Grid Voltage
	int16 	wVolt_q;					// the q value of Grid Voltage
	int16	wVolt_dFilter;				// the d value of Grid Voltage
	int16 	wVolt_qFilter;				// the q value of Grid Voltage
	int16	wCurr_d;					// the d value of Inv Current
	int16 	wCurr_q;					// the q value of Inv Current
}ST_INV_PARA;

typedef struct
{
	struct
	{
		int32	dVoltHighR_Kd;
       	 	int32	dVoltFstDiff_Kd;
        	int32	dVoltSecDiff_Kd;
	}stIn;

	struct
	{
		int16	wForwardVolt_d;
       	 	int16	wForwardVolt_q;
	}stOut;
}ST_INV_VOLT_CTRL;

typedef struct
{
	struct
	{
		int16	wCurr_d;
		int16	wCurr_q;
		int32	dCurr_Kd;
		int32	dCurr_Kpi;
		int32	dCurr_Kq;

		Uint16 	ReCtrl_Enable;					// Voltage Loop VoltSoftStart_Flag.
		int16 	ReCtrl_Id[RECTRL_POINTS];		// Repeat control save array.
		int16	ReCtrl_Iq[RECTRL_POINTS];		// Repeat control save array.
		int16	ReCtrl_Pos;						// Repeat control save position.
		int16 	ReCtrl_Cur;						// Repeat control current point.
		int16 	ReCtrl_Val_Id;					// Repeat control Value.
		int16 	ReCtrl_Val_Iq;					// Repeat control Value.
		int16 	Rectrl_Points;	
		Uint16	dReCtrl_KPR;
		Uint16	dReCtrl_KPR1;
		Uint16	dReCtrl_KPR2;
		
	
	}stIn;

	struct
	{
		int16	wOutCurr_d;
       	 int16	wOutCurr_q;
	}stOut;
}ST_INV_CURR_CTRL;

extern ST_CTRL_LOOP 	stInvBusVoltCtrl;
extern ST_INV_PARA		stInvPara;
extern ST_INV_VOLT_CTRL	stInvVoltCtrl;
extern ST_INV_CURR_CTRL	stInvCurrCtrl;

extern void InvCtrlParaInit(void);
extern void InvVoltLoopCtrl(void);
extern void InvCurrLoopCtrl(void);
extern void InvBusVoltLoopCtrl(void);
extern	void InvCurrCtrlParmUpdate(void);

#endif
//===========================================================================
// End of file.
//===========================================================================
