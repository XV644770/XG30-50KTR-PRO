/*
 * safty_load_limit.c
 *
 *  Created on: 2021年1月7日
 *      Author: Administrator
 */
#include "dsp_user_include.h"
#include "IQmathLib.h"

ST_LOAD_LIMIT stLoadLimit;

void LoadLimitParaInit(void)
{
	memset(&stLoadLimit,0,sizeof(stLoadLimit));

	stLoadLimit.dActivePower = stDspReceData.udOutputActivePower;		// 50kw = 50*1000w*10
	stLoadLimit.dApparentPower = stDspReceData.udOutputApparentPower;

	stLoadLimit.uwDeratingMode = 0;

	stLoadLimit.dReactivePowerRef = 0;
	stLoadLimit.wVoltReactiveCurrLimit = 0;

	stLoadLimit.dActivePowerActual = stLoadLimit.dActivePower;
	stLoadLimit.dApparPowerActual = stLoadLimit.dApparentPower;

	stLoadLimit.dActPowerInvBoostTempLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerAmTempLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerReactiveLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerPVHighVoltLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerACHighVoltLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActivePower;
	stLoadLimit.dActPowerMultiAntiFlowLimit = stLoadLimit.dActivePower;

	stLoadLimit.wRatedActiveCurr = (stLoadLimit.dActivePower/stDspReceData.uwACNormalVolt)+1;
	stLoadLimit.wRatedApparentCurr = (stLoadLimit.dApparentPower/stDspReceData.uwACNormalVolt)+1;
}

static void GridVoltActuraltPower(Uint16 uwRSTLineVoltMin)
{
	stLoadLimit.dActivePower = stDspReceData.udOutputActivePower;
	stLoadLimit.dApparentPower = stDspReceData.udOutputApparentPower;
	if(0 == stInvPwm.unPwmDisableBit.bit.LowVoltThrough)
	{
		/***************The rated power is calculated according to the maximum current*****************/
		stLoadLimit.dActivePowerActual = (int32)stLoadLimit.wRatedActiveCurr*uwRSTLineVoltMin;
		if(stLoadLimit.dActivePowerActual >= stLoadLimit.dActivePower)
		{
			stLoadLimit.dActivePowerActual = stLoadLimit.dActivePower;
		}
		/***************The apparent power is calculated from the maximum apparent current*****************/
		stLoadLimit.dApparPowerActual = (int32)stLoadLimit.wRatedApparentCurr*uwRSTLineVoltMin;
		if(stLoadLimit.dApparPowerActual >= stLoadLimit.dApparentPower)
		{
			stLoadLimit.dApparPowerActual = stLoadLimit.dApparentPower;
		}
	}
}

