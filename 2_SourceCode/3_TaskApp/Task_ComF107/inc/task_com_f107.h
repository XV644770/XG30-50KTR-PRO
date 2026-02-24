/*
* task_com_f107.h
*
*	Product:			three phase inveter
*	Filename:			task_sci_stm32.h
*	Purpose:			dsp and stm32 communicate
*	Date				Version		Author			Note
*   20/11/18			V1.01		zhaohui			none
*/

#ifndef TASK_COM_F107_H
#define TASK_COM_F107_H

#include "com_data_deal.h"

#define CLR_F107_COM_FAULT_CNT				20		// 100ms * 20 = 2s
#define SET_F107_COM_FAULT_CNT				150		// 100ms * 100 = 10s

#define	GRID_PROTECT_LIMIT(a,b) ((a >= b) ? (a-b):(a))

#define	ADJ_RATIO_MAX	12500		// 1.25
#define	ADJ_RATIO_MIN	7500		// 0.75
#define	ADJ_RATIO_DEFAULT	10000	// 1

typedef union
{
    Uint16  all;

    struct
    {
    	Uint16	LoadingSlope		:1;		// bit0 Loading Slope Enable
		Uint16	ACHighVoltDerating	:1;		// bit1 AC Voltage High->Derating
		Uint16	NEVoltCheck			:1;		// bit2 NE Voltage Check
		Uint16	LVRT				:1;		// bit3 low voltage through

		Uint16	ISLAND				:1;		// bit4 island protect enable
		Uint16	Freq50_60Hz			:1;		// bit5 0:50Hz 1:60Hz
		Uint16	OverFreqDerate		:1;		// bit6 Over Freqency derating
		Uint16	UnderFreqLoad		:1;		// bit7 Under Freqency loading

		Uint16	OverLoad			:1;		// bit8 Over Loading
		Uint16 	rsv9_15				:7;     // bit9-15 reserve
    }bit;
}UN_F107_ENABLE_REG;

typedef union
{
	Uint16 all;

	struct
	{
		Uint16	F107RxDspSci		:1;		// Fault-01
		Uint16	Reserve02_8			:7;		// Bit2-7 reserve
		Uint16	useless8_15			:8; 	// 8-15 Useless
	}bit;
}UN_F107_FAULT_BIT;

typedef union
{
	Uint16 all;

	struct
	{
		Uint16	AntiFlowMeterWarn	:1;		// Fault-01
		Uint16	Reserve02_8			:7;		// Bit2-7 reserve
		Uint16	useless8_15			:8; 	// 8-15 Useless
	}bit;
}UN_F107_WARN_BIT;

typedef union
{
    Uint16  all;

    struct
    {
    	Uint16	TrunOnOff		:1;		// 1:turn on inv	0:turn off inv
    	Uint16	FlashReady		:2;		// 0:no update 1:update dsp
    	Uint16	FanSelfCheckCmd	:1;		// 0:no fan check	1:fan check
    	Uint16	ACVoltLevel		:2;		// 0:no update 1:update dsp
    	Uint16	rsv				:10;
    }bit;
}UN_F107_SET_REG;

// Config:protect  + environment
typedef union
{
	Uint16		all;
	struct
	{
		Uint16 ISO 		:1;		// 0 ISO enable
		Uint16 GFCI 		:1;		// 1 GFCI enable
		Uint16 DCI 		:1; 	// 2 DCI enable
		Uint16 NonStand	:1;     // 3 NonStandard
		Uint16 rsv4_7	:4;     // 4-7 NonStandard
		Uint16 useless8_15	:8; // 8-15 Useless
	} bit;
} UN_PROTECT_BIT;

// Config:Environment Function
typedef union
{
	Uint16		all;
	struct
	{
		Uint16 ACVoltStand 	:2;		// bit0-1 	00:220V	01:400V	02:480V
		Uint16 Neutral 		:1;		// bit2 Neutral
		Uint16 rsv1_7		:5;     	// 3-7 reserve
		Uint16 useless8_15	:8; // 8-15 Useless
	} bit;
} UN_ENVRMT_BIT;

// Config:protect  + environment
typedef struct
{
	  UN_PROTECT_BIT		unProtectBit;
	  UN_ENVRMT_BIT			unEnvrmtBit;
}ST_PROTECT_ENVRMT;		// Protect+environment

// Config:safety and manufacturer message
typedef union
{
	Uint16		all;
	struct
	{
		Uint16	Manufacturer	:8;		// manufacturer
		Uint16	SafetyStandard	:8;		// safety standard
	} bit;
}UN_SAFETY_ODM;


typedef enum
{
    SAVESTART = 0,
    SAVEING,
    SAVESUCCESS,
    FLASH_ERASEFAIL,
    FLASH_SAVEFAIL,
}EN_WAVEDEAL_STATE;

