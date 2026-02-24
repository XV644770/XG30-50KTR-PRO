/*
 * safty_dci_check.c
 *
 *  Created on: 2021年3月11日
 *      Author: Administrator
 */

#include "dsp_user_include.h"

int16 wRDCI_PI,wSDCI_PI;
int16 wDCI_PI_SET;
int16 wDCI_Injection_Enable;

void DCIAdjust(void)
{
	static int32 sdRDCISoftSum,sdSDCISoftSum;
	static Uint16 uwDCIAdjustCnt;
	static Uint16 uwDCICalcCnt;
	static int16 wRDCISoftAvg,wSDCISoftAvg;
	static int16 swRDCI_PIBak,swSDCI_PIBak;
	if((cInverterStatus == eInverterStatus)&&(DISABLE == wDCI_Injection_Enable))
	{
		if(uwDCIAdjustCnt<=1500)		// 1500*20 ms = 30s
		{
			uwDCIAdjustCnt++;
			uwDCICalcCnt = 0;
			swRDCI_PIBak = 0;
			swSDCI_PIBak = 0;
			wRDCI_PI = 0;
			wSDCI_PI = 0;
			return ;
		}

		sdRDCISoftSum += stACSample.SoftDCI.wRN;
		sdSDCISoftSum += stACSample.SoftDCI.wSN;

		uwDCICalcCnt++;
		if(uwDCICalcCnt>=4)
		{
			wRDCISoftAvg = (sdRDCISoftSum/uwDCICalcCnt);
			wSDCISoftAvg = (sdSDCISoftSum/uwDCICalcCnt);
			uwDCICalcCnt = 0;
			wRDCISoftAvg -= 75;
			//wSDCISoftAvg += 25;

			sdRDCISoftSum = 0;
			sdSDCISoftSum = 0;

			if(stACSample.dActivePower > 5000)	// 500w
			{
				
				if(wRDCISoftAvg > 150)		// 100mA
					swRDCI_PIBak -= 3;
				else if(wRDCISoftAvg > 120)		// 100mA
					swRDCI_PIBak -= 2;
				else if(wRDCISoftAvg>60)		// 30mA
					swRDCI_PIBak-=1;
				else if(wRDCISoftAvg<=-150)
					swRDCI_PIBak+=3;
				else if(wRDCISoftAvg<=-120)
					swRDCI_PIBak+=2;
				else if(wRDCISoftAvg<=-60)
					swRDCI_PIBak+=1;

				if(wSDCISoftAvg > 150)
					swSDCI_PIBak -=3;
				else if(wSDCISoftAvg > 80)
					swSDCI_PIBak -=2;
				else if(wSDCISoftAvg > 30)
					swSDCI_PIBak -=1;
				else if(wSDCISoftAvg < -150)
					swSDCI_PIBak+=3;
				else if(wSDCISoftAvg < -80)
					swSDCI_PIBak+=2;
				else if(wSDCISoftAvg < -30)
					swSDCI_PIBak+=1;


				if(swRDCI_PIBak>=50)
					swRDCI_PIBak=50;
				else if(swRDCI_PIBak<=-50)
					swRDCI_PIBak=-50;

				if(swSDCI_PIBak>=50)
					swSDCI_PIBak=50;
				else if(swSDCI_PIBak<=-50)
					swSDCI_PIBak=-50;

				wRDCI_PI = swRDCI_PIBak;
				wSDCI_PI = swSDCI_PIBak;
			}
		}
	}
	else
	{
		uwDCICalcCnt = 0;
		uwDCIAdjustCnt = 0;
		sdRDCISoftSum = 0;
		sdSDCISoftSum = 0;
		swRDCI_PIBak = 0;
		swSDCI_PIBak = 0;
		
		if(DISABLE == wDCI_Injection_Enable)
		{
			wRDCI_PI=0;
			wSDCI_PI=0;
		}
		else
		{
			if(wDCI_PI_SET>=300)
			{
				wRDCI_PI = 300;
				wSDCI_PI = 300;
			}
			else
			{
				wRDCI_PI = wDCI_PI_SET;
				wSDCI_PI = wDCI_PI_SET;
			}

		}

	}
}

void DCIProtection(void)
{
	static Uint16 uwDCIFaultCnt=0;
	static Uint16 uwDCCalcCnt200ms=0;
	static Uint16 uwDCCalcCnt1s=0;
	static Uint16 uwDCCalcTemp=0;
	static Uint16 uwDCIFaultRecoverCnt=0;
	if((cInverterStatus == eInverterStatus)&&(ENABLE == stDspReceData.stProtectEnvrmt.unProtectBit.bit.DCI)&&(STRANDARD_ITALY_CEIO16==stDspReceData.unSafetyOdm.bit.SafetyStandard))
	{
		if(uwDCIFaultCnt<=1500)		// 1500*20 ms = 30s
		{
			uwDCIFaultCnt++;
			uwDCCalcCnt200ms = 0;
			uwDCCalcCnt1s = 0;
			uwDCIFaultRecoverCnt = 0;
			
			return ;
		}
		//uwDCCalcTemp =stLoadLimit.dApparPowerActual/2300/3*0.005*1000;//In=S/V/3*0.5%=25000/2300/3*0.005*1000=181mA
		uwDCCalcTemp =stLoadLimit.dApparPowerActual*30>>12;
		if(stACSample.dActivePower > 5000)	// 500w
		{
			if(stSysFaultReg.unDCIIFault.bit.RDCISoftWare ==0)
			{			  
				 if((abs(stACSample.HardDCI.wRN)> 10000)||(abs(stACSample.HardDCI.wSN)> 10000))		// 1000mA
				{				
					uwDCCalcCnt200ms++;
					uwDCCalcCnt1s++;
					if(uwDCCalcCnt200ms>=9) //< 20ms*10=200ms
					{
						uwDCCalcCnt200ms = 0;
						uwDCCalcCnt1s = 0;
						stSysFaultReg.unDCIIFault.bit.RDCISoftWare =1;
						stSysFaultReg.unFaultBit.bit.DCIOverCurr = 1;
					}
				}
				else if((abs(stACSample.HardDCI.wRN)> uwDCCalcTemp)||(abs(stACSample.HardDCI.wSN)> uwDCCalcTemp))//181mA
				{
					uwDCCalcCnt200ms=0;
					uwDCCalcCnt1s++;
					if(uwDCCalcCnt1s>=45) // <50ms*20=1000ms
					{
						uwDCCalcCnt1s = 0;
						stSysFaultReg.unFaultBit.bit.DCIOverCurr = 1;
						stSysFaultReg.unDCIIFault.bit.RDCISoftWare = 1;
					}
				}
				else
				{
					uwDCCalcCnt200ms = 0;
					uwDCCalcCnt1s = 0;
				}
			}
			
		}
	}
	else
	{
		uwDCIFaultCnt = 0;
		uwDCCalcCnt200ms = 0;
		uwDCCalcCnt1s = 0;
		if(stSysFaultReg.unDCIIFault.bit.RDCISoftWare ==1)
		{
			uwDCIFaultRecoverCnt++;
			if(uwDCIFaultRecoverCnt >250)	// 10S
			{
				uwDCIFaultRecoverCnt = 0;
				stSysFaultReg.unDCIIFault.bit.RDCISoftWare = 0;
				stSysFaultReg.unDCIIFault.bit.SDCISoftWare =0;
				stSysFaultReg.unDCIIFault.bit.TDCISoftWare =0;
				stSysFaultReg.unFaultBit.bit.DCIOverCurr = 0;
			}
		}
	}
}

