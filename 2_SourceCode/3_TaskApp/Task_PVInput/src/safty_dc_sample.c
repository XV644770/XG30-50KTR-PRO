/*
 * safty_dc_sample.c
 *
 *  Created on: 2021年1月7日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

void CheckPVVoltRange(void)
{
	static Uint16 suwPVClrFaultCnt=0;

	stDCSample.wPVVoltMax = MAX4(stDCSample.wPVVolt[PVA], stDCSample.wPVVolt[PVB],
								stDCSample.wPVVolt[PVC], stDCSample.wPVVolt[PVD]);

	if(0==stSysFaultReg.unFaultBit.bit.PVVolt)
	{
		if(stDCSample.wPVVoltMax > PV_VOLT_SET_FAULT)
		{
			stSysFaultReg.unFaultBit.bit.PVVolt = 1;
			stSysFaultReg.unPVVoltFault.bit.OverVolt = 1;
		}
		suwPVClrFaultCnt=0;
	}
	else
	{
		if(stDCSample.wPVVoltMax < (PV_VOLT_SET_FAULT))
		{
			suwPVClrFaultCnt++;
			if(suwPVClrFaultCnt >= CLR_FAULT_TIME)
			{
				suwPVClrFaultCnt = 0;
				stSysFaultReg.unPVVoltFault.bit.OverVolt = 0;
			}
		}
		else
		{
			suwPVClrFaultCnt = 0;
		}
	}
}

/****************************************************************************
*
*	FunctionName:	PVPanelReserveCheck
*	Input:	none
*	Output:	none
*	Description:	(PV Inverse: PVVolt<30V && PVVolt<-2A )
*	Note:	none.
***************************************************************************/
void PVPanelReserveCheck(void)
{
	Uint16	uwPVIndex;
	static Uint16	suwPanelReserveFault[PV_MAX_NUM];
	static Uint16	suwSetPVReserveFaultCnt[PV_MAX_NUM], suwClrPVReserveFaultCnt[PV_MAX_NUM];

	if(cInitialStatus == eInverterStatus)
		return;

	for(uwPVIndex=0;	uwPVIndex<PV_MAX_NUM;	uwPVIndex++)
	{
		if(uwPVIndex >= stSysCfg.uwPVNumber)
		{
			suwPanelReserveFault[uwPVIndex] = FALSE;
			continue;
		}
		if(1 == suwPanelReserveFault[uwPVIndex])
		{
			if(stDCSample.wPVVolt[uwPVIndex] > (PV_REVERSE_VOLT+200))
			{
				suwClrPVReserveFaultCnt[uwPVIndex]++;
				if(suwClrPVReserveFaultCnt[uwPVIndex] >= CLR_FAULT_TIME)
				{
					suwClrPVReserveFaultCnt[uwPVIndex] = 0;
					suwPanelReserveFault[uwPVIndex] = 0;
				}
			}
			else
			{
				suwClrPVReserveFaultCnt[uwPVIndex] = 0;
			}
		}
		else
		{
			if((stDCSample.wPVVolt[uwPVIndex] < PV_REVERSE_VOLT)
			&& (stDCSample.wBTCurr[uwPVIndex] < PV_REVERSE_CURR))
			{
				suwSetPVReserveFaultCnt[uwPVIndex]++;
				if(suwSetPVReserveFaultCnt[uwPVIndex] >= SET_PV_FAULT_TIME)
				{
					suwSetPVReserveFaultCnt[uwPVIndex] = 0;
					suwPanelReserveFault[uwPVIndex] = 1;
				}
			}
			else
			{
				suwSetPVReserveFaultCnt[uwPVIndex] = 0;
			}
		}
	}

	if((TRUE == suwPanelReserveFault[PVA]) || (TRUE == suwPanelReserveFault[PVB])
	|| (TRUE == suwPanelReserveFault[PVC]) || (TRUE == suwPanelReserveFault[PVD]))
	{
		stSysFaultReg.unFaultBit.bit.PVVolt = 1;
		stSysFaultReg.unPVVoltFault.bit.PanelReserve = 1;
	}
	else
	{
		stSysFaultReg.unPVVoltFault.bit.PanelReserve = 0;
	}
}