// Config:Function Message
typedef union
{
	Uint16		all;
	struct
	{
		Uint16	ExportLimit		:1;		// bit0 exportlimit
		Uint16	StringMonitor	:1;		// bit1 StringMonitoring
		Uint16	PID				:1;		// bit2 pid
		Uint16	DC_SPD			:1;		// bit3 DC SPD
		Uint16	AC_SPD			:1;		// bit4 DC SPD
		Uint16	RelayCheck		:1;		// bit5 Single & Double Relay Check	1:Double Relay	0:Single Relay
		Uint16	IGBTCheck		:1;		// bit6 IGBT Check--XG100KTR-136KTR
		Uint16	Consistency_Protect	:1;	// bit7 Enable Consistency Protect--XG100KTR-136KTR
		Uint16  MultiExportLimit    :1; // bit8 Multi-Machine AntiFlow
		Uint16 	AFCIEnable		:1;		// bit9
		Uint16 	NPEEnable		:1;		// bit10  Enable N-PE fault check
		Uint16  SVG              :1;     // bit11  Enable SVG check
		Uint16  AutotestQuick    :1;     // bit12  Enable Auto test choose for short time
		Uint16  StrCurr_Choose   :1;     // bit13 Str Curr choose ,0:2string;1:3sting;
		Uint16 	rsv11_15		 :2;	// bit14-bit15 reserve
	} bit;
}UN_FUNCTION_ENABLE;

typedef struct
{
	/*****************F107 TX Frame0*******************/
	Uint16	uwPFCtrlMode;
	int16	wCosPhi;
	Uint16	uwStartLoadSpeed;
	Uint16	uwQvHighVoltEndPoint;
	Uint16	uwQvHighVoltStartPoint;
	Uint16	uwQvHighVoltReactivePrt;
	Uint16	uwQvLowVoltStartPoint;
	Uint16	uwQvLowVoltEndPoint;
	Uint16	uwQvLowVoltReactivePrt;

	Uint16	uwQvPFPvHighVoltPrt;
	Uint16	uwQvPFAutoAdjustPower;
	Uint16	uwQvPFAutoAdjustPF;
	
	UN_F107_ENABLE_REG	unEnable;
	Uint16	uwOverFreqDeratePoint;
    Uint16  uwFreqDeratEndPoint;
	Uint16	uwFreqPowerChangeRate;
	Uint16	uwHighVoltDerateStart;
	Uint16	uwHighVoltDerateEnd;
	Uint16	uwLowVoltDerateStart;
	Uint16	uwLowVoltDerateEnd;
	Uint16 	uwRestartLoadSpeed;
	Uint16  uwShutDownSpeed;
	UN_F107_SET_REG		unSet;

	/*****************F107 TX Frame1*******************/
	/***AC Adjust Ratio to advance Sample precision****/
	int16   wRSVoltAdjRatio;
	int16   wSTVoltAdjRatio;
	int16   wTRVoltAdjRatio;
	int16   wRNCurr30AdjRatio;
	int16   wSNCurr30AdjRatio;
	int16   wTNCurr30AdjRatio;
	int16   wRNCurr70AdjRatio;
	int16   wSNCurr70AdjRatio;
	int16   wTNCurr70AdjRatio;
	int16   wActPower30AdjRatio;
	int16   wActPower70AdjRatio;
	Uint16	uwSoftWareDebug1;
	Uint16	uwSoftWareDebug2;
	
	/*****************F107 TX Frame2*******************/
    UN_SAFETY_ODM	unSafetyOdm;
    ST_PROTECT_ENVRMT stProtectEnvrmt;	// 10
	UN_FUNCTION_ENABLE unFuncEnable;
	Uint16 uwSettingPower;

	Uint16 uwMpptModeSet;
	Uint16 uwAdjustMode;

	Uint16 uwPVStartUnderVolt;
	Uint16 uwACStartOverVolt;
	Uint16 uwACStartUnderVolt;
	Uint16 uwACStartOverFreq;
	Uint16 uwACStartUnderFreq;
	Uint16 uwStartDelayTime;
	Uint16 uwReStartDelayTime;

	Uint16 uwACUnderVolt1;
	Uint16 uwACOverVolt1;
	Uint16 uwACUnderVolt1Time;
	Uint16 uwACOverVolt1Time;

	Uint16 uwACUnderVolt2;
	Uint16 uwACUnderVolt2Time;
	Uint16 uwACOverVolt2;
	Uint16 uwACOverVolt2Time;
	Uint16 uwAC10MinOverVolt;

    Uint16 uwACUnderVolt3;
    Uint16 uwACUnderVolt3Time;
	/*****************F107 TX Frame3*******************/
	Uint16 uwACUnderFreq1;
	Uint16 uwACUnderFreq2;
	Uint16 uwACUnderFreq1Time;
	Uint16 uwACUnderFreq2Time;

	Uint16 uwACOverFreq1;
	Uint16 uwACOverFreq2;
	Uint16 uwACOverFreq1Time;
	Uint16 uwACOverFreq2Time;
	Uint16 uwArcFaultLimit;
	Uint16 uwAfciSetCmd;

    Uint16 uwAFCI_StartFreq;             //AFCIÆµ¶Î¿ªÊ¼Öµ
    Uint16 uwAFCI_EndFreq;               //AFCIÆµ¶Î½áÊøÖµ
    Uint16 uwAFCI_MagRatio;              //AFCIµ±Ç°ÆµÆ×ºÍÂË²¨ÆµÆ×µÄ±È½Ï²îÒì±¶Êý
    Uint16 uwAFCI_FreqRatio;             //AFCIµç»¡¼ÆÊýÊýÁ¿ãÐÖµ
    Uint16 uwAFCI_BinRatio;              //AFCI¸ßÆµÆ×¼ÆÊýÊýÁ¿ãÐÖµ
    Uint16 uwAFCI_MagThld_PowerOn_H;     //AFCIÆµÆ×¸ß¶ÈãÐÖµ

    Uint16 uwSelfCheckEnable_New;        //AFCI×Ô¼ìÊ¹ÄÜ 0:×Ô¶¯×Ô¼ì 1:ÊÖ¶¯×Ô¼ì
    Uint16 uwAFCI_Set_FaultCntTime_S;    //AFCI¹ÊÕÏ×Ô¶¯Çå³ýÊ±¼ä(×Ô¶¯ÖØÁ¬)
    Uint16 usAFCI_AuthenFlag;            //AFCIÈÏÖ¤¹¦ÄÜÊ¹ÄÜ
    Uint16 uwAFCICMD;                    //AFCI¹ÊÕÏÇå³ý±êÖ¾Î»
    Uint16 uwAFCI_ManualResetFlag;       //AFCIÊÖ¶¯¸´Î»±êÖ¾Î»

	Uint16 uwDCIOverCurrTime;

	/*****************F107 TX Frame4*******************/
	int16	wPVVoltAdjRatio[PV_MAX_NUM];
	int16	wBTCurr30PctAdjRatio[PV_MAX_NUM];
	int16	wBTCurr70PctAdjRatio[PV_MAX_NUM];

    Uint16 uwWaveDealFrame;
    EN_WAVEDEAL_STATE uwWaveDealState;
    Uint16  uwFaultWaveChannelNum1;
    Uint16  uwFaultWaveChannelNum2;
    Uint16  uwFaultWaveChannelNum3;
    Uint16  uwFaultWaveChannelNum4;
    Uint16  uwFaultWaveChannelNum5;
    Uint16  uwFaultWaveChannelNum6;
    Uint16  uwFaultWaveChannelNum7;
    Uint16  uwFaultWaveChannelNum8;

    Uint16  uwRecordHandEnable;
    Uint16  uwRecordFaultEnable;
	/*****************F107 TX PUBLIC*******************/
	UN_F107_FAULT_BIT	unF107FaultBit;
	UN_F107_WARN_BIT	unF107WarnBit;
    Uint16 uwActiveRate;
    int16	wAntiFlowMeterPower;		// 0.1kw
    int16 wReactiveRate;
    int16   wMultiAntiFlowRate;      // 0.1%

    int16   wISOValue;      //
    int16   uwDeratPFVoltPrt;  

    Uint16  wACVoltLowThrough;
    Uint16  wACVoltHighThrough;
    Uint16  wACVoltLowThroughRecover;
    Uint16  wACVoltHighThroughRecover;

    Uint16  swP_PFLineVoltPercentStart;
    Uint16  swP_PFLineVoltPercentRecover;
    Uint16  swP_PFActPowerPercentStart;
 
}ST_F107_DATA;   //The structure for Communication setting parameters