static void PVVoltHighDerating(Uint16 uwBusVolt)
{
    	Uint32 udPVHighVoltPowerLimitTmp;
   	 //20210329,Revised(Non-Test)
	if(MACHINE_ID_60_70KW == stSysCfg.uwMachineType)
	{
		if(AC_480V_LEVEL == stDspReceData.stProtectEnvrmt.unEnvrmtBit.bit.ACVoltStand)
		{
			// 70kw 277V
			if(uwBusVolt <= 8000 )//480~800v,No-Derating
			{
				udPVHighVoltPowerLimitTmp = 700000;//Ppv = 70kW
			}
			else if(uwBusVolt <= 8400)
			{
				udPVHighVoltPowerLimitTmp = 700000 - (((Uint32)uwBusVolt-8000)*27);			
			}
			else if(uwBusVolt <= 8800)
			{
				udPVHighVoltPowerLimitTmp = 687242 - (((Uint32)uwBusVolt-8400)*116);	
			}
			else if(uwBusVolt <= 9200)
			{
				udPVHighVoltPowerLimitTmp = 640658 - (((Uint32)uwBusVolt-8800)*255);	
			}
			else if(uwBusVolt <= 9600)
			{
				udPVHighVoltPowerLimitTmp = 538369  - (((Uint32)uwBusVolt-9200)*485);	
			}
			else if(uwBusVolt <= 10000)
			{
				udPVHighVoltPowerLimitTmp = 344449 - (((Uint32)uwBusVolt-9600)*861);	
			}    
			else//<200v Or >1000v,Protected
			{
				udPVHighVoltPowerLimitTmp=0;
			}
		}
		else
		{
			// 60kw 230
			if(uwBusVolt <= 8000 )//480~800v,No-Derating
			{
				udPVHighVoltPowerLimitTmp = 600000;//Ppv = 60kW
			}
			else if(uwBusVolt <= 8400)
			{
				udPVHighVoltPowerLimitTmp = 600000 - (((Uint32)uwBusVolt-8000)*27);			
			}
			else if(uwBusVolt <= 8800)
			{
				udPVHighVoltPowerLimitTmp = 589064 - (((Uint32)uwBusVolt-8400)*100);
			}
			else if(uwBusVolt <= 9200)
			{
				udPVHighVoltPowerLimitTmp = 549136 - (((Uint32)uwBusVolt-8800)*219);
			}
			else if(uwBusVolt <= 9600)
			{
				udPVHighVoltPowerLimitTmp = 461459 - (((Uint32)uwBusVolt-9200)*416);
			}
			else if(uwBusVolt <= 10000)
			{
				udPVHighVoltPowerLimitTmp = 295242 - (((Uint32)uwBusVolt-9600)*738);
			}    
			else//<200v Or >1000v,Protected
			{
				udPVHighVoltPowerLimitTmp=0;
			}
		}
	}
	else if(MACHINE_ID_25_33KW == stSysCfg.uwMachineType || MACHINE_ID_36_40KW == stSysCfg.uwMachineType
		||MACHINE_ID_PRO_33_40KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 400000;								// (800V,40KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,40KW),(840V,39KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 600000 -  (INT32U)uwBusVolt *25;
			udPVHighVoltPowerLimitTmp = udPVHighVoltPowerLimitTmp + 10000;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,39KW),(880V,36.5KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 915000 -  (INT32U)uwBusVolt *62 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,36.5KW),(920V,30KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1795000 - (INT32U)uwBusVolt *162 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,30KW),(960V,19KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2830000  - (INT32U)uwBusVolt *275;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,19KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 4750000 - (INT32U)uwBusVolt *475;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 360000;								// (800V,36KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,36KW),(840V,35.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 491310 -  (INT32U)uwBusVolt *16;
			//udPVHighVoltPowerLimitTmp = udPVHighVoltPowerLimitTmp + 13120;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,35.6KW),(880V,33KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 856540 -  (INT32U)uwBusVolt *59 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,33KW),(920V,27.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1486810 - (INT32U)uwBusVolt *131 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,27.6KW),(960V,18KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2570660 - (INT32U)uwBusVolt *249;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,18KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 4425000 - (INT32U)uwBusVolt *442 - uwBusVolt / 2;;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_Plus_50KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 500000;								// (800V,36KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,36KW),(840V,35.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 682375-  (INT32U)uwBusVolt *22 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,35.6KW),(880V,33KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1189641 -  (INT32U)uwBusVolt *83;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,33KW),(920V,27.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2065022 - (INT32U)uwBusVolt *182- uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,27.6KW),(960V,18KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 3570379 - (INT32U)uwBusVolt *346;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,18KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 6150000 - (INT32U)uwBusVolt *615;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_PRO_60KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 600000;								// (800V,36KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,36KW),(840V,35.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 818850-  (INT32U)uwBusVolt *26;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,35.6KW),(880V,33KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1427569 -  (INT32U)uwBusVolt *99;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,33KW),(920V,27.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2478026 - (INT32U)uwBusVolt *218;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,27.6KW),(960V,18KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 4284454 - (INT32U)uwBusVolt *415;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,18KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 7380000 - (INT32U)uwBusVolt *738;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2))
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 500000;								// (800V,36KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,36KW),(840V,35.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 682375-  (INT32U)uwBusVolt *22 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,35.6KW),(880V,33KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1189641 -  (INT32U)uwBusVolt *83;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,33KW),(920V,27.6KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2065022 - (INT32U)uwBusVolt *182- uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,27.6KW),(960V,18KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 3570379 - (INT32U)uwBusVolt *346;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,18KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 6150000 - (INT32U)uwBusVolt *615;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 300000;								// (800V,25KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,25KW),(840V,24.4KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 404000 -  (INT32U)uwBusVolt *13;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,24.4KW),(880V,22.4KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 714530 -  (INT32U)uwBusVolt *50;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,22.4KW),(920V,18.5KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1233760 - (INT32U)uwBusVolt *109 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,18.5KW),(960V,11.5KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2135130  - (INT32U)uwBusVolt *207;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,11.5KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 3690000 - (INT32U)uwBusVolt *369 - uwBusVolt / 2;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
		 udPVHighVoltPowerLimitTmp += 2000;
	}
	else if((MACHINE_ID_25KW == stSysCfg.uwMachineType)||(MACHINE_ID_PARALLEL_TUBE_25KW == stSysCfg.uwMachineType))
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 250000;								// (800V,25KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,25KW),(840V,24.4KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 370000 -  (INT32U)uwBusVolt *15;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,24.4KW),(880V,22.4KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 664000 -  (INT32U)uwBusVolt *50;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,22.4KW),(920V,18.5KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1082000 - (INT32U)uwBusVolt *97 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,18.5KW),(960V,11.5KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1795000  - (INT32U)uwBusVolt *175;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,11.5KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 2875000 - (INT32U)uwBusVolt *287 - uwBusVolt / 2;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
		 udPVHighVoltPowerLimitTmp += 2000;
	}
	else if((MACHINE_ID_15_22KW == stSysCfg.uwMachineType)||(MACHINE_ID_PARALLEL_TUBE_15_22KW == stSysCfg.uwMachineType))
	{
	    if(uwBusVolt <= 8000)//480~800v,No-Derating
	    {
	        udPVHighVoltPowerLimitTmp = 220000;                             // (800V,22KW)
	    }
	    else if(uwBusVolt <= 8400)
	    {
	        udPVHighVoltPowerLimitTmp = 436800 - (INT32U)uwBusVolt *271/10;    // (840V,20.916KW)
	    }
	    else if(uwBusVolt <= 8800)
	    {
	        udPVHighVoltPowerLimitTmp = 665910 - (INT32U)uwBusVolt *544/10;    // (880V,18.741KW)
	    }
	    else if(uwBusVolt <= 9200)
	    {
	        udPVHighVoltPowerLimitTmp = 1005810 - (INT32U)uwBusVolt *93;   // (920V,15.021KW)
	    }
	    else if(uwBusVolt <= 9600)
	    {
	        udPVHighVoltPowerLimitTmp = 1514110 - (INT32U)uwBusVolt *1482/10;  // (960V,9.091KW)
	    }
	    else if(uwBusVolt <= 10000)
	    {
	        udPVHighVoltPowerLimitTmp = 2270110 - (INT32U)uwBusVolt *227;  // (10000V,0.011KW)
	    }
	    else
	    {
	        udPVHighVoltPowerLimitTmp=0;
	    }
	    udPVHighVoltPowerLimitTmp += 2000;
	}
	else if(MACHINE_ID_10_13KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 130000;								// (800V,13KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V13KW),(840V,12.7KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 190000 -  (INT32U)uwBusVolt *7 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,12.7KW),(880V,11.64KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 349600 -  (INT32U)uwBusVolt *26 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,11.64KW),(920V,9.58KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 569600 - (INT32U)uwBusVolt *51 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V,9.58KW),(960V,5.98KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 923800  - (INT32U)uwBusVolt *90;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,5.98KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1495000 - (INT32U)uwBusVolt *149 - uwBusVolt / 2;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_7_9KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 90000;									// (800V,9KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,9KW),(840V,8.82KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 126000 -  (INT32U)uwBusVolt *4 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,8.82KW),(880V,8.14KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 239400 -  (INT32U)uwBusVolt *18;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,8.1KW),(920V,6.66KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 397800 - (INT32U)uwBusVolt *36;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V6.66KW),(960V,4.16KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 641600  - (INT32U)uwBusVolt *62 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,4.16KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 1040000 - (INT32U)uwBusVolt *104;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else if(MACHINE_ID_3_6KW == stSysCfg.uwMachineType)
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 60000;									// (800V,6KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,6KW),(840V,5.82KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 96000 -  (INT32U)uwBusVolt *4 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,5.82KW),(880V,5.34KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 159000 -  (INT32U)uwBusVolt *12;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,5.34KW),(920V,4.44KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 260200 - (INT32U)uwBusVolt *23 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V4.44KW),(960V2.74KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 425800  - (INT32U)uwBusVolt *41 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,2.74KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 685000 - (INT32U)uwBusVolt *68 - uwBusVolt / 2;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
	else
	{
		 if(uwBusVolt <= 8000 )//480~800v,No-Derating
	    	{
	        	udPVHighVoltPowerLimitTmp = 60000;									// (800V,6KW)
	    	}
	    	else if(uwBusVolt <= 8400)											//(800V,6KW),(840V,5.82KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 96000 -  (INT32U)uwBusVolt *4 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 8800)											//(840V,5.82KW),(880V,5.34KW
	    	{
	        	udPVHighVoltPowerLimitTmp = 159000 -  (INT32U)uwBusVolt *12;
	    	}
	    	else if(uwBusVolt <= 9200)											//(880V,5.34KW),(920V,4.44KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 260200 - (INT32U)uwBusVolt *23 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 9600)											//(920V4.44KW),(960V2.74KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 425800  - (INT32U)uwBusVolt *41 - uwBusVolt / 2;
	    	}
	    	else if(uwBusVolt <= 10000)											//(960V,2.74KW),(1000V,0KW)
	    	{
	        	udPVHighVoltPowerLimitTmp = 685000 - (INT32U)uwBusVolt *68 - uwBusVolt / 2;
	    	}    
	    	else//<200v Or >1000v,Protected
	    	{
	        	udPVHighVoltPowerLimitTmp=0;
	    	}
	}
		
	/************PV high voltage limits active power**************/
	if(1==stF107Data.unEnable.bit.OverLoad)
	{
		stLoadLimit.dActPowerPVHighVoltLimit = (udPVHighVoltPowerLimitTmp*1127)>>10;
	}
	else
	{
		stLoadLimit.dActPowerPVHighVoltLimit = udPVHighVoltPowerLimitTmp;
	}

	if(stLoadLimit.dActPowerPVHighVoltLimit > stLoadLimit.dActivePower)
	{
		stLoadLimit.dActPowerPVHighVoltLimit = stLoadLimit.dActivePower;
	}

	/************PV high voltage limits apparent power**************/
	stLoadLimit.dApparentPowerPVHighVoltLimit = (udPVHighVoltPowerLimitTmp*1126)>>10;
	if(stLoadLimit.dApparentPowerPVHighVoltLimit > stLoadLimit.dApparentPower)
	{
		stLoadLimit.dApparentPowerPVHighVoltLimit = stLoadLimit.dApparentPower;
	}
}

