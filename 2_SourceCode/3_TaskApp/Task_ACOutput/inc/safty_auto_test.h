/*
 * safty_auto_test.h
 *
 *  Created on: 2023年1月31日
 *      Author: Administrator
 */


#ifndef SAFTY_AUTO_TEST_H
#define SAFTY_AUTO_TEST_H

#define		AT_STOP			0
#define		AT_1P10Un_R		1 		//Maximum voltage (59.S1)
#define		AT_1P10Un_S		2 		//Maximum voltage (59.S1)
#define		AT_1P10Un_T		3 		//Maximum voltage (59.S1)
#define		AT_0P85Un_R		4		//Undervoltage (27.S1)
#define		AT_0P85Un_S		5		//Undervoltage (27.S1)
#define		AT_0P85Un_T		6		//Undervoltage (27.S1)

#define		AT_50P5Hz			7		//Maximum frequency(81>.S1)
#define		AT_49P5Hz			8		//Minimum frequency(81<.S1)

#define		AT_1P15Un_R		9		//Maximum voltage (59.S2)
#define		AT_1P15Un_S		10		//Maximum voltage (59.S2)
#define		AT_1P15Un_T		11		//Maximum voltage (59.S2)

#define		AT_0P40Un_R		12		//Undervoltage (27.S2)
#define		AT_0P40Un_S		13		//Undervoltage (27.S2)
#define		AT_0P40Un_T		14		//Undervoltage (27.S2)

#define		AT_51P5Hz			15		//Maximum frequency(81>.S2) 
#define		AT_47P5Hz			16		//Minimum frequency(81<.S2) 

/*#define		AT_1P10Un		1 		//Maximum voltage (59.S1)
#define		AT_0P85Un		2		//Undervoltage (27.S1)
#define		AT_50P5Hz		3		//Maximum frequency(81>.S1)
#define		AT_49P5Hz		4		//Minimum frequency(81<.S1)
#define		AT_1P15Un		5		//Maximum voltage (59.S2)
#define		AT_0P40Un		6		//Undervoltage (27.S2)
#define		AT_51P5Hz		7		//Maximum frequency(81>.S2) 
#define		AT_47P5Hz		8		//Minimum frequency(81<.S2) 
*/
#define		cTestWaiting		0	
#define		cTestRunning		1
#define		cTestOK				2
#define		cTestFail			3
#define		cTestAllFail			4
#define		cAllTestOK			5

#define		PROCES_STOP		0	
#define		PROCES_WAIT		1	
#define		PROCES_WORK		2	

#define		cTestTimeOver		200


#define		cDelay10s_20ms		501
#define		cDelay3Mins_20ms	9001


typedef struct
{
	Uint16 uwEnableFlag;
	Uint16 uwTestProcess;	
	Uint16 uwStep;	
	Uint16 uwStatus;	
	Uint16 uwResult;	
	int16 wBreakCnt;	
	Uint16 uwBreakTime;	
	float uwFaultValue;	
	float uwVFValue;
	Uint16 uwDefaultValue;
	int16 wWaitTime;	
	int16 uwStopShiftFlag;
	Uint16 uwVFDefaultValue;	
	Uint16 uwLocalCMD;	
	Uint16 uwTripValue;
	Uint16 uwProcess;
	Uint16 uwGridVoltJudge;
	Uint16 uwBreakTimeAdj;	


}ST_AUTO_TEST;

extern ST_AUTO_TEST  stautotest;

extern void sAutoTestUpShift(void);
extern void OPTask_AutoTest(void);
extern void stautotestInit(void);

#endif 