typedef struct
{
	/*****************F107 TX Frame2*******************/
    UN_SAFETY_ODM	unSafetyOdm;
    ST_PROTECT_ENVRMT stProtectEnvrmt;	// 10
	UN_FUNCTION_ENABLE unFuncEnable;
	Uint16	uwSettingPower;

    Uint16	uwACNormalVolt; //AC normal voltage value
	Uint32	udOutputActivePower;
	Uint32	udOutputApparentPower;

	int32	wGFCIRmsFault;
    int32   wGFCIRmsFaultTmp;
    int16   wGFCIRmsFaultDerate;

}ST_DSP_RECE_DATA;

extern ST_DSP_RECE_DATA stDspReceData;
extern ST_F107_DATA stF107Data;

/**************SCI Tx/Rx Buffer**************************/
typedef	struct{
	Uint16 uwComReceFrameEnd;
	Uint16 uwRxBuf[DSP_RX_SIZE];
	Uint16 uwRxBufBak[DSP_RX_SIZE];
	Uint16 uwTxBuf[DSP_TX_SIZE];
	Uint16 uwTxBufBak[DSP_TX_SIZE];
	Uint16 uwRxDataLen;
	Uint16 wSciDVICommand;
}ST_DSP_UART;

extern ST_DSP_UART stDspUart;

extern void ComF107Task(void);
extern void ComF107ParaInit(void);

#endif /* TASK_COM_F107_H */