static int16 PVVoltLowCurrLimit(Uint16 uwPVIndex)
{
   int16 wPVCurrLimitTmp;
   
   if(stDCSample.wPVVoltAvg[uwPVIndex] < PV_VOLT_LIMIT_MIN)
   {
        wPVCurrLimitTmp = PV_CURR_LIMIT_MIN_60_70K;           // 2A
   }
   else if((stDCSample.wPVVoltAvg[uwPVIndex]  > PV_VOLT_LIMIT_MIN)&&(stDCSample.wPVVoltAvg[uwPVIndex] < stSysCfg.wPVVoltLimitMax)&&(0 == stIVCurve.unIVCurveBits.bit.StartEnable))
   {
   		if(MACHINE_ID_50KW == stSysCfg.uwMachineType || MACHINE_ID_60_70KW == stSysCfg.uwMachineType)
   		{
			wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *378-692000)/1000;
		}			
		else if(MACHINE_ID_25_33KW == stSysCfg.uwMachineType || MACHINE_ID_36_40KW == stSysCfg.uwMachineType)
		{
        		wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *286-508800)/1000;
		}
		else if((MACHINE_ID_Plus_30_36KW == stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW == stSysCfg.uwMachineType))
		{
			if(stDCSample.wPVVoltAvg[uwPVIndex] > 4500)
			{
				wPVCurrLimitTmp = 1024;
			}
			else if(stDCSample.wPVVoltAvg[uwPVIndex] > 4000)
			{
	        	wPVCurrLimitTmp = (((int32)stDCSample.wPVVoltAvg[uwPVIndex] - 4000)*192)/500+832;//mgs modify for 26A to 32A
			}
			else
			{
				wPVCurrLimitTmp = (((int32)stDCSample.wPVVoltAvg[uwPVIndex] - 2000)*768)/2000+64;//mgs modify for 2A to 26A
			}
		}
		else if((MACHINE_ID_25KW == stSysCfg.uwMachineType)&&(PVA == uwPVIndex))	// XG25K PVACurrLimit:28A
		{
			wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *297-529600)/1000;
		}
		else if((MACHINE_ID_25KW == stSysCfg.uwMachineType)&&(PVB == uwPVIndex))	// XG25K PVBCurrLimit:42A
		{
			wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *457-849600)/1000;
		}
		else if(MACHINE_ID_Plus_25_30KW == stSysCfg.uwMachineType)	// XG30-plusK PVBCurrLimit:40A
		{
			wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *553-1041440)/1000;//0.1727*32*100=553;32.545*32*100=1041440
		}
		else if(MACHINE_ID_PRO_60KW == stSysCfg.uwMachineType || MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType||(stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2)
			||MACHINE_ID_PRO_33_40KW == stSysCfg.uwMachineType)
		{
			
			if(stDCSample.wPVVoltAvg[uwPVIndex] < 1800)
			{
				wPVCurrLimitTmp = 64;
			}
			else if(stDCSample.wPVVoltAvg[uwPVIndex] < 3450)
			{
				wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] * 896 - 1507200)/1650;
			}
			else if(stDCSample.wPVVoltAvg[uwPVIndex] < 4050)
			{
				wPVCurrLimitTmp = 1024;
			}
			else if(stDCSample.wPVVoltAvg[uwPVIndex] < 4150)
			{
				//405V ~ 415V
				wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] * 256 - 934400)/100;//(32A + (PV - 405V) * ((40A- 32A)/(10V)) * 32
			}
			else
			{
				wPVCurrLimitTmp = 1280;
			}
			
		}
		else
		{
			wPVCurrLimitTmp = ((int32)stDCSample.wPVVoltAvg[uwPVIndex] *274-483200)/1000;
		}
   }
   else
   {
        wPVCurrLimitTmp = stSysCfg.wBTCurrLimitMax[uwPVIndex];       // Curr Max
   }

   if(wPVCurrLimitTmp > stSysCfg.wBTCurrLimitMax[uwPVIndex])
   {
	   wPVCurrLimitTmp = stSysCfg.wBTCurrLimitMax[uwPVIndex];
   }
   	if(uwPVIndex >= stSysCfg.uwPVNumber)
   		wPVCurrLimitTmp = 0;
	
   return wPVCurrLimitTmp;
}


