#ifndef ISR_PROTECT_H              			// Prevents multiple inclusion of this file.
#define ISR_PROTECT_H


/*************************AC Relay Current Protect***************************************/
#define INV_CURR_RELAY_MAX			960//544			// 17A * 32 = 544
#define INV_CURR_RELAY_MIN			384			// 12A * 32 = 384

/************************Three Phase Volt Sqrt Sum Protect******************************/
#define	SQRT_SUM_480V_MAX		20844		// 3*[(277*1.2*32)^2]>>14
#define	SQRT_SUM_400V_MAX		18077		// 3*[(230*1.35*32)^2]>>14
#define	SQRT_SUM_220V_MAX		5192		// 3*[(128*1.3*32)^2]>>14
#define	SQRT_SUM_5V				5			// 3*[(5*32)]^2]>>14

#define	SQRT_SUM_480V_MIN		3597		// 3*[(277*0.5*32)^2]>>14
#define	SQRT_SUM_400V_MIN		2480		// 3*[(230*0.5*32)^2]>>14
#define	SQRT_SUM_220V_MIN		768			// 3*[(128*0.5*32)^2]>>14

/************************Three Phase Volt MAX Protect******************************/
#define	PHASE_480V_MAX		15040			// 277*1.414*1.2*32
#define	PHASE_400V_MAX		14050			// 230*1.414*1.35*32
#define	PHASE_220V_MAX		7529			// 128*1.414*1.3*32
#define	PHASE_5V			227			// 50*1.414*32

#define	PHASE_480V_MIN		6267			// 277*1.414*0.5*32
#define	PHASE_400V_MIN		5204			// 230*1.414*0.5*32
#define	PHASE_220V_MIN		2896			// 128*1.414*0.5*32

#define	TAIWAN_PHASE_400V_MIN		4525			// 230V*1.414*0.43*32
/************************Three Phase Volt Island Protect******************************/
#define	ISLAND_MAX_PROTECT			3860//4480//4160		// 130V * 32 = 4160

/*************************BOOST Current Protect Time********************/
#define	ISR_BT_OCP1_TIME			1152000		// 1152000/19200 = 60S
#define	ISR_BT_OCP2_TIME			192000		// 192000/19200 = 10S

#define	ISR_CLR_ERROR_TIME     		96000		// 96000/19200 = 5S
extern void SoftOverProtect(void);
extern void RelaySoftOverCurrProtect(void);

#endif
//===========================================================================
// End of file.
//===========================================================================
