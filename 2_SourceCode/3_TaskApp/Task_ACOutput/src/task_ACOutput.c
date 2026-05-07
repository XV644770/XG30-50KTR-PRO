/*
 * task_ACOutput.c
 *
 *  Created on: 2020年12月26日
 *      Author: Administrator
 */

#include "dsp_user_include.h"
ST_SVG_PARA SvgParam;


/****************************************************************************
*
*	FunctionName:	PVInputTaskParaInit
*	Input:	none
*	Output:	none
*	Description:	DSP Status Change Task, quicly on/off inverter
*	Note:	none.
***************************************************************************/
void ACOutputTaskParaInit(void)
{
	stACSample.uwPowerEnergySum = 0;
	LoadLimitParaInit();
	GridFreqParaInit();
	stautotestInit();
}


void InvFaultWarnCheck(void)
{
	static Uint16	suwVoltWarnClearCnt=0;
				/*******************Fault Flag Clear*******************/
	if((1==stSysFaultReg.unFaultBit.bit.PVVolt)&&(0==stSysFaultReg.unPVVoltFault.all))
	{
		stSysFaultReg.unFaultBit.bit.PVVolt = 0;
	}

	if((1==stSysFaultReg.unFaultBit.bit.GridVolt)&&(0==stSysFaultReg.unGridVoltFault.all))
	{
		stSysFaultReg.unFaultBit.bit.GridVolt = 0;
	}

	if((1==stSysFaultReg.unFaultBit.bit.GridFreq)&&(0==stSysFaultReg.unGridFreqFault.all))
	{
		stSysFaultReg.unFaultBit.bit.GridFreq = 0;
	}

	if((1==stSysFaultReg.unFaultBit.bit.OverCurr)&&(0==stSysFaultReg.unOverCurrFault.all))
	{
		stSysFaultReg.unFaultBit.bit.OverCurr = 0;
	}

	if((1==stSysFaultReg.unFaultBit.bit.CommunicateFault)&&(0==stSysFaultReg.unCommunicateFault.all))
	{
		stSysFaultReg.unFaultBit.bit.CommunicateFault = 0;
	}

	if((1==stSysFaultReg.unFaultBit.bit.SampleDiffer)&&(0==stSysFaultReg.unSampleFault.all))
	{
		stSysFaultReg.unFaultBit.bit.SampleDiffer = 0;
	}
	if((1==stSysFaultReg.unFaultBit.bit.AFCIFault)&&(0==stSysFaultReg.unAFCIFault.all))
	{
		stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
	}
	if((1==stSysFaultReg.unFaultBit.bit.AFCISelfCheck)&&(0==stSysFaultReg.unAFCISelfCheck.all))
	{
		stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
	}
	stSysFaultReg.uwFaultCode = GetUint64Bit(stSysFaultReg.unFaultBit.all);
	switch(stSysFaultReg.uwFaultCode)
	{
		case 0:					// no fault
			stSysFaultReg.uwFaultSubCode = 0;
		break;

		case 1:					// Bit-1 PV Input Voltage Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unPVVoltFault.all);
		break;

		case 3:					// Bit-3 BusVolt Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unBusVoltFault.all);
		break;

		case 5:					// Bit-5 OverCurr Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unOverCurrFault.all);
		break;

		case 6:                 // Bit-6 AMTempture Fault--20211220Revised
		    stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unTempFault.all);
		break;

		case 9:					// Bit-9 Communicate Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unCommunicateFault.all);
		break;

		case 10:					// Bit-10 GFCI Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unGFCIFault.all);
		break;

		case 19:					// Bit-19 Communicate Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unSampleFault.all);
		break;

		case 31:					// Bit-31 Grid Voltage Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unGridVoltFault.all);
		break;

		case 33:					// Bit-33  Grid Freqency Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unGridFreqFault.all);
		break;
		case 38:					// Bit-38 Grid AFCI Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unAFCIFault.all);
		break;
		case 39:					// Bit-39 Grid AFCI Self check Fault
			stSysFaultReg.uwFaultSubCode = GetUint64Bit(stSysFaultReg.unAFCISelfCheck.all);
		break;

		default:
			stSysFaultReg.uwFaultSubCode = 1;
		break;
	}

	/*******************Warning Flag Clear*******************/

	if(1==stSysFaultReg.unWarnBit.bit.ACOverVolt)
	{
		if(stF107Data.unSet.bit.ACVoltLevel!=0)
		{
			suwVoltWarnClearCnt++;
			if(suwVoltWarnClearCnt>=3000)		// 20ms*3000 = 60s
			{
				suwVoltWarnClearCnt = 0;
				stSysFaultReg.unWarnBit.bit.ACOverVolt = 0;
			}
		}
	}
	else
	{
		suwVoltWarnClearCnt = 0;
	}
	
	if(true==stF107Data.unF107WarnBit.bit.AntiFlowMeterWarn)
	{
		stSysFaultReg.unWarnBit.bit.MeterCommFault = 1;
	}
	else
	{
		stSysFaultReg.unWarnBit.bit.MeterCommFault = 0;
	}

	stSysFaultReg.uwWarnCode = GetUint64Bit(stSysFaultReg.unWarnBit.all);
	switch(stSysFaultReg.uwWarnCode)
	{
		case 0:					// no fault
			stSysFaultReg.uwWarnSubCode = 0;
		break;
		
		case 1:					// Bit-1 FAN Warning subcode
			stSysFaultReg.uwWarnSubCode = GetUint64Bit(stSysFaultReg.unFanWarn.all);
		break;

		case 2:					// Bit-2 SPD Warning subcode
			stSysFaultReg.uwWarnSubCode = GetUint64Bit(stSysFaultReg.unSPDWarn.all);
		break;

		default:
			stSysFaultReg.uwWarnSubCode = 1;
		break;
	}

	
}