// temperature sample ratio are all same
static void TemperatureHighDerating(void)
{
	static Uint16 suwInvBoostDeratCnt=0,suwAmDeratCnt=0;

	if(cInverterStatus == eInverterStatus)
	{
		/**********************Inverter Boost/inv High Temperature Derating************************************/
		suwInvBoostDeratCnt++;
		if(suwInvBoostDeratCnt>=10)		// 10*20ms = 200ms
		{
			suwInvBoostDeratCnt = 0;
			if(stDCSample.wInvTemp > stMachineCfg.wInvTempDerate || stDCSample.wBoostTemp > stMachineCfg.wBoostTempDerate)
			{

				if(stACSample.dActivePower > (stLoadLimit.dActivePower/10))	// RealPower > RatedPower*10%
				{
					if(stACSample.dActivePower > stLoadLimit.dActPowerLimitOutput)
					{
						stLoadLimit.dActPowerInvBoostTempLimit = stLoadLimit.dActPowerLimitOutput-stMachineCfg.wInvBstDerateStep;
					}
					else
					{
						stLoadLimit.dActPowerInvBoostTempLimit = stACSample.dActivePower-stMachineCfg.wInvBstDerateStep;
					}
				}
				else
				{
					stLoadLimit.dActPowerInvBoostTempLimit = 0;
				}
			}
			else if(stDCSample.wInvTemp < (stMachineCfg.wInvTempDerate-10) && stDCSample.wBoostTemp < (stMachineCfg.wBoostTempDerate-10))
			{
				stLoadLimit.dActPowerInvBoostTempLimit = stLoadLimit.dActPowerInvBoostTempLimit + stMachineCfg.wInvBstDerateBackStep;
			}
		}

		/***************************************AM High Temperature Derating************************************/
		suwAmDeratCnt++;
		if(suwAmDeratCnt>=300)		// 300*20ms = 6000ms
		{
			suwAmDeratCnt = 0;
			if(stDCSample.wAmTemp > stMachineCfg.wAmbTempDerate)
			{
				if(stACSample.dActivePower > (stLoadLimit.dActivePower/10))	// RealPower > RatedPower*10%
				{
					if(stACSample.dActivePower > stLoadLimit.dActPowerLimitOutput)
					{
						stLoadLimit.dActPowerAmTempLimit = stLoadLimit.dActPowerLimitOutput-stMachineCfg.wAmbDerateStep;
					}
					else
					{
						stLoadLimit.dActPowerAmTempLimit = stACSample.dActivePower-stMachineCfg.wAmbDerateStep;
					}
				}
				else
				{
					stLoadLimit.dActPowerAmTempLimit = 0;
				}
			}
			else if(stDCSample.wAmTemp < (stMachineCfg.wAmbTempDerate-5))
			{
				stLoadLimit.dActPowerAmTempLimit = stLoadLimit.dActPowerAmTempLimit + stMachineCfg.wAmbDerateBackStep;
			}
		}
	}
	else
	{
		suwInvBoostDeratCnt=0;
		suwAmDeratCnt=0;
		stLoadLimit.dActPowerInvBoostTempLimit = stLoadLimit.dActivePower;
		stLoadLimit.dActPowerAmTempLimit = stLoadLimit.dActivePower;
	}

    if(stLoadLimit.dActPowerInvBoostTempLimit > stLoadLimit.dActivePower)
	{
		stLoadLimit.dActPowerInvBoostTempLimit = stLoadLimit.dActivePower;
	}

    if(stLoadLimit.dActPowerAmTempLimit > stLoadLimit.dActivePower)
   	{
   		stLoadLimit.dActPowerAmTempLimit = stLoadLimit.dActivePower;
   	}
}


