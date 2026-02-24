
/*
 * task_com_debug.c
 *
 *  Created on: 2020年12月26日
 *      Author: Administrator
 */
#include "dsp_user_include.h"

ST_AFCI_UART	stAfciUart;
ST_AFCI_PARA	stAfciPara1;
ST_AFCI_PARA	stAfciPara2;
ST_AFCI_PARA	stAfciPara3;
ST_AFCI_PARA	stAfciPara4;
ST_AFCI_PARA    stAfciPara;

/****************************************************************************
*
*	FunctionName:	ComAfciTask
*	Input:	none
*	Output:	none
*	Description:	DSP and AFCI Internal communicate
*	Note:	none.
***************************************************************/
void ComAfciParaInit(void)
{
    memset((void*)&stAfciUart,0,sizeof(stAfciUart));
    memset((void*)&stAfciPara1,0,sizeof(stAfciPara1));
   memset((void*)&stAfciPara2,0,sizeof(stAfciPara2));
   memset((void*)&stAfciPara3,0,sizeof(stAfciPara3));
   memset((void*)&stAfciPara4,0,sizeof(stAfciPara4));
   memset((void*)&stAfciPara,0,sizeof(stAfciPara));

    stAfciPara1.uwArcFaultLimit = 120;
    stAfciPara2.uwArcFaultLimit = 120;
    stAfciPara3.uwArcFaultLimit = 120;
    stAfciPara4.uwArcFaultLimit = 120;

    stAfciPara.uwArcFaultLimit = 120;
    stAfciPara.uwSelfCheckCmd = 1;  //上电默认自检
    stAfciPara.uwAFCI_StartFreq = 10;
    stAfciPara.uwAFCI_EndFreq = 40;
    stAfciPara.uwAFCI_MagRatio = 23;
    stAfciPara.uwAFCI_FreqRatio = 40;
    stAfciPara.uwAFCI_BinRatio = 8;
    stAfciPara.uwAFCI_MagThld_PowerOn_H = 8;

    stF107Data.uwSelfCheckEnable_New = 0;
    stAfciPara.uwAFCI_SelfCheckFlag = 0;        //自检标志位
    stAfciPara.uwAFCI_SelfCheckTime_H = 0;
    stAfciPara.uwAFCI_SelfCheckTime = 0;
    stF107Data.uwAFCICMD = CLR_MODEL_Auto;
    stAfciPara.CommFault_ReSetCmdSendFlag = 0;
    stAfciPara.uwAFCI_SelfCheckComp_Flag = 0;     //自检开始发给ARM

    stAfciPara.TurnOn = 1;
}


void ComAfciTask(void)
{
	static TASK_EVENT event = 0;
	Uint16 uwCheckSumTmp = 0;
	static Uint16 suwAfciComFaultSetCnt=0, suwAfciComFaultClrCnt=0;
	 Uint16 i = 0;
	

	while(1)
	{
		event = OSEventPend();

		//receive AFCI data
		if(event&(1<<COM_AFCI_RX_ENDED))
		{
			if(0x03 == stAfciUart.uwRxBuf[1])
			{
                stAfciPara.uwRxLenTmp = stAfciUart.uwRxBuf[2]+5;
            }
            else
            {
                stAfciPara.uwRxLenTmp = 8;
            }
            if((stAfciPara.uwRxLenTmp<=AFCI_RX_SIZE)&&(stAfciPara.uwRxLenTmp>2))
            {
                uwCheckSumTmp = uwCalcCRC16(stAfciUart.uwRxBuf, (stAfciPara.uwRxLenTmp-2));

                if(((uwCheckSumTmp&0xFF)==stAfciUart.uwRxBuf[stAfciPara.uwRxLenTmp-2])
                &&((uwCheckSumTmp>>8)==stAfciUart.uwRxBuf[stAfciPara.uwRxLenTmp-1]))
                {
                    ComAfci_RX();
                    ComAfci_SelfCHK();
                    ComAfci_CHK();
                    suwAfciComFaultSetCnt = 0;
                    if((++suwAfciComFaultClrCnt) > CLR_F042_COM_FAULT_CNT)
                    {
                        suwAfciComFaultClrCnt = CLR_F042_COM_FAULT_CNT;
                        stSysFaultReg.unCommunicateFault.bit.AFCISci = 0;
                    }
				}
			}
		}
		else if(event&(1<<COM_AFCI_TIMER))
		{

            if((stAfciPara.suwAfciSetCmdCnt == 1)&&(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD))
            {
                suwAfciComFaultSetCnt = 0;
            }
            else
            {
                suwAfciComFaultSetCnt++;
            }
			if(suwAfciComFaultSetCnt > SET_F042_COM_FAULT_CNT)		//10S
			{
				suwAfciComFaultSetCnt = 0;
				suwAfciComFaultClrCnt = 0;

				if(ENABLE == stDspReceData.unFuncEnable.bit.AFCIEnable)		// AFCI Enable
				{
					stSysFaultReg.unFaultBit.bit.CommunicateFault = 1;
					stSysFaultReg.unCommunicateFault.bit.AFCISci = 1;
				}
				else // AFCI Disable
				{
					/***clear AFCI communicate Fault*/
					stSysFaultReg.unCommunicateFault.bit.AFCISci = 0;
					/***clear AFCI Fault*/
					stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
					stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
					stSysFaultReg.unAFCIFault.all = 0;
				}
			}
		}

		// DSP send message to AFCI
		if(event&(1<<COM_AFCI_TIMER))
		{
		    ComAfci_TX();
		}
	}
}


