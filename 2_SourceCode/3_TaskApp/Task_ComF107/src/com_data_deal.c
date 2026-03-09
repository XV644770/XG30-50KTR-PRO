/*
 * ComDataDeal.c
 *
 *  Created on: 2021年5月7日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

ST_DSP_RECE_DATA stDspReceData;

/****************************************************************************
*
*	FunctionName:	ComF107Task
*	Input:	none
*	Output:	none
*	Description:	DSP and m3 Internal communicate
*	Note:	none.
***************************************************************/
void ComDataInit(void)
{
    memset((void*)&stDspReceData,0,sizeof(stDspReceData));
    stDspReceData.unSafetyOdm.all = 0x0E00;		// default:	CQC2018
	stDspReceData.stProtectEnvrmt.unEnvrmtBit.all = 0x01;		// default:	400V
	stDspReceData.stProtectEnvrmt.unProtectBit.all = 0x07;		// default: enable ISO/DCI/GFCI Chcek
	stDspReceData.unFuncEnable.all = 0x00;						// default: Disable all function
	stDspReceData.uwSettingPower = 30;							// default: 250*100w = 25kw
    stDspReceData.uwACNormalVolt = 4000;
    stDspReceData.udOutputActivePower = 30000;		// 25kw
   	stDspReceData.udOutputApparentPower = 33333;	// 25/0.9 = 27.7
   	stDspReceData.wGFCIRmsFaultTmp = 3000;			// 300mA
}
void DealComPowerData(void)
{
	static Uint16 unEnableBkp;
	/********************Non inverter state and type identification completed****************/
	if((eInverterStatus != cInverterStatus)&&(1==unSysFlag.bit.MachineCheckEnd))
	{
		if((stDspReceData.unSafetyOdm.all != stF107Data.unSafetyOdm.all)
		 ||(stDspReceData.stProtectEnvrmt.unEnvrmtBit.all != stF107Data.stProtectEnvrmt.unEnvrmtBit.all)
		 ||(stDspReceData.stProtectEnvrmt.unProtectBit.all != stF107Data.stProtectEnvrmt.unProtectBit.all)
		 ||(stDspReceData.unFuncEnable.all != stF107Data.unFuncEnable.all)
		 ||(stDspReceData.uwSettingPower != stF107Data.uwSettingPower)
		 ||(unEnableBkp != stF107Data.unEnable.all))
		{
			unEnableBkp = stF107Data.unEnable.all;
			stDspReceData.unSafetyOdm.all = stF107Data.unSafetyOdm.all;
			stDspReceData.stProtectEnvrmt.unEnvrmtBit.all = stF107Data.stProtectEnvrmt.unEnvrmtBit.all;
			stDspReceData.stProtectEnvrmt.unProtectBit.all = stF107Data.stProtectEnvrmt.unProtectBit.all;
			stDspReceData.unFuncEnable.all = stF107Data.unFuncEnable.all;
			stDspReceData.uwSettingPower = stF107Data.uwSettingPower;
			/*******************Active Power range confirmation***********/
			if((stDspReceData.uwSettingPower>stSysCfg.uwMachineMax)
			 ||(stDspReceData.uwSettingPower<stSysCfg.uwMachineMin))
			{
				stDspReceData.uwSettingPower = stSysCfg.uwMachineMin;
			}

			/*******************Voltage range confirmation***********/
			
			switch(stSysCfg.uwMachineType)
			{
				case MACHINE_ID_3_6KW:
					if(stDspReceData.uwSettingPower > CONFIG_6KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_7_9KW:
					if(stDspReceData.uwSettingPower > CONFIG_9KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_10_13KW:
					if(stDspReceData.uwSettingPower > CONFIG_13KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_15_22KW:
				    if(stDspReceData.uwSettingPower > CONFIG_22KW)
				        stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
				    break;
				case MACHINE_ID_25KW:
					if(stDspReceData.uwSettingPower > CONFIG_25KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_Plus_25_30KW:
					if(stDspReceData.uwSettingPower > CONFIG_30KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_25_33KW:
					if(stDspReceData.uwSettingPower > CONFIG_33KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_36_40KW:
					if(stDspReceData.uwSettingPower > CONFIG_40KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_50KW:
					if(stDspReceData.uwSettingPower > CONFIG_50KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_60_70KW:
					if(stDspReceData.uwSettingPower > CONFIG_60KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_PARALLEL_TUBE_15_22KW:
					if(stDspReceData.uwSettingPower > CONFIG_22KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_PARALLEL_TUBE_25KW:
					if(stDspReceData.uwSettingPower > CONFIG_25KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_Plus_30_36KW:
					if(stDspReceData.uwSettingPower > CONFIG_36KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				case MACHINE_ID_Plus_50KW:
					if(stDspReceData.uwSettingPower > CONFIG_50KW)
						stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand = AC_480V_LEVEL;
					break;
				default:					
					break;
			}

			/*******************Voltage range confirmation***********/
			if(AC_220V_LEVEL == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand)
			{
				stDspReceData.uwACNormalVolt = 2200;
				stSysCfg.dACVoltSqrtSumFaultMax = SQRT_SUM_220V_MAX;
				stSysCfg.wACVoltFaultMax = PHASE_220V_MAX;
				stSysCfg.dACVoltSqrtSumFaultMin = SQRT_SUM_220V_MIN;
				stSysCfg.wACVoltFaultMin = PHASE_220V_MIN;
			}
			else if(AC_480V_LEVEL == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand)
			{
				stDspReceData.uwACNormalVolt = 4800;
				stSysCfg.dACVoltSqrtSumFaultMax = SQRT_SUM_480V_MAX;
				stSysCfg.wACVoltFaultMax = PHASE_480V_MAX;
				stSysCfg.dACVoltSqrtSumFaultMin = SQRT_SUM_480V_MIN;
				stSysCfg.wACVoltFaultMin = PHASE_480V_MIN;
			}
			else
			{
				stDspReceData.uwACNormalVolt = 4000;
				stSysCfg.dACVoltSqrtSumFaultMax = SQRT_SUM_400V_MAX;
				stSysCfg.wACVoltFaultMax = PHASE_400V_MAX;
				stSysCfg.dACVoltSqrtSumFaultMin = SQRT_SUM_400V_MIN;
				stSysCfg.wACVoltFaultMin = PHASE_400V_MIN;
				
				if((STRANDARD_CQC_2018 != stDspReceData.unSafetyOdm.bit.SafetyStandard
				&&(stSysCfg.uwMachineType == MACHINE_ID_PRO_33_40KW
				||stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW
				 || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
				||stSysCfg.uwMachineType == MACHINE_ID_PRO_60KW))
				||STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard
                ||STRNDARD_BRAZIL == stDspReceData.unSafetyOdm.bit.SafetyStandard)
				{
					stDspReceData.uwACNormalVolt = 3810;
				}

				if(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard)
				{
					stSysCfg.wACVoltFaultMin = TAIWAN_PHASE_400V_MIN;
				}
			}
			/***************Active Power confirmation***************************/
			if(1==stF107Data.unEnable.bit.OverLoad)
			{
				stDspReceData.udOutputActivePower = (INT32U)stDspReceData.uwSettingPower * 1100;				// Unit 0.1w	OverLoad 1.1
				stDspReceData.udOutputApparentPower = stDspReceData.udOutputActivePower;
			}
			else
			{
				stDspReceData.udOutputActivePower = (INT32U)stDspReceData.uwSettingPower * 1000;				// Unit 0.1w
				stDspReceData.udOutputApparentPower = stDspReceData.udOutputActivePower * 10 / 9;				// Unit 0.1w
			}
			
			
			// Hardware_InvOverCurr = (InvCurrentRms * 1.414*1.2(ripple) * 1.4)
			if((MACHINE_ID_36_40KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType))
			{				
				// Software_InvOverCurr = (InvCurrentRms * 1.414*1.2(ripple) * 1.2)
				// 1204 = (1.414*1.2*1.2*32/1.732)*32 << 5
				stSysCfg.wInvCurrFaultMax = (stDspReceData.udOutputApparentPower*1204/stDspReceData.uwACNormalVolt)>>5;		// Q32
			}
			else
			{	
				// Software_InvOverCurr = (InvCurrentRms * 1.414*1.15(ripple) * 1.2)
				// 1154 = (1.414*1.15*1.2*32/1.732)*32<< 5
				stSysCfg.wInvCurrFaultMax = (stDspReceData.udOutputApparentPower*1154/stDspReceData.uwACNormalVolt)>>5;		// Q32
			}
			// Id_Limit = AcitvePower/(230V-20V)/3* 1.414*32
			// 836 = (1.414*32/1.732)*32
			stSysCfg.wInvCurrMaxLimit = (stDspReceData.udOutputActivePower*836/(stDspReceData.uwACNormalVolt-346))>>5;		// Q32
			// Iq_Limit = (ApparentPower*48.43%/230V)/3*1.414*32
			// 405 = (0.4843*1.414*32/1.732)*32
			stSysCfg.wReactiveCurrMaxLimit = (stDspReceData.udOutputApparentPower*405/stDspReceData.uwACNormalVolt)>>5;		// Q32
			// Active Power Current
			stLoadLimit.wRatedActiveCurr = (stDspReceData.udOutputActivePower/stDspReceData.uwACNormalVolt)+1;
			// Apparent Power Current
			stLoadLimit.wRatedApparentCurr = (stDspReceData.udOutputApparentPower/stDspReceData.uwACNormalVolt)+1;

			// GFCI Fault Value
			if(stDspReceData.udOutputActivePower > 300000)	// 30kw
			{
				stDspReceData.wGFCIRmsFaultTmp = stDspReceData.udOutputActivePower/100;	// 10mA ~ 1KVA
			}
			else
			{
				stDspReceData.wGFCIRmsFaultTmp = 3000;			// 290mA-Fault
			}
			//stWaitStatus.stRelayCheck.wPhaseVoltRmsMax = ((int32)stDspReceData.uwACNormalVolt*296)>>10;		// Rms * 0.5
			stWaitStatus.stRelayCheck.wPhaseVoltRmsMax = ((int32)stDspReceData.uwACNormalVolt*355)>>10;		// Rms * 0.6
			if((MACHINE_ID_PARALLEL_TUBE_15_22KW == stSysCfg.uwMachineType)
				||(MACHINE_ID_PARALLEL_TUBE_25KW == stSysCfg.uwMachineType))
			{
				stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*254)>>10;		// Rms * 0.43
			}
			else
			{
				stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*296)>>10;		// Rms * 0.5
				//stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*414)>>10;		// Rms * 0.7
				//stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*474)>>10;		// Rms * 0.8 950v under
				//stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*503)>>10;		// Rms * 0.85  --930v fail
				//stWaitStatus.stRelayCheck.wPhaseVoltRmsMin = ((int32)stDspReceData.uwACNormalVolt*532)>>10;		// Rms * 0.85 
			}
		}

        UPDNLMT(stF107Data.uwFaultWaveChannelNum1,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum2,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum3,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum4,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum5,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum6,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum7,CHANNELNUMMAX - 1,0);
        UPDNLMT(stF107Data.uwFaultWaveChannelNum8,CHANNELNUMMAX - 1,0);

        stWaveRecord.pChannelAddress1 = pArithValArray[stF107Data.uwFaultWaveChannelNum1];
        stWaveRecord.pChannelAddress2 = pArithValArray[stF107Data.uwFaultWaveChannelNum2];
        stWaveRecord.pChannelAddress3 = pArithValArray[stF107Data.uwFaultWaveChannelNum3];
        stWaveRecord.pChannelAddress4 = pArithValArray[stF107Data.uwFaultWaveChannelNum4];

        stWaveRecord.pChannelAddress5 = pArithValArray[stF107Data.uwFaultWaveChannelNum5];
        stWaveRecord.pChannelAddress6 = pArithValArray[stF107Data.uwFaultWaveChannelNum6];
        stWaveRecord.pChannelAddress7 = pArithValArray[stF107Data.uwFaultWaveChannelNum7];
        stWaveRecord.pChannelAddress8 = pArithValArray[stF107Data.uwFaultWaveChannelNum8];
	}
}