// AC Voltage High Derating
static void ACVoltHighDerating(void)
{
    static Uint16 uwACOverVoltTmp=0;
	if((ENABLE == stF107Data.unEnable.bit.ACHighVoltDerating)&&(cInverterStatus == eInverterStatus))
	{
	    /**************Insure ACVoltHighDerating Phase Voltage Value = 285V**********************************/
	    if(ODM_CHUANG_WEI == stDspReceData.unSafetyOdm.bit.Manufacturer)
	    {
	        uwACOverVoltTmp = 5023;
	    }
	    else
	    {
	        uwACOverVoltTmp = stF107Data.uwACOverVolt1;
	    }

	    if((stACSample.wLineVoltRmsMax+87)>uwACOverVoltTmp)		// 87/1.73 = 5V
		{
			if(stLoadLimit.dActPowerACHighVoltLimit>200)		// 20w
			{
				stLoadLimit.dActPowerACHighVoltLimit -= 200;
			}
			else
			{
				stLoadLimit.dActPowerACHighVoltLimit = 200;
			}
		}
		else if((stACSample.wLineVoltRmsMax+174)<uwACOverVoltTmp)	// 174/1.73 = 10V
		{
			if(stLoadLimit.dActPowerACHighVoltLimit<stLoadLimit.dActivePower)		// 200w
			{
				stLoadLimit.dActPowerACHighVoltLimit += 500;	// 50W
			}
			else
			{
				stLoadLimit.dActPowerACHighVoltLimit = stLoadLimit.dActivePower;
			}
		}
	}
	else
	{
		stLoadLimit.dActPowerACHighVoltLimit = stLoadLimit.dActivePower;
	}

    if(stLoadLimit.dActPowerACHighVoltLimit > stLoadLimit.dActivePower)
   	{
   		stLoadLimit.dActPowerACHighVoltLimit = stLoadLimit.dActivePower;
   	}
}

// Single Machine AntiFlow Derating
static void AntiFlowDerating(void)
{
    static Uint16 	suwAntiFlowLimitCnt=0,suwAntiFlowRecorveCnt=0;
    int32	dAntiFlowMeterPowerTmp;

	if((ENABLE == stDspReceData.unFuncEnable.bit.ExportLimit)&&(cInverterStatus == eInverterStatus))
	{
		dAntiFlowMeterPowerTmp = (int32)stF107Data.wAntiFlowMeterPower*1000;		// 0.1kw

		if(false==stF107Data.unF107WarnBit.bit.AntiFlowMeterWarn)
		{
			dAntiFlowMeterPowerTmp = -dAntiFlowMeterPowerTmp;
			if(dAntiFlowMeterPowerTmp < 0)
			{
				suwAntiFlowRecorveCnt=0;
				suwAntiFlowLimitCnt++;
				if(suwAntiFlowLimitCnt>=50)	// 50*20ms = 1000ms
				{
					suwAntiFlowLimitCnt = 0;
					stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActPowerLimitOutput+dAntiFlowMeterPowerTmp;
				}
			}
			else if(dAntiFlowMeterPowerTmp > 2000)	// 200w
			{
				suwAntiFlowLimitCnt = 0;
				suwAntiFlowRecorveCnt++;
				if(suwAntiFlowRecorveCnt>=100)	// 100*20ms = 2s
				{
					suwAntiFlowRecorveCnt = 0;
					if(dAntiFlowMeterPowerTmp>=stLoadLimit.dActivePower/10)
					{
						stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActPowerLimitOutput+stLoadLimit.dActivePower/10;
					}
					else if(dAntiFlowMeterPowerTmp>=stLoadLimit.dActivePower/100)
					{
						stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActPowerLimitOutput+stLoadLimit.dActivePower/100;
					}
					else
					{
						stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActPowerLimitOutput+stLoadLimit.dActivePower/1000;
					}
				}
			}
		}
		else
		{
			suwAntiFlowLimitCnt = 0;
			suwAntiFlowRecorveCnt = 0;
			stLoadLimit.dActPowerAntiFlowLimit = dAntiFlowMeterPowerTmp;
		}
	}
	else
	{
		suwAntiFlowLimitCnt = 0;
		suwAntiFlowRecorveCnt = 0;
		stLoadLimit.dActPowerAntiFlowLimit = stLoadLimit.dActivePower;
	}

	UPDNLMT(stLoadLimit.dActPowerAntiFlowLimit, stLoadLimit.dActivePower, AC300W);
}

// Multi-Machine AntiFlow Derating
static void MultiAntiFlowDerating(void)
{
    int16   wMultiAntiFlowRateTmp;
    if((ENABLE == stDspReceData.unFuncEnable.bit.MultiExportLimit)&&(cInverterStatus == eInverterStatus))
    {
        wMultiAntiFlowRateTmp = stF107Data.wMultiAntiFlowRate;      // 0.1%
        if((false==stF107Data.unF107WarnBit.bit.AntiFlowMeterWarn)&&(wMultiAntiFlowRateTmp>=0)&&(wMultiAntiFlowRateTmp<=1000))
        {
            stLoadLimit.dActPowerMultiAntiFlowLimit = (Uint32)(stLoadLimit.dActivePower)*wMultiAntiFlowRateTmp/1000;
        }
        else
        {
        	stLoadLimit.dActPowerMultiAntiFlowLimit = stLoadLimit.dActivePower;
        }
    }
    else
    {
    	wMultiAntiFlowRateTmp = 0;
    	stLoadLimit.dActPowerMultiAntiFlowLimit = stLoadLimit.dActivePower;
    }

	UPDNLMT(stLoadLimit.dActPowerMultiAntiFlowLimit, stLoadLimit.dActivePower, AC300W);
}