void ComAfci_RX(void)
{

    if((0x03 == stAfciUart.uwRxBuf[1])&&(stAfciPara.uwRxLenTmp>=35))
    {
        stAfciPara.uwAFCI_FR_INVT_Flag = stAfciUart.uwRxBuf[27] * 256 + stAfciUart.uwRxBuf[28];  //自研标志位
        if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
        {
            stAfciPara.uwCurrCH1 = stAfciUart.uwRxBuf[4];
            stAfciPara.uwCurrCH2 = stAfciUart.uwRxBuf[6];
            stAfciPara.uwArcFaultBit = stAfciUart.uwRxBuf[12];
            stAfciPara.uwArcCH1 = stAfciUart.uwRxBuf[14];
            stAfciPara.uwArcCH2 = stAfciUart.uwRxBuf[16];
			stAfciPara.uwArcCH3 =   stAfciUart.uwRxBuf[18];
        	stAfciPara.uwArcCH4 =   stAfciUart.uwRxBuf[20];
            stAfciPara.uwSelfCheckResult = stAfciUart.uwRxBuf[22];
            stAfciPara.uwSelfCheckCmd = stAfciUart.uwRxBuf[24];
            stAfciPara.uwArcFaultLimit = stAfciUart.uwRxBuf[26];
            stAfciPara.uwArcRecordCH1 = stAfciUart.uwRxBuf[28];
            stAfciPara.uwArcRecordCH2 = stAfciUart.uwRxBuf[30];
			stAfciPara.uwArcRecordCH3 = stAfciUart.uwRxBuf[32];
            stAfciPara.uwArcRecordCH4 = stAfciUart.uwRxBuf[34];
            stAfciPara.uwAFCI_StartFreq = stAfciUart.uwRxBuf[36];
            stAfciPara.uwAFCI_EndFreq = stAfciUart.uwRxBuf[38];
            stAfciPara.uwAFCI_MagRatio = stAfciUart.uwRxBuf[40];
            stAfciPara.uwAFCI_FreqRatio = stAfciUart.uwRxBuf[42];
            stAfciPara.uwAFCI_BinRatio = stAfciUart.uwRxBuf[44];
            stAfciPara.uwAFCI_MagThld_PowerOn_H = stAfciUart.uwRxBuf[46];

        }
        else
        {
            if(1 == stAfciUart.uwRxBuf[0])
            {
                stAfciPara1.uwCurrCH1 = stAfciUart.uwRxBuf[4];
                stAfciPara1.uwCurrCH2 = stAfciUart.uwRxBuf[6];
                stAfciPara1.uwCurrCH3 = stAfciUart.uwRxBuf[8];
                stAfciPara1.uwCurrCH4 = stAfciUart.uwRxBuf[10];
                stAfciPara1.uwArcFaultBit = stAfciUart.uwRxBuf[12];
                stAfciPara1.uwArcCH1 = stAfciUart.uwRxBuf[14];
                stAfciPara1.uwArcCH2 = stAfciUart.uwRxBuf[16];
                stAfciPara1.uwArcCH3 = stAfciUart.uwRxBuf[18];
                stAfciPara1.uwArcCH4 = stAfciUart.uwRxBuf[20];
                stAfciPara1.uwSelfCheckResult = stAfciUart.uwRxBuf[22];
                stAfciPara1.uwArcFaultLimit = stAfciUart.uwRxBuf[26];
                stAfciPara1.uwArcRecordCH1 = stAfciUart.uwRxBuf[28];
                stAfciPara1.uwArcRecordCH2 = stAfciUart.uwRxBuf[30];
                stAfciPara1.uwArcRecordCH3 = stAfciUart.uwRxBuf[32];
                stAfciPara1.uwArcRecordCH4 = stAfciUart.uwRxBuf[34];
                stAfciPara1.uwReadFlag = 1;
            }
            else if(2 == stAfciUart.uwRxBuf[0])
            {
                stAfciPara2.uwCurrCH1 = stAfciUart.uwRxBuf[4];
                stAfciPara2.uwCurrCH2 = stAfciUart.uwRxBuf[6];
                stAfciPara2.uwCurrCH3 = stAfciUart.uwRxBuf[8];
                stAfciPara2.uwCurrCH4 = stAfciUart.uwRxBuf[10];
                stAfciPara2.uwArcFaultBit = stAfciUart.uwRxBuf[12];
                stAfciPara2.uwArcCH1 = stAfciUart.uwRxBuf[14];
                stAfciPara2.uwArcCH2 = stAfciUart.uwRxBuf[16];
                stAfciPara2.uwArcCH3 = stAfciUart.uwRxBuf[18];
                stAfciPara2.uwArcCH4 = stAfciUart.uwRxBuf[20];
                stAfciPara2.uwSelfCheckResult = stAfciUart.uwRxBuf[22];
                stAfciPara2.uwArcFaultLimit = stAfciUart.uwRxBuf[26];
                stAfciPara2.uwArcRecordCH1 = stAfciUart.uwRxBuf[28];
                stAfciPara2.uwArcRecordCH2 = stAfciUart.uwRxBuf[30];
                stAfciPara2.uwArcRecordCH3 = stAfciUart.uwRxBuf[32];
                stAfciPara2.uwArcRecordCH4 = stAfciUart.uwRxBuf[34];
                stAfciPara2.uwReadFlag = 1;
            }
            else if(3 == stAfciUart.uwRxBuf[0])
            {
                stAfciPara3.uwCurrCH1 = stAfciUart.uwRxBuf[4];
                stAfciPara3.uwCurrCH2 = stAfciUart.uwRxBuf[6];
                stAfciPara3.uwCurrCH3 = stAfciUart.uwRxBuf[8];
                stAfciPara3.uwCurrCH4 = stAfciUart.uwRxBuf[10];
                stAfciPara3.uwArcFaultBit = stAfciUart.uwRxBuf[12];
                stAfciPara3.uwArcCH1 = stAfciUart.uwRxBuf[14];
                stAfciPara3.uwArcCH2 = stAfciUart.uwRxBuf[16];
                stAfciPara3.uwArcCH3 = stAfciUart.uwRxBuf[18];
                stAfciPara3.uwArcCH4 = stAfciUart.uwRxBuf[20];
                stAfciPara3.uwSelfCheckResult = stAfciUart.uwRxBuf[22];
                stAfciPara3.uwArcFaultLimit = stAfciUart.uwRxBuf[26];
                stAfciPara3.uwArcRecordCH1 = stAfciUart.uwRxBuf[28];
                stAfciPara3.uwArcRecordCH2 = stAfciUart.uwRxBuf[30];
                stAfciPara3.uwArcRecordCH3 = stAfciUart.uwRxBuf[32];
                stAfciPara3.uwArcRecordCH4 = stAfciUart.uwRxBuf[34];
                stAfciPara3.uwReadFlag = 1;
            }
            else if(4 == stAfciUart.uwRxBuf[0])
            {
                stAfciPara4.uwCurrCH1 = stAfciUart.uwRxBuf[4];
                stAfciPara4.uwCurrCH2 = stAfciUart.uwRxBuf[6];
                stAfciPara4.uwCurrCH3 = stAfciUart.uwRxBuf[8];
                stAfciPara4.uwCurrCH4 = stAfciUart.uwRxBuf[10];
                stAfciPara4.uwArcFaultBit = stAfciUart.uwRxBuf[12];
                stAfciPara4.uwArcCH1 = stAfciUart.uwRxBuf[14];
                stAfciPara4.uwArcCH2 = stAfciUart.uwRxBuf[16];
                stAfciPara4.uwArcCH3 = stAfciUart.uwRxBuf[18];
                stAfciPara4.uwArcCH4 = stAfciUart.uwRxBuf[20];
                stAfciPara4.uwSelfCheckResult = stAfciUart.uwRxBuf[22];
                stAfciPara4.uwArcFaultLimit = stAfciUart.uwRxBuf[26];
                stAfciPara4.uwArcRecordCH1 = stAfciUart.uwRxBuf[28];
                stAfciPara4.uwArcRecordCH2 = stAfciUart.uwRxBuf[30];
                stAfciPara4.uwArcRecordCH3 = stAfciUart.uwRxBuf[32];
                stAfciPara4.uwArcRecordCH4 = stAfciUart.uwRxBuf[34];
                stAfciPara4.uwReadFlag = 1;
            }
        }
    }
}