/****************************************************************************
*
*	FunctionName:	PVPanelShortCheck
*	Input:	none
*	Output:	none
*	Description:	(PV Short: PVVolt<30V && PVVolt>5A )
*	Note:	none.
***************************************************************************/
void PVPanelShortCheck(void)
{
	Uint16	uwPVIndex;
	static Uint16	suwPanelShortFault[PV_MAX_NUM];
	static Uint16	suwSetPVShortFaultCnt[PV_MAX_NUM], suwClrPVShortFaultCnt[PV_MAX_NUM];

	if(cInitialStatus == eInverterStatus)
		return;
	for(uwPVIndex=0;	uwPVIndex<PV_MAX_NUM;	uwPVIndex++)
	{
		if(uwPVIndex >= stSysCfg.uwPVNumber)
		{
			suwPanelShortFault[uwPVIndex] = FALSE;
			continue;
		}
		if(1 == suwPanelShortFault[uwPVIndex])
		{
			if(stDCSample.wPVVolt[uwPVIndex] > (PV_SHORT_VOLT+200))
			{
				suwClrPVShortFaultCnt[uwPVIndex]++;
				if(suwClrPVShortFaultCnt[uwPVIndex] >= CLR_FAULT_TIME)
				{
					suwClrPVShortFaultCnt[uwPVIndex] = 0;
					suwPanelShortFault[uwPVIndex] = 0;
				}
			}
			else
			{
				suwClrPVShortFaultCnt[uwPVIndex] = 0;
			}
		}
		else
		{
			if((stDCSample.wPVVolt[uwPVIndex] < PV_SHORT_VOLT)
			&& (stDCSample.wBTCurr[uwPVIndex] > PV_SHORT_CURR))
			{
				suwSetPVShortFaultCnt[uwPVIndex]++;
				if(suwSetPVShortFaultCnt[uwPVIndex] >= SET_PV_FAULT_TIME)
				{
					suwSetPVShortFaultCnt[uwPVIndex] = 0;
					suwPanelShortFault[uwPVIndex] = 1;
				}
			}
			else
			{
				suwSetPVShortFaultCnt[uwPVIndex] = 0;
			}
		}
	}

	if((TRUE == suwPanelShortFault[PVA]) || (TRUE == suwPanelShortFault[PVB])
	|| (TRUE == suwPanelShortFault[PVC]) || (TRUE == suwPanelShortFault[PVD]))
	{
		stSysFaultReg.unFaultBit.bit.PVVolt = 1;
		stSysFaultReg.unPVVoltFault.bit.PanelShort = 1;
	}
	else
	{
		stSysFaultReg.unPVVoltFault.bit.PanelShort = 0;
	}
}