// communicate Set Power
static void ComSetDerating(Uint16 uwSetPercent)
{
	if(eInverterStatus != cInverterStatus)
	{
		stLoadLimit.dActPowerSetLimit = 20000;	// 2000w  230-2.89A
	}
	else
	{
		if(uwSetPercent < 1000)
		{
			stLoadLimit.dActPowerSetLimit = (stLoadLimit.dActivePower * uwSetPercent) / 1000;
		}
		else
		{
			stLoadLimit.dActPowerSetLimit = stLoadLimit.dActivePower;
		}
	}

    if(stLoadLimit.dActPowerSetLimit > stLoadLimit.dActivePower)
   	{
   		stLoadLimit.dActPowerSetLimit = stLoadLimit.dActivePower;
   	}
}

static void RempptPowerDerating(void)
{
	static Uint16 suwRempptTimesCntBak = 0;
	static Uint16 suwRempptPowerPercent = 0;
	Uint16 uwRempptTimesTmp = 0;

	uwRempptTimesTmp = stInvPwm.uwPwmDisableTimersCnt;
	if(cInverterStatus == eInverterStatus)
	{
		if(suwRempptTimesCntBak != uwRempptTimesTmp)
		{
			suwRempptPowerPercent = 40;			// default: 40/512=7%*Power
		}
		suwRempptPowerPercent++;
		if(suwRempptPowerPercent >= 512)		// 512 * 20ms = 10.2s
		{
			suwRempptPowerPercent = 512;
		}
	}
	else
	{
		suwRempptPowerPercent = 512;
	}
	suwRempptTimesCntBak = uwRempptTimesTmp;
	stLoadLimit.dActPowerRemmptLimit = (stLoadLimit.dActivePower*suwRempptPowerPercent)>>9;
	// remppt limit active power
	if(stLoadLimit.dActPowerRemmptLimit > stLoadLimit.dActivePower)
	{
		stLoadLimit.dActPowerRemmptLimit = stLoadLimit.dActivePower;
	}
	// remppt limit apparent power
	stLoadLimit.dApparentPowerRemmptLimit = (stLoadLimit.dApparentPower*suwRempptPowerPercent)>>9;
	if(stLoadLimit.dApparentPowerRemmptLimit > stLoadLimit.dApparentPower)
	{
		stLoadLimit.dApparentPowerRemmptLimit = stLoadLimit.dApparentPower;
	}
}

static void InvCurrLimit(void)
{
	int16 wInvActiveCurrRefTmp,wInvReactiveCurrRefTmp;
	int16 uwReactiveCurrStep,uwActiveCurrStep;
	static int16 swInvActiveCurrRef,swInvReactiveCurrRef;
	static int16 swActivePowerPercentAdj,swInvReactiveCurrRefAdj;

	if(STRANDARD_TAIWAI == stDspReceData.unSafetyOdm.bit.SafetyStandard)
	{
		uwReactiveCurrStep = 512;
		uwActiveCurrStep = 256;
	}
	else
	{
		uwReactiveCurrStep = 16;
		uwActiveCurrStep = 16;
	}
	
	if((ENABLE == unSysFlag.bit.InvPwmEnable)&& (cInverterStatus == eInverterStatus))
	{
		if(stACSample.wLineVoltRmsMin>0)
		{
			// active power current limit
			// Curr_dLimit(Q32)	32*1.414/1.732 = 26
			wInvActiveCurrRefTmp = (stLoadLimit.dActPowerLimitOutput*26)/stACSample.wLineVoltRmsMin;
			if((swInvActiveCurrRef+uwActiveCurrStep) < wInvActiveCurrRefTmp)
			{
				swInvActiveCurrRef += uwActiveCurrStep;					// 20ms ~ 0.5625A
			}
			else
			{
				swInvActiveCurrRef = wInvActiveCurrRefTmp;
			}

			// reactive power current limit
			// Curr_qLimit(Q32)	32*1.414/1.732 = 26
			wInvReactiveCurrRefTmp = (stLoadLimit.dReactivePowerRef*26)/stACSample.wLineVoltRmsMin;
			if((swInvReactiveCurrRef+uwReactiveCurrStep)<wInvReactiveCurrRefTmp)
			{
				swInvReactiveCurrRef += uwReactiveCurrStep;		// 20ms ~ 0.5625A
			}
			else if(swInvReactiveCurrRef>(wInvReactiveCurrRefTmp+uwReactiveCurrStep))
			{
				swInvReactiveCurrRef -= uwReactiveCurrStep;		// 20ms ~ 0.5625A
			}
			else
			{
				swInvReactiveCurrRef = wInvReactiveCurrRefTmp;
			}
		}
	}
	else
	{
		swInvReactiveCurrRef = 0;		// default Reactive current:0A
		swInvActiveCurrRef = 64;		// default Active current:2A
	}
	swActivePowerPercentAdj = (int16)(stACSample.dActivePowerAvg*1000/stDspReceData.udOutputActivePower);
	if(swActivePowerPercentAdj<0)
	{
		swActivePowerPercentAdj = 0;
	}
	if(swActivePowerPercentAdj < 100)
	{
		swInvReactiveCurrRefAdj = 15;
	}
	else if(swActivePowerPercentAdj < 500)
	{
		swInvReactiveCurrRefAdj = 39;
	}
	else if(swActivePowerPercentAdj < 900)
	{
		swInvReactiveCurrRefAdj = 34;
	}
	else
	{
		swInvReactiveCurrRefAdj = 30;
	}

	if(stSysCfg.uwFastDCMode == 1)
	{
		swInvReactiveCurrRefAdj = 0;
	}


//	stLoadLimit.wVoltReactiveCurrLimit = swInvReactiveCurrRef+40+stDebug.SetData.wIqRef;		// XG25-40KW(Offset_45) XG50-70KW(Offset_0)
	stLoadLimit.wVoltReactiveCurrLimit = swInvReactiveCurrRef+swInvReactiveCurrRefAdj+stDebug.SetData.wIqRef;        // XG25-40KW(Offset_45) XG50-70KW(Offset_0)
	stLoadLimit.wVoltActiveCurrLimit = swInvActiveCurrRef;
}