void sSVGCheck(void)
{
    static Uint16 SVGSwitchWaitTime_Cnt =0,SVGSwitchWaitTime_Cnt1 =0;
    if(((stDCSample.wPVVolt[PVA] > wStartPvVolt) || (stDCSample.wPVVolt[PVB] > wStartPvVolt)
      || (stDCSample.wPVVolt[PVC] > wStartPvVolt)||(stDCSample.wPVVolt[PVD] > wStartPvVolt))
    &&(ENABLE == stDspReceData.unFuncEnable.bit.SVG) && (true == SvgParam.bEnterSVGStateFlag))
    {
        SVGSwitchWaitTime_Cnt++;
        if(500 <= SVGSwitchWaitTime_Cnt) 
        {
            SvgParam.bEnterSVGStateFlag = false;
			SVGSwitchWaitTime_Cnt = 0;
            OSEventSend(STATUS_PRIO, STATUS_TO_FAULT);
        }
    }
    else
    {
        SVGSwitchWaitTime_Cnt = 0;
    }

  if((stDCSample.wPVVolt[PVA] < (wStartPvVolt-VDC20V)) && (stDCSample.wPVVolt[PVB] < (wStartPvVolt-VDC20V))
    &&(stDCSample.wPVVolt[PVC] < (wStartPvVolt-VDC20V))&&(stDCSample.wPVVolt[PVD]  < (wStartPvVolt-VDC20V))
    &&(ENABLE == stDspReceData.unFuncEnable.bit.SVG) && (false == SvgParam.bEnterSVGStateFlag)&&(cInverterStatus == eInverterStatus))
    {
      if((MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)&&(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard))
      {
        SVGSwitchWaitTime_Cnt1++;
        if(30 < SVGSwitchWaitTime_Cnt1)   //20 * 50 * 6 = 6s
        {
            SvgParam.bEnterSVGStateFlag = true;
			SVGSwitchWaitTime_Cnt1 = 0;
        }
      }
      else
      {
          SvgParam.bEnterSVGStateFlag = false;
		  SVGSwitchWaitTime_Cnt1 = 0;
      }
    }
    else
    {
        SVGSwitchWaitTime_Cnt1 = 0;
    }

    if(DISABLE == stDspReceData.unFuncEnable.bit.SVG)
    {
        SVGSwitchWaitTime_Cnt = 0;
        SVGSwitchWaitTime_Cnt1 = 0;
        SvgParam.bEnterSVGStateFlag = false;
    }
}



/****************************************************************************
*
*	FunctionName:	StatusTask
*	Input:	none
*	Output:	none
*	Description:	DSP Status Change Task, quicly on/off inverter
*	Note:	none.
***************************************************************/
void ACOutputTask(void)
{
	static TASK_EVENT event = 0;
	static Uint16 suwTimer500msCnt;

	

	while(1)
	{
		event = OSEventPend();		// idle

		if(event&(1<<ACOUTPUT_GRID_ZERO))		// task period 20ms
		{
		    sSVGCheck();
			CalcACVoltCurrRms();
			ZeroLossChk();
			CalcOutputPower();
	 		if(stautotest.uwProcess  == PROCES_WORK)  // Autotest timer
	 		{
	 		 	stautotest.wBreakCnt++; // add every ms
	 		}
			GridVoltProtection();
			GridFreqProtection();
			if(DISABLE == stDebug.SetData.unSetReg.bit.OpenLoopUnlock)
			{
				TemperatureProtection();
				GFCISelfCheck();
				GFCIOverCurrCheck();
				DCIAdjust();
				DCIProtection();
				OPTask_AutoTest();
			}
		}

		if(event&(1<<ACOUTPUT_TASK_PERIOD))		// task period 20ms
		{
			InvFaultWarnCheck();
			ReactivePowerCtrl();
			InverterLoadLimit();
			GridFreqRangeCheck();
			FanOnOffCtrl();

			suwTimer500msCnt++;
			if(suwTimer500msCnt>=25)		// 25*20ms = 500ms
			{
				suwTimer500msCnt = 0;
				FanApplication();
				EnergizeDegreeCalc();			// Inverter Power Energy
			}
		}
	}
}



