/*
 * task_afci_debug.h
 *
 *  Created on: 2022年8月8日
 *      Author: Administrator
 */

#ifndef TASK_AFCI_DEBUG_H
#define TASK_AFCI_DEBUG_H

#define AFCI_ADDRESS		1

#define AFCI_RX_SIZE	64		// MAX : COM + CODE + REG_H + REG_L + LEN_H + LEN_L +  DATA_LEN + D_H + D_L + CRC(2)
#define AFCI_TX_SIZE	64		// COM + CODE + REG_H + REG_L + DYTE + DATA(0-255) + CRC_H + CRC_L

/**************SCI Tx Status**************************/
typedef	struct{
	Uint16 uwTxBuf[AFCI_TX_SIZE];
	Uint16 uwTxBufBak[AFCI_TX_SIZE];
	Uint16 uwRxBuf[AFCI_RX_SIZE];
	Uint16 uwRxBufBak[AFCI_RX_SIZE];
	Uint16 uwCommAddr;
}ST_AFCI_UART;
extern ST_AFCI_UART	stAfciUart;

typedef struct
{
    Uint16 uwCurrCH1;
    Uint16 uwCurrCH2;
    Uint16 uwCurrCH3;
    Uint16 uwCurrCH4;
	Uint16 uwArcFaultBit;
	Uint16 uwArcCH1;
	Uint16 uwArcCH2;
	Uint16 uwArcCH3;
	Uint16 uwArcCH4;
	Uint16 uwSelfCheckResult;
	Uint16 uwSelfCheckCmd;
	Uint16 uwArcFaultLimit;
	Uint16 uwArcRecordCH1;
	Uint16 uwArcRecordCH2;
	Uint16 uwArcRecordCH3;
	Uint16 uwArcRecordCH4;
	Uint16 uwArcFaultTimers;
	Uint16 uwReadFlag;

    Uint16 uwRxLenTmp;
    Uint16 suwAfciSetCmdCnt;

    Uint16 uwAFCI_FR_INVT_Flag;          //自研标志位  ABCD为自研

    Uint16 uwAFCI_StartFreq;             //AFCI频段开始值
    Uint16 uwAFCI_EndFreq;               //AFCI频段结束值
    Uint16 uwAFCI_MagRatio;              //AFCI当前频谱和滤波频谱的比较差异倍数
    Uint16 uwAFCI_FreqRatio;             //AFCI电弧计数数量阈值
    Uint16 uwAFCI_BinRatio;              //AFCI高频谱计数数量阈值
    Uint16 uwAFCI_MagThld_PowerOn_H;     //AFCI频谱高度阈值

    Uint16 uwAFCI_FaultCntFlag;         //AFCI故障清除锁死标志位(发送给ARM)
    Uint16 uwAFCI_Online_FaultCnt;    //远程三次计数
    Uint16 uwAFCIFaultLast;           //AFCI历史故障
    Uint16 uwAFCI_Auto_FaultCnt;      //AFCI自动清除次数
    Uint16 uwAFCI_Auto_FaultTime;     //AFCI自动清除时间
    Uint16 uwAFCI_SelfCheckTime_H;     //AFCI自检累计时间
    Uint16 uwAFCI_SelfCheckTime;     //AFCI自检累计时间
    Uint16 uwAFCI_SelfCheckFlag;     //AFCI自检写入标志位
    Uint16 uwAFCI_SelfCheckComp_Flag;     //AFCI自检传给ARM标志位
    Uint16 uwAFCI_FaultCnt_Clear_H;      //AFCI24小时故障累计次数清除
    Uint16 uwAFCI_FaultCnt_Clear_T;      //AFCI24小时故障累计次数清除

    Uint16 uwAFCI_ReadTime;     //AFCI自检累计时间
    Uint16 CommFault_ReSetCmdSend_Cnt;       //复位发送指令计数
    Uint16 CommFault_ReSetCmdSendFlag;       //复位发送指令标志位
    Uint16 CommFault_ReSetCmdComp_Flag;     //复位发送传给ARM标志位

    Uint16 uwAFCI_FaultMisFlag;       //故障误报复位指令
    Uint16 uwAFCI_ResetFlag;           //手动复位标志位

    Uint16 TurnOn;                  //上电初始化
    Uint16 uwAFCI_SelfOnTime;     //AFCI自检累计时间
}ST_AFCI_PARA;   //The structure for AFCI System Parameter
extern ST_AFCI_PARA	stAfciPara1;
extern ST_AFCI_PARA	stAfciPara2;
extern ST_AFCI_PARA	stAfciPara3;
extern ST_AFCI_PARA	stAfciPara4;
extern ST_AFCI_PARA stAfciPara;

extern void ComAfciParaInit(void);
extern void ComAfciTask(void);
extern void ComAfci_RX(void);
extern void ComAfci_SelfCHK(void);
extern void ComAfci_CHK(void);
extern void ComAfci_TX(void);
extern void ComAfci_FaultDiagn(void);


#endif /* SYSTEM_PERIPH_INIT_H */