// uwLoadSpeed: uwLoadSpeed/min
static void LoadSpeedDerating(Uint16 uwLoadSpeed, Uint16 uwShutDownSpeed)
{
	static Uint16 suwLoadSpeed100msCnt;

	if(eInverterStatus != cInverterStatus)
	{
		stLoadLimit.dActPowerLoadSpeedLimit = 20000;	// 2000w  230-2.89A
	}
	else
	{
		suwLoadSpeed100msCnt++;
		if(suwLoadSpeed100msCnt>=5)		// 20ms * 5 = 100ms
		{
			suwLoadSpeed100msCnt = 0;
			if((OFF == stF107Data.unSet.bit.TrunOnOff)&&(uwShutDownSpeed!=0))
			{
				stLoadLimit.dActPowerLoadSpeedLimit = stLoadLimit.dActPowerLoadSpeedLimit - ((int32)((int64)stLoadLimit.dActivePower*uwShutDownSpeed/600000));		// 100*60s(base on 100ms) * speed
			}
			else
			{
				stLoadLimit.dActPowerLoadSpeedLimit = stLoadLimit.dActPowerLoadSpeedLimit + ((int32)((int64)stLoadLimit.dActivePower*uwLoadSpeed/600000));		// 100*60s(base on 100ms) * speed

			}
		}
	}
	if(stLoadLimit.dActPowerLoadSpeedLimit > stLoadLimit.dActivePower)
	{
		stLoadLimit.dActPowerLoadSpeedLimit = stLoadLimit.dActivePower;
	}
	if(stLoadLimit.dActPowerLoadSpeedLimit < 1000)		// < 100w
	{
		stLoadLimit.dActPowerLoadSpeedLimit = 1000;		// =100w
	}
}


// uwLoadSpeed: uwLoadSpeed/min
static void ATEAdjustDerating(Uint16	uwAdjustMode)
{
	static Uint16 suwLoadSpeed100msCnt;
	static int32 sdActPowerATEAdjustRate;

	if(uwAdjustMode==ATE_ADJUST_30)
	{
		sdActPowerATEAdjustRate = stLoadLimit.dActivePower*3/10;		// 30%
	}
	else
	{
		sdActPowerATEAdjustRate = stLoadLimit.dActivePower*7/10;		// 70%
	}
	stDebug.ReadData.uwDebug46 = sdActPowerATEAdjustRate/10;
	if(eInverterStatus != cInverterStatus)
	{
		stLoadLimit.dActPowerATEAdjustLimit = 20000;	// 2000w  230-2.89A
	}
	else
	{
		suwLoadSpeed100msCnt++;
		if(suwLoadSpeed100msCnt>=5)		// 20ms * 5 = 100ms
		{
			suwLoadSpeed100msCnt = 0;
			if(stLoadLimit.dActPowerATEAdjustLimit<=sdActPowerATEAdjustRate)
			{
				stLoadLimit.dActPowerATEAdjustLimit = stLoadLimit.dActPowerATEAdjustLimit + (stLoadLimit.dActivePower * 600/60000);		// 10s full active power
			}
			else
			{
				stLoadLimit.dActPowerATEAdjustLimit = sdActPowerATEAdjustRate;
			}
		}
	}
	if(stLoadLimit.dActPowerATEAdjustLimit > stLoadLimit.dActivePower)
	{
		stLoadLimit.dActPowerATEAdjustLimit = stLoadLimit.dActivePower;
	}
	if(stLoadLimit.dActPowerATEAdjustLimit < 1000)		// < 100w
	{
		stLoadLimit.dActPowerATEAdjustLimit = 1000;		// =100w
	}
}

