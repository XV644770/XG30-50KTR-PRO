/*
* task_com_debug.h
*
*	Product:			three phase inveter
*	Filename:			system_periph_init.h
*	Purpose:			Stm32 system init
*	Date				Version		Author			Note
*   20/11/18			V1.01		csx		none
*/

#ifndef TASK_COM_DEBUG_H
#define TASK_COM_DEBUG_H

#define DEBUG_ADDRESS		56

#define RS485_RX_SIZE	264		// MAX : COM + CODE + REG_H + REG_L + LEN_H + LEN_L +  DATA_LEN + D_H + D_L + CRC(2)
#define RS485_TX_SIZE	1024		// COM + CODE + REG_H + REG_L + DYTE + DATA(0-255) + CRC_H + CRC_L

#define WAVE_POINTNUM_MAX       1000
#define WAVE_RECORD_DELAY_POINT_NUM  250
#define FAULT_RECORD_RECOVER_TIME  1200		// 1200*50ms = 60s
#define WAVE_SEND_UINT16_MAX    250
#define WAVE_CHANNELNUM_MAX    8

#define IPMOS_DEBUG         0
#define CHANNELNUMMAX       40
/**************SCI Tx Status**************************/
typedef	struct{
	Uint16 bTxBuf[RS485_TX_SIZE];
	Uint16 bTxBufBak[RS485_TX_SIZE];
	Uint16 bRxBuf[RS485_RX_SIZE];
	Uint16 bRxBufBak[RS485_RX_SIZE];
}ST_RS485_UART4;
extern ST_RS485_UART4	stRs485Uart4;

typedef union
{
    Uint16  all;
    struct
    {
    	Uint16 TurnOnOff           		:1;	// bit0
    	Uint16 NLineEnable          		:1;	// bit1
    	Uint16 InvPwmLoop			:1;	// bit2
    	Uint16 BoostPwmLoop			:1;	// bit3
    	Uint16 PWMMode				:1;	// bit4
    	Uint16 rsv					:11;
    }bit;
}UN_SET_REG;

/**************SCI Setting Data**************************/
typedef	struct{
	UN_SET_REG unSetReg;
	Uint16 	uwActivePercent;
	Uint16	uwReactiveModel;
	int16 	wReacitvePercent;
	int16 	wReacitvePF;
	/* Boost Curr Ctrl Loop Para*/
	Uint16  uwIdRef;
	int16  	wIqRef;
    int16   wIqPF;
	int16	wBTCurrRef[PV_MAX_NUM];
	int16	wMpptVoltRef[PV_MAX_NUM];
	
	Uint16	uwBusRefMin;
	Uint16	uwBusRef;

	Uint16	uwBtOpenLoopDuty;
	Uint16	uwDutyOffset;
	Uint16	uwAngleOffset;
	Uint16	uwBTSetDuty;
	int16	uwReactiveOffset;

}ST_SET_DATA;

typedef	struct{
	Uint16	uwWaitStatus;
	Uint16	uwWaitError;
	Uint16	uwInvStatus;
	Uint16	uwInvError;
	Uint16	uwFaultStatus;
	Uint16	uwDebug2;
	Uint16	uwDebug3;
	Uint16	uwDebug4;
	Uint16	uwDebug44;
	Uint16	uwDebug45;
	Uint16	uwDebug46;
	Uint16	uwInvOverCurr;
	Uint16	uwBoostOverCurr;
	Uint16	uwPowerFail;
	Uint16	uwBusOverVolt;
	Uint16	uwPllFailCnt;
	Uint16	uwPllSuccessedCnt;
	Uint16	uwWaitStartMsgStatus;
	Uint16	uwWaitStartError;
	Uint16	uwRelayCheckStep;
	Uint16	uwRelayCheckStatus;
	int16	wVolt_dErr;
	int16	wVolt_qErr;
	int16	wVolt_dFstDiff;
	int16	wVolt_qFstDiff;
	int16	uwBTRefPowerLimit[PV_MAX_NUM];
	int16	uwBTRefCurrLimit[PV_MAX_NUM];
	int16	wACVoltPeakValue;
	Uint16	uwIsoCheckCnt;
    int16   wDebug1;
    int16   wDebug2;
    int16   wDebug3;
    int16   wDebug8;
	
    int16   wDebug4[10];
    int16   wDebug5[10];
    int16   wDebug6[20];
	int16	wQvPFValue;
    int32	dMeterPower;
    int16   wInvVoltLack;
    int16   wInvVoltLackCnt;
    int16   wInvVoltLackReCnt;
    int16   wQVPFFlag;
}ST_READ_DATA;

/**************SCI Tx Status**************************/
typedef	struct{
	ST_SET_DATA		SetData;
	ST_READ_DATA	ReadData;
}ST_DEBUG;

typedef union
{
	Uint16 all;
	struct
	{
		Uint16  Bit0    	:1;
		Uint16  Bit1     	:1;
		Uint16  Bit2        :1;
		Uint16  Bit3        :1;
		Uint16  Bit4        :1;
		Uint16  Bit5        :1;
		Uint16  Bit6        :1;

		Uint16  reserved     :9;
	}bit;
}UN_FAULT_RECORD_REASON;

typedef struct
{
	UN_FAULT_RECORD_REASON unFaultRecordReason;
	UN_FAULT_RECORD_REASON unFaultRecordReasonBak;
	Uint16 uwReadWaveWaitCnt;
	Uint16 uwWaveRecordStopFlag;
	Uint16 uwRecordSaveDelayCnt;
	Uint16 uwRecordTrigger;
	Uint16 uwFaultRecordWaveReCnt;
	Uint16 uwReadWaveTrigger;
	Uint16 uwWaveRecordCnt;
	Uint16 uwWavePoint;
	Uint16 uwReadWavePoint;
	Uint16 uwWavePointNumMax;
	Uint16 uwWaveSamRatio;
	Uint16 uwRecoverFaultWaveIDCnt;
	Uint16 uwWaveID[4];
	Uint16 uwWaveDataBuffer[WAVE_CHANNELNUM_MAX][WAVE_POINTNUM_MAX];

    Uint16 uwSendStart;
    Uint16 uwSendTimeCnt;
    Uint16 uwWaveFrame;
    Uint16 uwSendPointIndex;
    Uint16 uwWaveChannelIndex;
    Uint16 uwDataLen;

    Uint16 uwFaultCodeBkp;

    Uint32 *pChannelAddress1;
    Uint32 *pChannelAddress2;
    Uint32 *pChannelAddress3;
    Uint32 *pChannelAddress4;

    Uint32 *pChannelAddress5;
    Uint32 *pChannelAddress6;
    Uint32 *pChannelAddress7;
    Uint32 *pChannelAddress8;
}ST_WAVERECORD;


extern ST_WAVERECORD stWaveRecord;
extern ST_DEBUG stDebug;


extern void InitDebugData(void);
extern void ComDebugTask(void);
extern void RecordingWave(void);
extern int16 ChangeWaveDataRate(Uint16 uwChannelNum, int16 wVaule);
extern Uint16 GetChannelNum(Uint16 uwWaveBuffIndex);

extern void sBufferShow(INT16U *pBuffAddr,INT16U wLength);
extern Uint16 ReadWaveData(Uint16 uwReadIndex_s,Uint16 uwWavePointCnt_s);
extern const Uint32 *pArithValArray[CHANNELNUMMAX];

#endif /* SYSTEM_PERIPH_INIT_H */
