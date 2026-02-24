/*
* task_com_f107.h
*
*	Product:			three phase inveter
*	Filename:			modbus_protocol.h
*	Purpose:			dsp and stm32 communicate
*	Date				Version		Author			Note
*   20/11/18			V1.01		zhaohui			none
*/

#ifndef MODBUS_PROTOCOL_H
#define MODBUS_PROTOCOL_H


extern Uint16 uwReadHoldingReg(Uint16 uwRegAddr,Uint16 uwRegNum,Uint16* puwTxBuf);
extern Uint16 uwSetMultipleReg(Uint16 uwRegAddr,Uint16 uwRegNum,Uint16* puwRxBuf);

#endif /* TASK_COM_F107_H */
