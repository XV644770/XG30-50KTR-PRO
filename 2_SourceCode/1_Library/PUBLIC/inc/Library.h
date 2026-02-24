/*******************************************************************************
*Constant declaration
*******************************************************************************/
#ifndef LIBRARY_H
#define LIBRARY_H

extern Uint16 uwCalcCRC16(Uint16 * puwBuf, Uint16 uwLen);
extern Uint16 uwCalcCheckSum(Uint16 * puwBuf, Uint16 uwLen);
extern Uint16 uwCalcRms(int32 dSqrtSum,Uint16 uwSqrtCnt);
extern Uint16 GetUint64Bit(Uint64 ulWordData);

#endif