/****************************************************************************
*
*	FunctionName:	BusVoltSampleCheck
*	Input:	none
*	Output:	none
*	Description:	Waiting Status : abs(Vpv - Vbus)>50V Set Bus Sample Fault
*	Note:	none.
***************************************************************************/
void BusVoltSampleCheck(void)
{
	static Uint16	suwSetBusSampleFaultCnt=0,	suwClrBusSampleFaultCnt=0;

	if((eInverterStatus!=cInverterStatus)&&(cInit==stWaitStatus.eStandbyStep))
	{
		if(1 == stSysFaultReg.unSampleFault.bit.BusVolt)
		{
			if(abs(stDCSample.wPVVoltMax-stDCSample.wBusVolt) < (PV_BUS_DELTA-200))
			{
				suwClrBusSampleFaultCnt++;
				if(suwClrBusSampleFaultCnt>=CLR_FAULT_TIME)
				{
					suwClrBusSampleFaultCnt = 0;
					stSysFaultReg.unSampleFault.bit.BusVolt = 0;
				}
			}
			else
			{
				suwClrBusSampleFaultCnt = 0;
			}
		}
		else
		{
			suwClrBusSampleFaultCnt = 0;
			if((abs(stDCSample.wPVVoltMax-stDCSample.wBusVolt) > PV_BUS_DELTA)&&(stDCSample.wPVVoltMax > PV_BUS_Volt))
			{
				suwSetBusSampleFaultCnt++;
				if(suwSetBusSampleFaultCnt>=SET_PV_FAULT_TIME)
				{
					suwSetBusSampleFaultCnt = 0;
					stSysFaultReg.unFaultBit.bit.SampleDiffer = 1;
					stSysFaultReg.unSampleFault.bit.BusVolt = 1;
				}

			}
			else
			{
				suwSetBusSampleFaultCnt = 0;
			}
		}
	}
}

/****************************************************************************
*
*	FunctionName:	BTCurrSampleCheck
*	Input:	none
*	Output:	none
*	Description:	Waiting Status : abs(Vpv - Vbus)>50V Set Bus Sample Fault
*	Note:	none.
***************************************************************************/
void BTCurrSampleCheck(void)
{
	Uint16	uwPVIndex;
	static Uint16	suwBTCurrSampleFault[PV_MAX_NUM];
	static Uint16	suwSetBTCurrSampleFaultCnt[PV_MAX_NUM], suwClrBTCurrSampleFaultCnt[PV_MAX_NUM];

	if(eInverterStatus!=cInverterStatus)
	{
		for(uwPVIndex=0;	uwPVIndex<PV_MAX_NUM;	uwPVIndex++)
		{
			if(uwPVIndex >= stSysCfg.uwPVNumber)
			{
				suwBTCurrSampleFault[uwPVIndex] = FALSE;
				continue;
			}
			if(1 == suwBTCurrSampleFault[uwPVIndex])
			{
				if((stDCSample.wBTCurr[uwPVIndex] < stSysCfg.wBTOverCurrFaultAvg)
			    && (stDCSample.wBTCurr[uwPVIndex] > PV_REVERSE_CURR))
				{
					suwClrBTCurrSampleFaultCnt[uwPVIndex]++;
					if(suwClrBTCurrSampleFaultCnt[uwPVIndex] >= CLR_FAULT_TIME)
					{
						suwClrBTCurrSampleFaultCnt[uwPVIndex] = 0;
						suwBTCurrSampleFault[uwPVIndex] = 0;
					}
				}
				else
				{
					suwClrBTCurrSampleFaultCnt[uwPVIndex] = 0;
				}
			}
			else
			{
				if(stDCSample.wBTCurr[uwPVIndex] < PV_REVERSE_CURR)
				{
					suwSetBTCurrSampleFaultCnt[uwPVIndex]++;
					if(suwSetBTCurrSampleFaultCnt[uwPVIndex] >= SET_PV_FAULT_TIME)
					{
						suwSetBTCurrSampleFaultCnt[uwPVIndex] = 0;
						suwBTCurrSampleFault[uwPVIndex] = 1;
					}
				}
				else
				{
					suwSetBTCurrSampleFaultCnt[uwPVIndex] = 0;
				}
			}
		}

		if((TRUE==suwBTCurrSampleFault[PVA]) || (TRUE==suwBTCurrSampleFault[PVB])
		|| (TRUE==suwBTCurrSampleFault[PVC]) || (TRUE==suwBTCurrSampleFault[PVD]))
		{
			stSysFaultReg.unFaultBit.bit.SampleDiffer = 1;
			stSysFaultReg.unSampleFault.bit.BTCurr = 1;
		}
		else
		{
			stSysFaultReg.unSampleFault.bit.BTCurr = 0;
		}
	}
}