static void LimitOutputActivePower(void)
{
	Uint32 udActPowerLimitTmp=0;
	Uint16 uwDeratingModeTmp=0;
        Uint32 udActPowerMinLimitTmp=0;

	uwDeratingModeTmp = 0;			// Derating model clear 0
	udActPowerLimitTmp = stLoadLimit.dActivePowerActual;
	if(udActPowerLimitTmp > stLoadLimit.dActPowerPVHighVoltLimit)
	{
		uwDeratingModeTmp = PVVOLT_HIGH_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerPVHighVoltLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerInvBoostTempLimit)
	{
		uwDeratingModeTmp = BOOST_TEMP_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerInvBoostTempLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerAmTempLimit)
	{
		uwDeratingModeTmp = AM_TEMP_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerAmTempLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerSetLimit)
	{
		uwDeratingModeTmp = SETTING_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerSetLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerRemmptLimit)
	{
		uwDeratingModeTmp = REMPPT_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerRemmptLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerReactiveLimit)
	{
		uwDeratingModeTmp = REACTIVE_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerReactiveLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerLoadSpeedLimit
	&& ((DISABLE == stDspReceData.unFuncEnable.bit.ExportLimit) && (DISABLE == stDspReceData.unFuncEnable.bit.MultiExportLimit)))
	{
		uwDeratingModeTmp = LOADSPEED_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerLoadSpeedLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerACHighVoltLimit)
	{
		uwDeratingModeTmp = ACVOLT_HIGH_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerACHighVoltLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerATEAdjustLimit)
	{
		uwDeratingModeTmp = ATE_ADJUST_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerATEAdjustLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerAntiFlowLimit)
	{
		uwDeratingModeTmp = ANTI_FLOW_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerAntiFlowLimit;
	}

	if(udActPowerLimitTmp > stLoadLimit.dActPowerMultiAntiFlowLimit)
	{
	    uwDeratingModeTmp = MULTI_ANTI_FLOW_DERATING;
	    udActPowerLimitTmp = stLoadLimit.dActPowerMultiAntiFlowLimit;
	}
	if(udActPowerLimitTmp > stLoadLimit.dActPowerOverFreqLimit)
	{
		uwDeratingModeTmp = OVER_FREQ_DERATING;
		udActPowerLimitTmp = stLoadLimit.dActPowerOverFreqLimit;
	}

	stLoadLimit.uwDeratingMode = uwDeratingModeTmp;

    if(udActPowerLimitTmp>=udActPowerMinLimitTmp)		// 200w
	{
    	stLoadLimit.dActPowerLimitOutput = udActPowerLimitTmp;
	}
	else
	{
		stLoadLimit.dActPowerLimitOutput = udActPowerMinLimitTmp;		// 200w
	}
    stLoadLimit.dActPowerOutputRef = (stLoadLimit.dActPowerLimitOutput/10);
}

static void LimitOutputApparentPower(void)
{
	Uint32 udApparentPowerLimitTmp=0;
	int16	wPVVoltMinValueTmp;

	udApparentPowerLimitTmp = stLoadLimit.dApparPowerActual;
	/************PV high voltage limits apparent power**************/
	if(udApparentPowerLimitTmp > stLoadLimit.dApparentPowerPVHighVoltLimit)
	{
		udApparentPowerLimitTmp = stLoadLimit.dApparentPowerPVHighVoltLimit;
	}

	/************Apparent power calculated according to PV voltage**************/
	wPVVoltMinValueTmp = MAX4(stDCSample.wPVVoltAvg[PVA],stDCSample.wPVVoltAvg[PVB],
							  stDCSample.wPVVoltAvg[PVC],stDCSample.wPVVoltAvg[PVD]);
	stLoadLimit.dApparentPowerPVLowVoltLimit = ((int32)stLoadLimit.wBTCurrLimitMax*wPVVoltMinValueTmp * stSysCfg.uwPVNumber >>5);		// V*I * PVNo
	if(stLoadLimit.dApparentPowerPVLowVoltLimit > stLoadLimit.dApparentPower)
	{
		stLoadLimit.dApparentPowerPVLowVoltLimit = stLoadLimit.dApparentPower;
	}

	/************PV low voltage limits apparent power**************/
	if(udApparentPowerLimitTmp > stLoadLimit.dApparentPowerPVLowVoltLimit)
	{
		udApparentPowerLimitTmp = stLoadLimit.dApparentPowerPVLowVoltLimit;
	}

	/************Over Current(Boost or Inv) limits apparent power**************/
	if(udApparentPowerLimitTmp > stLoadLimit.dApparentPowerRemmptLimit)
	{
		udApparentPowerLimitTmp = stLoadLimit.dApparentPowerRemmptLimit;
	}

	stLoadLimit.dApparentPowerLimitOutput = udApparentPowerLimitTmp;

}

void InverterLoadLimit(void)
{
	Uint16 uwPVIndexTmp;
	GridVoltActuraltPower(stACSample.wLineVoltRmsMin);
	if(0 == stIVCurve.unIVCurveBits.bit.StartEnable)
	{
		PVVoltHighDerating(stDCSample.wBusVolt);
	}

	for(uwPVIndexTmp=0; uwPVIndexTmp < PV_MAX_NUM;    uwPVIndexTmp++)
	{
		stLoadLimit.wBTCurrLimit[uwPVIndexTmp] = PVVoltLowCurrLimit(uwPVIndexTmp);       //20210401add,Non-Test
	}
	stLoadLimit.wBTCurrLimitMax = MAX4(stLoadLimit.wBTCurrLimit[PVA],stLoadLimit.wBTCurrLimit[PVB],
									   stLoadLimit.wBTCurrLimit[PVC],stLoadLimit.wBTCurrLimit[PVD]);
	TemperatureHighDerating();
	RempptPowerDerating();
#if F107_BOARD
	if((stF107Data.uwAdjustMode==ATE_ADJUST_30)||(stF107Data.uwAdjustMode==ATE_ADJUST_70))
	{
		ATEAdjustDerating(stF107Data.uwAdjustMode);
		stLoadLimit.dActPowerSetLimit = stLoadLimit.dActivePower;
		stLoadLimit.dActPowerLoadSpeedLimit = stLoadLimit.dActivePower;
	}
	else
	{
		ComSetDerating(stF107Data.uwActiveRate);
		LoadSpeedDerating(stF107Data.uwStartLoadSpeed,stF107Data.uwShutDownSpeed);
		stLoadLimit.dActPowerATEAdjustLimit = stLoadLimit.dActivePower;
	}
#else
	if(OFF==stDebug.SetData.unSetReg.bit.TurnOnOff)
	{
		stSysFaultReg.unFaultBit.bit.TurnOff = 1;
	}
	else
	{
		stSysFaultReg.unFaultBit.bit.TurnOff = 0;
	}
	ComSetDerating(stDebug.SetData.uwActivePercent);
#endif
	ACVoltHighDerating();
	App_FreqDerating();
	AntiFlowDerating();
	MultiAntiFlowDerating();
	LimitOutputActivePower();
	LimitOutputApparentPower();
	InvCurrLimit();
}