void ComAfci_TX(void)
{
    Uint16 uwCheckSumTmp = 0;
    static Uint16 uwAFCI_ManualResetCnt=0;
    Uint16 uCommAddrTemp = 0;

    if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
    {
        stAfciUart.uwTxBuf[0] = AFCI_ADDRESS;
        if(stAfciPara.TurnOn == 1)
        {
            stAfciPara.uwAFCI_SelfOnTime++;
            if(stAfciPara.uwAFCI_SelfOnTime >= 30)
            {
                stAfciPara.TurnOn = 0;
                stAfciPara.uwAFCI_SelfOnTime = 31;
            }
        }
        if(stF107Data.uwAFCI_ManualResetFlag == 1)         //手动复位
        {
            uwAFCI_ManualResetCnt++;
            if(uwAFCI_ManualResetCnt >= 50)
            {
                stAfciPara.CommFault_ReSetCmdComp_Flag = 1;    //手动复位发送给ARM
                stAfciPara.uwAFCI_ResetFlag = 1;
                uwAFCI_ManualResetCnt = 0;
            }
        }
        else
        {
            stAfciPara.CommFault_ReSetCmdComp_Flag = 0;
            stAfciPara.uwAFCI_ResetFlag = 0;
            uwAFCI_ManualResetCnt = 0;
        }

        if(((stAfciPara.CommFault_ReSetCmdSendFlag != 0)||(stSysFaultReg.unFaultBit.bit.AFCIFault == 1)
        ||(stAfciPara.uwAFCI_FaultMisFlag == 1)||(stAfciPara.uwAFCI_ResetFlag == 1))
        &&(stAfciPara.suwAfciSetCmdCnt==0))
        {
            {
                stAfciUart.uwTxBuf[1] = 0x06;                   // 功能码
                stAfciUart.uwTxBuf[2] = 0x10;                   // 寄存器地址高位
                stAfciUart.uwTxBuf[3] = 0x36;                   // 寄存器地址低位
                stAfciUart.uwTxBuf[4] = 0x12;                   // 数据高位
                stAfciUart.uwTxBuf[5] = 0x34;                   // 数据低位
                stAfciPara.CommFault_ReSetCmdSendFlag = 0;
                stAfciPara.suwAfciSetCmdCnt = 1;
                stAfciPara.uwAFCI_FaultMisFlag = 0;
            }
        }
        else if((stAfciPara.uwArcFaultLimit != stF107Data.uwArcFaultLimit)
         &&(stF107Data.uwArcFaultLimit>=0)&&(stF107Data.uwArcFaultLimit<=250)&&(stAfciPara.suwAfciSetCmdCnt==0)
         &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0B;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwArcFaultLimit&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 1;
        }
        else if((stAfciPara.uwAFCI_StartFreq != stF107Data.uwAFCI_StartFreq)
        &&(stF107Data.uwAFCI_StartFreq>=0)&&(stF107Data.uwAFCI_StartFreq<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x10;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_StartFreq&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 2;
        }
        else if((stAfciPara.uwAFCI_EndFreq != stF107Data.uwAFCI_EndFreq)
        &&(stF107Data.uwAFCI_EndFreq>0)&&(stF107Data.uwAFCI_EndFreq<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x11;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_EndFreq&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 3;
        }
        else if((stAfciPara.uwAFCI_MagRatio != stF107Data.uwAFCI_MagRatio)
        &&(stF107Data.uwAFCI_MagRatio>0)&&(stF107Data.uwAFCI_MagRatio<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x12;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_MagRatio&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 4;
        }
        else if((stAfciPara.uwAFCI_FreqRatio != stF107Data.uwAFCI_FreqRatio)
        &&(stF107Data.uwAFCI_FreqRatio>0)&&(stF107Data.uwAFCI_FreqRatio<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x13;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_FreqRatio&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 5;
        }
        else if((stAfciPara.uwAFCI_BinRatio != stF107Data.uwAFCI_BinRatio)
        &&(stF107Data.uwAFCI_BinRatio>0)&&(stF107Data.uwAFCI_BinRatio<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x14;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_BinRatio&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 6;
        }
        else if((stAfciPara.uwAFCI_MagThld_PowerOn_H != stF107Data.uwAFCI_MagThld_PowerOn_H)
        &&(stF107Data.uwAFCI_MagThld_PowerOn_H>0)&&(stF107Data.uwAFCI_MagThld_PowerOn_H<=100)&&(stAfciPara.suwAfciSetCmdCnt==0)
        &&(stAfciPara.uwSelfCheckCmd == 0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x15;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwAFCI_MagThld_PowerOn_H&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
            stAfciPara.CommFault_ReSetCmdSendFlag = 7;
        }
        else if((stAfciPara.uwAFCI_SelfCheckFlag == 1)&&(stAfciPara.uwSelfCheckCmd == 0)
        &&(stAfciPara.suwAfciSetCmdCnt==0)&&(stAfciPara.TurnOn == 0))//&&(eInverterStatus != cInverterStatus))
        {
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0A;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (0x01&0xff);      // data low
            stAfciPara.suwAfciSetCmdCnt = 1;
        }
        else
        {
            stAfciUart.uwTxBuf[1] = 0x03;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x00;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = 0x16;       // data low

            if(stAfciPara.suwAfciSetCmdCnt==1)
            {
                stAfciPara.uwAFCI_ReadTime++;
                if(stAfciPara.uwAFCI_ReadTime > 30)     //3S
                {
                    stAfciPara.suwAfciSetCmdCnt = 0;
                    stAfciPara.uwAFCI_ReadTime = 0;
                }
            }
            else
           {
                stAfciPara.suwAfciSetCmdCnt = 0;
           }
        }
    }
    else
    {
        if((MACHINE_ID_50KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_60_70KW ==  stSysCfg.uwMachineType))
        {
            uCommAddrTemp =4;
        }
        else if((MACHINE_ID_25_33KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_36_40KW ==  stSysCfg.uwMachineType)|| (MACHINE_ID_25KW ==  stSysCfg.uwMachineType)
        || (MACHINE_ID_25KW_MAX ==  stSysCfg.uwMachineType)|| (MACHINE_ID_36_40KW_MAX ==  stSysCfg.uwMachineType)|| (MACHINE_ID_30_33KW_MAX ==  stSysCfg.uwMachineType)
        ||(MACHINE_ID_Plus_30_36KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_Plus_50KW ==  stSysCfg.uwMachineType)||(MACHINE_ID_PARALLEL_TUBE_25KW == stSysCfg.uwMachineType)
        ||(MACHINE_ID_PRO_60KW ==  stSysCfg.uwMachineType) || (MACHINE_ID_PRO_50KW == stSysCfg.uwMachineType) || (stSysCfg.uwMachineType == MACHINE_ID_PRO_50KW_2))
        {
            uCommAddrTemp =2;
        }
        else
        {
            uCommAddrTemp =1;
        }
        stAfciUart.uwCommAddr++;
        if(stAfciUart.uwCommAddr > uCommAddrTemp)
        {
            stAfciUart.uwCommAddr = 1;
        }
        stAfciUart.uwTxBuf[0] = stAfciUart.uwCommAddr;

        if((stAfciPara1.uwArcFaultLimit != stF107Data.uwArcFaultLimit)&&(stAfciUart.uwCommAddr == 1)
         &&(stF107Data.uwArcFaultLimit>0)&&(1==stAfciPara1.uwReadFlag))
        {
            stAfciUart.uwTxBuf[0] = 0x01;
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0B;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwArcFaultLimit&0xff);      // data low
            stAfciPara1.uwReadFlag = 0;
        }
        else if((stAfciPara2.uwArcFaultLimit != stF107Data.uwArcFaultLimit)&&(stAfciUart.uwCommAddr == 2)
         &&(stF107Data.uwArcFaultLimit>0)&&(1==stAfciPara2.uwReadFlag))
        {
            stAfciUart.uwTxBuf[0] = 0x02;
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0B;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwArcFaultLimit&0xff);      // data low
            stAfciPara2.uwReadFlag = 0;
        }
        else if((stAfciPara3.uwArcFaultLimit != stF107Data.uwArcFaultLimit)&&(stAfciUart.uwCommAddr == 3)
         &&(stF107Data.uwArcFaultLimit>0)&&(1==stAfciPara3.uwReadFlag))
        {
            stAfciUart.uwTxBuf[0] = 0x03;
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0B;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwArcFaultLimit&0xff);      // data low
            stAfciPara3.uwReadFlag = 0;
        }
        else if((stAfciPara4.uwArcFaultLimit != stF107Data.uwArcFaultLimit)&&(stAfciUart.uwCommAddr == 4)
         &&(stF107Data.uwArcFaultLimit>0)&&(1==stAfciPara4.uwReadFlag))
        {
            stAfciUart.uwTxBuf[0] = 0x04;
            stAfciUart.uwTxBuf[1] = 0x06;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x0B;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = (stF107Data.uwArcFaultLimit&0xff);      // data low
            stAfciPara4.uwReadFlag = 0;
        }
        else
        {
            //stAfciPara1.uwReadFlag = 0;
            stAfciUart.uwTxBuf[1] = 0x03;       // function code
            stAfciUart.uwTxBuf[2] = 0x00;       // address High
            stAfciUart.uwTxBuf[3] = 0x00;       // address Low     6bytes
            stAfciUart.uwTxBuf[4] = 0x00;       // data high
            stAfciUart.uwTxBuf[5] = 0x10;       // data low
        }
    }

    uwCheckSumTmp = uwCalcCRC16(stAfciUart.uwTxBuf,6);

    stAfciUart.uwTxBuf[6] = (uwCheckSumTmp&0x00FF);
    stAfciUart.uwTxBuf[7] = ((uwCheckSumTmp&0xFF00)>>8);
    uwSciWrite(DSP_AFCI_SCIC,stAfciUart.uwTxBufBak,stAfciUart.uwTxBuf,8);
}

void ComAfci_SelfCHK(void)
{
    static Uint16 uwSelfCheckTemp = 0,uwSelfCheckCnt = 0;
    /*********自检方式选择*******/
    if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
    {
         if(stF107Data.uwSelfCheckEnable_New == 1)         //手动自检
         {
             if(stAfciPara.uwSelfCheckCmd == 0)
             {
                 uwSelfCheckTemp++;
                 if(uwSelfCheckTemp >= 50)
                 {
                     stAfciPara.uwAFCI_SelfCheckFlag = 1;        //自检标志位
                     stAfciPara.uwAFCI_SelfCheckComp_Flag = 1;     //自检开始发给ARM
                     uwSelfCheckTemp = 0;
                 }
             }
             else
             {
                 stAfciPara.uwAFCI_SelfCheckFlag = 0;        //自检标志位
                 stAfciPara.uwAFCI_SelfCheckComp_Flag = 0;     //自检开始发给ARM
                 uwSelfCheckTemp = 0;
             }
         }
         else if(stSysFaultReg.unFaultBit.bit.AFCISelfCheck == 1)     //自检失败
         {
             if(stAfciPara.uwSelfCheckCmd == 0)
             {
                 uwSelfCheckCnt++;
                 if(uwSelfCheckCnt >= 600)
                 {
                     stAfciPara.uwAFCI_SelfCheckFlag = 1;        //自检标志位
                     uwSelfCheckCnt = 0;
                 }
             }
             else
             {
                 stAfciPara.uwAFCI_SelfCheckFlag = 0;
             }
         }
         else
         {
             if(stAfciPara.uwSelfCheckCmd == 0)
             {
                 stAfciPara.uwAFCI_SelfCheckTime_H++;
                 if(stAfciPara.uwAFCI_SelfCheckTime_H >= 36000)
                 {
                     stAfciPara.uwAFCI_SelfCheckTime_H = 0;
                     stAfciPara.uwAFCI_SelfCheckTime++;
                 }
                 if(stAfciPara.uwAFCI_SelfCheckTime >= 25)
                 {
                     stAfciPara.uwAFCI_SelfCheckTime = 0;
                     if(stF107Data.usAFCI_AuthenFlag == 1)          //认证使能
                     {
                         stAfciPara.uwAFCI_SelfCheckFlag = 1;        //自检标志位
                     }
                 }
                 else
                 {
                     stAfciPara.uwAFCI_SelfCheckFlag = 0;
                 }
             }
             else
             {
                 stAfciPara.uwAFCI_SelfCheckTime_H = 0;
                 stAfciPara.uwAFCI_SelfCheckTime = 0;
                 stAfciPara.uwAFCI_SelfCheckFlag = 0;
                 stAfciPara.uwAFCI_SelfCheckComp_Flag = 0;
             }
             uwSelfCheckTemp = 0;
             uwSelfCheckCnt = 0;
         }

         if(stAfciPara.uwAFCI_SelfCheckFlag == 1)
         {
             OSEventSend(STATUS_PRIO, STATUS_TO_FAULT);
         }
    }
}

void ComAfci_CHK(void)
{
    static Uint16 suwAfciFaultClrCnt=0;

    if(ENABLE == stDspReceData.unFuncEnable.bit.AFCIEnable)
    {
        if(1==stSysFaultReg.unFaultBit.bit.AFCIFault)
        {
            if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
            {
                switch(stF107Data.uwAFCICMD)
                {
                    case CLR_MODEL_NOT:              //不清除
                    break;

                    case CLR_MODEL_OnlineOnly:        //远程手动清除
                    if(stAfciPara.uwAFCI_FaultCntFlag == 0)       //未锁死
                    {
                        stAfciPara.uwAFCI_Online_FaultCnt++;
                        if(stAfciPara.uwAFCI_Online_FaultCnt < 3)
                        {
                            stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                        }
                    }
                    if(stF107Data.usAFCI_AuthenFlag == 1)          //认证使能
                    {
                        if(stAfciPara.uwAFCI_Online_FaultCnt >= 3)
                        {
                            stAfciPara.uwAFCI_FaultCntFlag = 1;    //发送给ARM
                            stAfciPara.uwAFCI_Online_FaultCnt = 0;
                        }
                    }
                    else
                    {
                        stAfciPara.uwAFCI_FaultCntFlag = 0;
                        stAfciPara.uwAFCI_Online_FaultCnt = 0;
                    }
                    break;

                    case CLR_MODEL_SelfFault_Online:
                    break;

                    case CLR_MODEL_Offline:
                        stAfciPara.uwAFCI_FaultCntFlag = 0;
                        stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                    break;

                    case CLR_MODEL_Auto:
                    stAfciPara.uwAFCI_Auto_FaultTime++;
                    if(stAfciPara.uwAFCI_Auto_FaultTime >= stF107Data.uwAFCI_Set_FaultCntTime_S)
                    {
                        if(stAfciPara.uwAFCI_FaultCntFlag == 0)       //未锁死
                        {
                            stAfciPara.uwAFCI_Auto_FaultCnt++;
                            if(stAfciPara.uwAFCI_Auto_FaultCnt < 5)
                            {
                                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                            }
                        }
                        stAfciPara.uwAFCI_Auto_FaultTime = 0;
                    }
                    if(stF107Data.usAFCI_AuthenFlag == 1)          //认证使能
                    {
                        if(stAfciPara.uwAFCI_Auto_FaultCnt >= 5)
                        {
                            stAfciPara.uwAFCI_FaultCntFlag = 1;    //发送给ARM
                            stAfciPara.uwAFCI_Auto_FaultCnt = 0;
                        }
                    }
                    else
                    {
                        stAfciPara.uwAFCI_FaultCntFlag = 0;    //发送给ARM
                        stAfciPara.uwAFCI_Auto_FaultCnt = 0;
                    }
                    break;

                    default:
                        stF107Data.uwAFCICMD = CLR_MODEL_Auto;
                    break;
                }

                /******24h对故障累计次数进行一次清除********/
                if((stF107Data.usAFCI_AuthenFlag == 1)&&(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD))          //认证使能
                {
                    stAfciPara.uwAFCI_FaultCnt_Clear_H++;
                    if(stAfciPara.uwAFCI_FaultCnt_Clear_H >= 36000)
                    {
                        stAfciPara.uwAFCI_FaultCnt_Clear_H = 0;
                        stAfciPara.uwAFCI_FaultCnt_Clear_T++;
                    }
                    if(stAfciPara.uwAFCI_FaultCnt_Clear_T >= 25)
                    {
                        stAfciPara.uwAFCI_FaultCnt_Clear_T = 0;
                        stAfciPara.uwAFCI_Online_FaultCnt = 0;
                        stAfciPara.uwAFCI_Auto_FaultCnt = 0;
                    }
                }
            }
            else
            {
                if((0==stAfciPara1.uwArcFaultBit)&&(0==stAfciPara1.uwSelfCheckResult)
                    &&(0==stAfciPara2.uwArcFaultBit)&&(0==stAfciPara2.uwSelfCheckResult)
                    &&(0==stAfciPara3.uwArcFaultBit)&&(0==stAfciPara3.uwSelfCheckResult)
                    &&(0==stAfciPara4.uwArcFaultBit)&&(0==stAfciPara4.uwSelfCheckResult))
                {
                    suwAfciFaultClrCnt++;
                    if(suwAfciFaultClrCnt >= 3000)  // 3000*100ms = 5s
                    {
                        suwAfciFaultClrCnt = 0;
                        /***clear AFCI Fault*/
                        stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                        stSysFaultReg.unAFCIFault.all = 0;
                        stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
                        stSysFaultReg.unAFCISelfCheck.all = 0;

                    }
                }
            }
        }
        else
        {
            suwAfciFaultClrCnt = 0;
            ComAfci_FaultDiagn();
        }
    }
    else
    {
        stAfciPara.uwAFCI_Online_FaultCnt = 0;
        stAfciPara.uwAFCI_Auto_FaultTime = 0;
        stAfciPara.uwAFCI_Auto_FaultCnt = 0;
        stAfciPara.uwAFCI_FaultCntFlag = 0;
        stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
        stSysFaultReg.unAFCIFault.all = 0;
        stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
        stSysFaultReg.unAFCISelfCheck.all = 0;
    }
}

void ComAfci_FaultDiagn(void)
{
    static int16 PVACurrentOld = 0;
    static int16 PVBCurrentOld = 0;
    static int16 PVCCurrentOld = 0;
    static int16 PVDCurrentOld = 0;
    static int16 AFCIFaultErrFlag = 0;
    if(stAfciPara.uwAFCI_FR_INVT_Flag == 0xABCD)
    {
        if(stAfciPara.uwArcFaultBit != 0)
        {
            if(stIVCurve.unIVCurveBits.bit.StartEnable == 1)
            {
                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                stAfciPara.uwAFCI_FaultMisFlag = 1;
            }
            else if((stDCSample.wBTCurr[PVA]<64)
            &&(stDCSample.wBTCurr[PVB]<64)
            &&(stDCSample.wBTCurr[PVC]<64)
            &&(stDCSample.wBTCurr[PVD]<64))
            {
                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                stAfciPara.uwAFCI_FaultMisFlag = 1;
                AFCIFaultErrFlag = 0;
            }
            else if(eInverterStatus != cInverterStatus)
            {
                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                stAfciPara.uwAFCI_FaultMisFlag = 1;
                AFCIFaultErrFlag = 0;
            }
//            else if(0 != stLoadLimit.uwDeratingMode)
//            {
//                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
//                stAfciPara.uwAFCI_FaultMisFlag = 1;
//                AFCIFaultErrFlag = 0;
//            }
            else if((labs(PVACurrentOld - stDCSample.wBTCurr[PVA]) >= 160)//5A*32
                   ||(labs(PVBCurrentOld - stDCSample.wBTCurr[PVB]) >= 160)
                   ||(labs(PVCCurrentOld - stDCSample.wBTCurr[PVC]) >= 160)
                   ||(labs(PVDCurrentOld - stDCSample.wBTCurr[PVD]) >= 160))
            {
                stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                stAfciPara.uwAFCI_FaultMisFlag = 1;
                AFCIFaultErrFlag = 0;
            }
            else
            {
                if((stAfciPara.uwArcFaultBit& 0x01) && (stDCSample.wBTCurr[PVA] > 64))
                {
                    AFCIFaultErrFlag = 1;
                    stAfciPara.uwArcFaultTimers++;
                    stSysFaultReg.unFaultBit.bit.AFCIFault = 1;
                    stSysFaultReg.unAFCIFault.bit.OverLimitStr1 = 1;
                }
                if((stAfciPara.uwArcFaultBit& 0x02) && (stDCSample.wBTCurr[PVB] > 64))
                {
                    AFCIFaultErrFlag = 1;
                    stAfciPara.uwArcFaultTimers++;
                    stSysFaultReg.unFaultBit.bit.AFCIFault = 1;
                    stSysFaultReg.unAFCIFault.bit.OverLimitStr2 = 1;
                }
                if((stAfciPara.uwArcFaultBit& 0x04) && (stDCSample.wBTCurr[PVC] > 64))
                {
                    AFCIFaultErrFlag = 1;
                    stAfciPara.uwArcFaultTimers++;
                    stSysFaultReg.unFaultBit.bit.AFCIFault = 1;
                    stSysFaultReg.unAFCIFault.bit.OverLimitStr3 = 1;
                }
                if((stAfciPara.uwArcFaultBit& 0x08) && (stDCSample.wBTCurr[PVD] > 64))
                {
                    AFCIFaultErrFlag = 1;
                    stAfciPara.uwArcFaultTimers++;
                    stSysFaultReg.unFaultBit.bit.AFCIFault = 1;
                    stSysFaultReg.unAFCIFault.bit.OverLimitStr4 = 1;
                }
                if((0 == AFCIFaultErrFlag)&&(0 != stAfciPara.uwArcFaultBit))
                {
                    stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
                    stAfciPara.uwAFCI_FaultMisFlag = 1;
                }
            }
        }
        else
        {
            stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
            stSysFaultReg.unAFCIFault.all = 0;
        }
        PVACurrentOld = stDCSample.wBTCurr[PVA];
        PVBCurrentOld = stDCSample.wBTCurr[PVB];
        PVCCurrentOld = stDCSample.wBTCurr[PVC];
        PVDCurrentOld = stDCSample.wBTCurr[PVD];

        if(stAfciPara.uwSelfCheckResult != 0)
        {
            if(stAfciPara.uwSelfCheckResult& 0x01)
            {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr1  = 1;
            }
            if(stAfciPara.uwSelfCheckResult& 0x02)
            {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr2  = 1;
            }
            if(stSysCfg.uwPVNumber == 2)
            {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr3  = 0;
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr4  = 0;
            }
            else if(stSysCfg.uwPVNumber == 3)
            {
                if(stAfciPara.uwSelfCheckResult& 0x04)
                {
                    stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr3  = 1;
                }
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr4  = 0;
            }
            else if(stSysCfg.uwPVNumber == 4)
            {
                if(stAfciPara.uwSelfCheckResult& 0x04)
                {
                    stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr3  = 1;
                }
                if(stAfciPara.uwSelfCheckResult& 0x08)
                {
                    stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr4  = 1;
                }
            }
            else
            {
                if(stAfciPara.uwSelfCheckResult& 0x04)
                {
                    stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr3  = 1;
                }
                if(stAfciPara.uwSelfCheckResult& 0x08)
                {
                    stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr4  = 1;
                }
            }
            if(stSysFaultReg.unAFCISelfCheck.all != 0)
            {
                stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 1;
            }
            else
            {
                stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
            }
        }
        else
        {
            stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
            stSysFaultReg.unAFCISelfCheck.all = 0;
        }

    }
    else
    {
        if((stAfciPara1.uwArcFaultBit != 0)||(stAfciPara2.uwArcFaultBit != 0)
        ||(stAfciPara3.uwArcFaultBit != 0)||(stAfciPara4.uwArcFaultBit != 0))
        {
            stAfciPara1.uwArcFaultTimers++;
            stSysFaultReg.unFaultBit.bit.AFCIFault = 1;

            /*** AFCI Fault model 1*/
            if(stAfciPara1.uwArcFaultBit& 0x01)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr1 = 1;
            }
            if(stAfciPara1.uwArcFaultBit& 0x02)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr2 = 1;
            }
            if(stAfciPara1.uwArcFaultBit& 0x04)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr3 = 1;
            }
            if(stAfciPara1.uwArcFaultBit& 0x08)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr4 = 1;
            }

            /*** AFCI Fault model 2*/
            if(stAfciPara2.uwArcFaultBit& 0x01)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr5 = 1;
            }
            if(stAfciPara2.uwArcFaultBit& 0x02)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr6 = 1;
            }
            if(stAfciPara2.uwArcFaultBit& 0x04)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr7 = 1;
            }
            if(stAfciPara2.uwArcFaultBit& 0x08)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr8 = 1;
            }

            /*** AFCI Fault model 3*/
            if(stAfciPara3.uwArcFaultBit& 0x01)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr9 = 1;
            }
            if(stAfciPara3.uwArcFaultBit& 0x02)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr10 = 1;
            }
            if(stAfciPara3.uwArcFaultBit& 0x04)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr11 = 1;
            }
            if(stAfciPara3.uwArcFaultBit& 0x08)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr12 = 1;
            }

            /*** AFCI Fault model 4*/
            if(stAfciPara4.uwArcFaultBit& 0x01)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr13 = 1;
            }
            if(stAfciPara4.uwArcFaultBit& 0x02)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr14 = 1;
            }
            if(stAfciPara4.uwArcFaultBit& 0x04)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr15 = 1;
            }
            if(stAfciPara4.uwArcFaultBit& 0x08)
            {
                stSysFaultReg.unAFCIFault.bit.OverLimitStr16 = 1;
            }
        }
        else
        {
            stSysFaultReg.unFaultBit.bit.AFCIFault = 0;
            stSysFaultReg.unAFCIFault.all = 0;
        }

        if((stAfciPara1.uwSelfCheckResult!=0)||(stAfciPara2.uwSelfCheckResult!=0)
        ||(stAfciPara3.uwSelfCheckResult!=0)||(stAfciPara4.uwSelfCheckResult!=0))
         {
             stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 1;
             /*** AFCI self check model 1*/
             if(stAfciPara1.uwSelfCheckResult& 0x01)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr1  = 1;
             }
             if(stAfciPara1.uwSelfCheckResult& 0x02)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr2  = 1;
             }
             if(stAfciPara1.uwSelfCheckResult& 0x04)
             {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr3  = 1;
             }
             if(stAfciPara1.uwSelfCheckResult& 0x08)
             {
               stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr4 = 1;
             }
             /*** AFCI self check model 2*/
             if(stAfciPara2.uwSelfCheckResult& 0x01)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr5  = 1;
             }
             if(stAfciPara2.uwSelfCheckResult& 0x02)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr6 = 1;
             }
             if(stAfciPara2.uwSelfCheckResult& 0x04)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr7  = 1;
             }
             if(stAfciPara2.uwSelfCheckResult& 0x08)
             {
               stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr8 = 1;
             }

             /*** AFCI self check model 3*/
             if(stAfciPara3.uwSelfCheckResult& 0x01)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr9  = 1;
             }
             if(stAfciPara3.uwSelfCheckResult& 0x02)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr10 = 1;
             }
             if(stAfciPara3.uwSelfCheckResult& 0x04)
             {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr11  = 1;
             }
             if(stAfciPara3.uwSelfCheckResult& 0x08)
             {
               stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr12 = 1;
             }
             /*** AFCI self check model 4*/
             if(stAfciPara4.uwSelfCheckResult& 0x01)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr13  = 1;
             }
             if(stAfciPara4.uwSelfCheckResult& 0x02)
             {
                 stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr14 = 1;
             }
             if(stAfciPara4.uwSelfCheckResult& 0x04)
             {
                stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr15  = 1;
             }
             if(stAfciPara4.uwSelfCheckResult& 0x08)
             {
               stSysFaultReg.unAFCISelfCheck.bit.SelfCheckStr16 = 1;
             }
         }
         else
         {
            stSysFaultReg.unFaultBit.bit.AFCISelfCheck = 0;
            stSysFaultReg.unAFCISelfCheck.all = 0;
         }

    }
}



