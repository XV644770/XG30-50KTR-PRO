 /*
* Sys_Init_Para.h
*
*	Product:			three phase inveter
*	Filename:			Sys_Init_Para.h
*	Purpose:			system init para
*	Date				Version		Author			Note
*   20/12/11			V1.01		chensx			none
*/
#ifndef SYS_INIT_PARA_H
#define SYS_INIT_PARA_H

#define	INVT_DEBUG				1
#define	F107_BOARD				1
#define	SAMPLE_DEBUG			0
#define	FAULT_SHUTDOWN			0
#define	PWM_OPEN_LOOP_ENABLE	0
#define	OLD_INV_VOLT_SAMPLE		0

#define	PLATFORM_CHAR				'G'			// G is on grid inverter
#define	DSPSOFTWARE_CHAR			'A'			// 3-70K is from A to E
#define	SOFTWARE_FORMAL			7
#define	SOFTWARE_TEST			6


#if SAMPLE_DEBUG
#define	VOLT_M_DEBUG				1078
#define	CURR_M_DEBUG				10
#define	DCI_M_DEBUG				2044
#define	VNPE_M_DEBUG				2042
#define	GFCI_M_DEBUG				2048
#define	ISOVOLT_M_DEBUG 			2048
#define	PVVOLT_M_DEBUG			2000
#define	PVCURR_M_DEBUG			1078

#define	INVTEMP_M_DEBUG			3750
#define	BOOSTTEMP_M_DEBUG		3895
#define	AMTEMP_M_DEBUG			3900
#endif

/*********************MAX & MIN Value Calculate define *****************************/
//#define	UPDNLMT(Var,Max,Min)   {(Var)=((Var)>=(Max))?(Max):(Var);(Var)=((Var)<=(Min))?(Min):(Var);}
#define	UPDNLMT(Var,Max,Min)\
    if(Var > Max)\
    {\
        Var = Max;\
    }\
    if(Var < Min)\
    {\
        Var = Min;\
    }\

#define	MAX2(a,b) 		(a>=b? a:b)

#define	MIN2(a,b)		(a<b? a:b)
#define	MAX3(a,b,c) 		(a>b?(a>c?a:c):(b>c?b:c))
#define	MIN3(a,b,c)		(a<b)?((a<c)?a:c):(b<c?b:c)

#define	MAX4(a,b,c,d)  	(a>b?a:b)>(c>d?c:d)?(a>b?a:b):(c>d?c:d)
/********************VAC**********************************/
#define VAC180V						1800
#define VAC100V						1000
#define VAC30V						300
#define VAC20V						200
/********************VDC**********************************/
#define VDC600V                     6000
#define VDC150V						1500     //20210831Revised
#define VDC160V                     1600     //20210831Revised
#define VDC30V						300
#define VDC20V						200
#define VDC15V                      150
#define VDC12V                      120
#define VDC10V						100
#define VDC8V                       80
#define VDC6V                       60
#define VDC5V                       50
#define VDC4V                       40
#define VDC3V						30
#define VDC2V                       20
#define VDC1V						10
#define VDC0_6V						6
#define VDC0_4V                     4
#define VDC0_3V                     3
#define VDC0_2V                     2
#define VDC0_1V                     1
/********************LIMIT WATT**********************************/
#define ACT300W						300
#define ACT500W						500
#define ACT1000W					1000
#define ACT2000W					2000
#define ACT3000W					3000
/********************AC WATT**********************************/
#define AC100W						1000
#define AC200W						2000
#define AC300W						3000
#define AC500W						5000
#define AC600W                      6000
#define AC700W                      7000
#define AC800W						8000
#define AC900W                      9000
#define AC1000W                     10000
#define AC1200W                     12000
#define AC1500W                     15000
#define AC1800W                     18000
#define AC2000W                     20000
#define AC4000W						40000
#define AC8000W						80000
#define AC12000W					120000
/********************DC WATT**********************************/
#define MPPT5W						500
#define MPPT10W						1000
#define MPPT400W					40000
#define MPPT1000W					100000
/********************Temperature**********************************/
#define TempN35oC                   -350
#define TempP85oC					850
#define TempP80oC					800
#define TempP88oC					880
#define TempP90oC					900
#define TempP92oC					920
#define TempP95oC					950
#define TempP93oC					930
#define TempP97oC					970
#define TempP98oC					980
#define TempP100oC					1000
#define TempP101oC					1010
#define TempP105oC					1050
/********************Freqency**********************************/
#define Freq80Hz						8000

/*******************ODM factory choose******************/
#define ODM_FACTORY_MAX		2

#define ODM_NONE				0
#define ODM_CHUANG_WEI		1

/*******************safety standard/country choose******************/
#define STANDARD_SAFETY_MAX				70

#define STRANDARD_CQC_2013				0
#define STRNDARD_SKYWORTH               1   //
#define STRANDARD_IRELAND_EN50549                2
#define STRNDARD_BRAZIL                 3
#define STRANDARD_SPAIN					4
#define STRANDARD_PHILIP                 5
#define STRANDARD_INDIA                  6
#define STRANDARD_BELGAIN					7
#define STRANDARD_TURKEY_EN50438                8
#define STRANDARD_NRS                    9
#define STRANDARD_AUSTRALIA_WEST         10
#define STRANDARD_NETHERLANDS            11
#define STRANDARD_THAILAND				12
#define STRANDARD_BANGKOK                13
#define STRANDARD_CQC_2018				14
#define STRNDARD_GREECE                 15
#define STRANDARD_NORWAY					16
#define STRANDARD_KOREA					17
#define STRANDARD_GERMANY               18
#define STRANDARD_FRANCE                19
#define STRANDARD_IRELAND               20
#define STRANDARD_TURKEY                21
#define STRANDARD_TAIWAI					22
#define STRANDARD_ITALY_CEIO16			23
#define STRANDARD_SLOVAKIA				24
#define STRANDARD_ROMANIA                   25
#define STRANDARD_ITALY_CEI021_INTERNAL         26
#define STRANDARD_ITALY_CEI021_EXTERNAL         27
#define STRANDARD_ITALY_CEI021_ARETI            28
#define STRANDARD_DENMARK                       29
#define STRANDARD_GREECE_ISLAND                 30
#define STRANDARD_ENGLAND_G59               31
#define STRANDARD_ENGLAND_G99               32
#define STRANDARD_ENGLAND_G83               33
#define STRANDARD_ENGLAND_G98               34
#define STRANDARD_POLAND                    35
#define STRANDARD_GERMANY_BDEW                  36
#define STRANDARD_GERMANY_AVDE0126              37
#define STRANDARD_IEC_EN61727                   38
#define STRANDARD_SWEDEN                        39
#define STRANDARD_EUROPE_GENERAL                40
#define STRANDARD_CYPRUS                    41
#define STRANDARD_NEWZEALAND                42
#define STRANDARD_SLOVAKIA_SSE30            43
#define STRANDARD_SLOVAKIA_ZSD31            44
#define STRANDARD_UKRAINE                   45
#define STRANDARD_MEXICO                        46
#define STRANDARD_FRANCE_ARRETE23               47
#define STRANDARD_DENMARK_TR322                 48
#define STRANDARD_DENMARK_TR321                 49
#define STRANDARD_DUBAI                         50
#define STRANDARD_VICTORIA                  51
#define STRANDARD_SWEDEN_EN50438            52
#define STRANDARD_AU_SA                     53
#define STRANDARD_AU_QLD                    54
#define STRANDARD_AU_VAR                    55
#define STRANDARD_AUSGRID                       56
#define STRANDARD_HORIZON                       57
#define STRANDARD_CROATIA                       58
#define STRANDARD_LITHUANIA                     59
#define STRANDARD_ESTONIA                       60
#define STRANDARD_MALAYSIA                  61
#define STRANDARD_GERMANY_VDE4110           62
#define STRANDARD_CHILE                     63
#define STRANDARD_AUSTRIA                   64
#define STRANDARD_KERALA                    65
/*********************AC Voltage Level******************************/
#define STANDARD_VOLTAGE_MAX		3
#define	AC_220V_LEVEL     			0x00
#define	AC_400V_LEVEL     			0x01
#define	AC_480V_LEVEL     			0x02
/********************MOdel ID Para***********************************/
#define 	MACHINE_ID_3_6KW			0
#define 	MACHINE_ID_7_9KW			2
#define 	MACHINE_ID_10_13KW		4
#define     MACHINE_ID_15_22KW     	5
#define 	MACHINE_ID_25KW			6
#define 	MACHINE_ID_25_33KW		8
#define 	MACHINE_ID_36_40KW		9
#define 	MACHINE_ID_50KW			12	
#define 	MACHINE_ID_60_70KW		13	
#define   MACHINE_ID_PARALLEL_TUBE_15_22KW      14
#define 	MACHINE_ID_PARALLEL_TUBE_25KW		15
#define 	MACHINE_ID_Plus_30_36KW		17
#define 	MACHINE_ID_Plus_50KW			18
#define 	MACHINE_ID_Plus_25_30KW		21
#define     MACHINE_ID_15_22KW_MAX      22	//32A
#define 	MACHINE_ID_25KW_MAX			23	//32A
#define 	MACHINE_ID_30_33KW_MAX		24	//32A
#define 	MACHINE_ID_36_40KW_MAX		25	//32A
#define 	MACHINE_ID_PRO_33_40KW		27
#define 	MACHINE_ID_PRO_50KW			28
#define     MACHINE_ID_PRO_50KW_2       31
#define 	MACHINE_ID_PRO_60KW			29

// LL400V CONFIG
// ID_3_6KW
#define 	CONFIG_3KW				30		
#define 	CONFIG_4KW				40		
#define 	CONFIG_5KW				50		
#define 	CONFIG_6KW				60
// ID_7_9KW
#define 	CONFIG_7KW				70		
#define 	CONFIG_8KW				80	
#define 	CONFIG_9KW				90
// ID_10_13KW
#define 	CONFIG_10KW				100		
#define 	CONFIG_11KW				110		
#define 	CONFIG_12KW				120		
#define 	CONFIG_13KW				130	
// ID_15_25KW
#define 	CONFIG_15KW				150	
#define 	CONFIG_17KW				170		
#define 	CONFIG_20KW				200		
#define 	CONFIG_22KW				220		
#define 	CONFIG_25KW				250	
// ID_25_40KW
//#define 	CONFIG_25KW				250
#define 	CONFIG_30KW				300	
#define 	CONFIG_33KW				330	
#define 	CONFIG_36KW				360	
#define 	CONFIG_40KW				400
// ID_50_70KW
#define 	CONFIG_50KW				500		
#define 	CONFIG_60KW				600	


// LL220V CONFIG
// ID_3_6KW
#define 	CONFIG_1K5W				15
#define 	CONFIG_2K2W				22
#define 	CONFIG_2K8W				28
#define 	CONFIG_3K3W				28
// ID_7_9KW
#define 	CONFIG_3K9W				39
#define 	CONFIG_4K4W				44		
//#define 	CONFIG_5KW				80
// ID_10_13KW
#define 	CONFIG_5K5W				55	
#define 	CONFIG_6K1W				61	
#define 	CONFIG_6K6W				66
#define 	CONFIG_7K2W				72
#define 	CONFIG_7K5W				75
// ID_15_25KW
#define 	CONFIG_8K3W				83	
#define 	CONFIG_9K4W				94		
#define 	CONFIG_11KW				110		
#define 	CONFIG_12K5W				125		
#define 	CONFIG_13K4W				134	
#define 	CONFIG_13K8W				138	
#define 	CONFIG_15KW				150	
// ID_25_40KW
//#define 	CONFIG_13K4W				134	
#define 	CONFIG_16K5W				165		
#define 	CONFIG_18K2W				182		
#define 	CONFIG_19K8W				198		
#define 	CONFIG_22KW				220
#define 	CONFIG_25KW				250
// ID_50_70KW
#define 	CONFIG_27K5W				275	
#define 	CONFIG_33KW				330		
#define 	CONFIG_38K5W				385
#define 	CONFIG_43K2W				432
#define 	CONFIG_60KW				600

// ID_Pro_50_60KW
#define 	CONFIG_50KW				500	
#define 	CONFIG_60KW				600		

// LL480V CONFIG
// ID_3_6KW
#define 	CONFIG_3K5W				35	
#define 	CONFIG_3K6W				36		
#define 	CONFIG_4K8W				48		
//#define 	CONFIG_6KW				60		
//#define 	CONFIG_7K2W				72
// ID_7_9KW
#define 	CONFIG_8K4W				84		
#define 	CONFIG_9K6W				96	
#define 	CONFIG_10K8W				108
// ID_10_13KW
#define 	CONFIG_12KW				120		
#define 	CONFIG_13K2W				130		
#define 	CONFIG_14K4W				144		
#define 	CONFIG_15K6W				156	
// ID_15_25KW
#define 	CONFIG_18KW				180	
#define 	CONFIG_20K4W				204		
#define 	CONFIG_24KW				240		
#define 	CONFIG_26K4W				264		
//#define 	CONFIG_30KW				300	
// ID_25_40KW
//#define 	CONFIG_30KW				300
#define 	CONFIG_36KW				360	
#define 	CONFIG_39K6W				396	
#define 	CONFIG_43K2W				432	
#define 	CONFIG_48KW				480
// ID_50_70KW
//#define 	CONFIG_60KW				600		
#define 	CONFIG_70KW				700	
/*********************XG 3-6KW Para******************************/
#define	AC_CURR_RATIO_3_6K		1083			// 32~1A	Iac =[(Vsample-2048)*2*1083]>>10
#define	DCI_RATIO_3_6K				1088//35			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_3_6K		262//248			// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_3_6K		2198
#define	PV_CURR_RATIO_3_6K		221				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_3_6K		2335			// 10~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_3_6K		2048
#define	PV_VOLT_RATIO_3_6K		176				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_3_6K		91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_3_6K		305				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_3_6K				2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>7
#define	NPE_RATIO_3_6K				439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_3_6K				3750			//	3750 /6250 = 0.6	
#define 	PWM_DBTIME_3_6K			144				// 1.2us @120M
#define	RELAY_PWM_3_6K			3900			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_3_6K		75				// 75%
#define 	CURR_RLYMAX_3_6K			544				// 17A * 32 
#define 	BTCURR_LIMIT_3_6K			416				// 13 *32
#define 	TEMP_OVER_INV_3_6K		TempP93oC		//  
#define 	TEMP_OVER_BST_3_6K		TempP93oC		// 
#define 	TEMP_OVER_AMB_3_6K		TempP93oC		// 
#define 	TEMP_DERAT_INV_3_6K		TempP80oC		//  
#define 	TEMP_DERAT_BST_3_6K		TempP80oC	
#define 	TEMP_DERAT_AMB_3_6K		TempP80oC	
#define 	DRAT_IVBST_STEP_3_6K		2000			// 200W
#define 	DRAT_IVBST_BKSTEP_3_6K	1000			// 100W
#define 	DRAT_AMB_STEP_3_6K		2000			// 200W
#define 	DRAT_AMB_BKSTEP_3_6K		1000			// 100W

#define 	BUSSOFT_CNTL_Kp_3_6K		1025
#define 	BUSSOFT_CNTL_Ki_3_6K		60

#define 	PVVOLT_CNTL_Kp_3_6K		3000
#define 	PVVOLT_CNTL_Ki_3_6K		24

#define 	BTCURR_CNTL_Kp_3_6K		245
#define 	BTCURR_CNTL_Ki_3_6K		56

#define 	ACTPOWR_CNTL_Kp_3_6K		816
#define 	ACTPOWR_CNTL_Ki_3_6K		88

#define 	IVVOLT_CNTL_HiRKd_3_6K	720
#define 	IVVOLT_CNTL_FstDKd_3_6K	240

#define 	INVCURR_CNTL_Kd_3_6K		2000
#define 	INVCURR_CNTL_Kq_3_6K		173
#define 	INVCURR_CNTL_Kpi_3_6K		64000

#define 	IVBUSVOLT_CNTL_Kp_H_3_6K	7000
#define 	IVBUSVOLT_CNTL_Ki_H_3_6K	18
#define 	IVBUSVOLT_CNTL_Kp_L_3_6K	7000
#define 	IVBUSVOLT_CNTL_Ki_L_3_6K	18

#define 	INVCURR_CNTL_KPR_3_6K		666
#define 	INVCURR_CNTL_KPR1_3_6K		51
#define 	INVCURR_CNTL_KPR2_3_6K		972

/*********************XG 7-9KW Para******************************/
#define	AC_CURR_RATIO_7_9K		1083			// 32~1A	Iac =[(Vsample-2048)*2*1083]>>10
#define	DCI_RATIO_7_9K				1088			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_7_9K		262//248			// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_7_9K		2198
#define	PV_CURR_RATIO_7_9K		221				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_7_9K		2335			// 10~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_7_9K		2048
#define	PV_VOLT_RATIO_7_9K		176				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_7_9K		91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_7_9K		305				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_7_9K				2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>7
#define	NPE_RATIO_7_9K				439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_7_9K				3750			//	3750 /6250 = 0.6	
#define 	PWM_DBTIME_7_9K			144				// 1.2us @120M
#define	RELAY_PWM_7_9K			3900			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_7_9K		75				// 75%
#define 	CURR_RLYMAX_7_9K			544				// 17A * 32 
#define 	BTCURR_LIMIT_7_9K			416				// 13 *32
#define 	TEMP_OVER_INV_7_9K		TempP93oC		//  
#define 	TEMP_OVER_BST_7_9K		TempP93oC		// 
#define 	TEMP_OVER_AMB_7_9K		TempP93oC		// 
#define 	TEMP_DERAT_INV_7_9K		TempP80oC		//  
#define 	TEMP_DERAT_BST_7_9K		TempP80oC	 
#define 	TEMP_DERAT_AMB_7_9K		TempP80oC	
#define 	DRAT_IVBST_STEP_7_9K		3000			// 300W
#define 	DRAT_IVBST_BKSTEP_7_9K	1500			// 150W
#define 	DRAT_AMB_STEP_7_9K		3000			// 300W
#define 	DRAT_AMB_BKSTEP_7_9K		1500			// 150W

#define 	BUSSOFT_CNTL_Kp_7_9K		1025
#define 	BUSSOFT_CNTL_Ki_7_9K		60

#define 	PVVOLT_CNTL_Kp_7_9K		3000
#define 	PVVOLT_CNTL_Ki_7_9K		24

#define 	BTCURR_CNTL_Kp_7_9K		245
#define 	BTCURR_CNTL_Ki_7_9K		56

#define 	ACTPOWR_CNTL_Kp_7_9K		816
#define 	ACTPOWR_CNTL_Ki_7_9K		88

#define 	IVVOLT_CNTL_HiRKd_7_9K	720
#define 	IVVOLT_CNTL_FstDKd_7_9K	240

#define 	INVCURR_CNTL_Kd_7_9K		2000
#define 	INVCURR_CNTL_Kq_7_9K		173
#define 	INVCURR_CNTL_Kpi_7_9K		64000

#define 	IVBUSVOLT_CNTL_Kp_H_7_9K	7000
#define 	IVBUSVOLT_CNTL_Ki_H_7_9K	18
#define 	IVBUSVOLT_CNTL_Kp_L_7_9K	7000
#define 	IVBUSVOLT_CNTL_Ki_L_7_9K	18

#define 	INVCURR_CNTL_KPR_7_9K		666
#define 	INVCURR_CNTL_KPR1_7_9K		51
#define 	INVCURR_CNTL_KPR2_7_9K		972
/*********************XG 10-13KW Para******************************/
#define	AC_CURR_RATIO_10_13K		1083			// 32~1A	Iac =[(Vsample-2048)*2*1083]>>10
#define	DCI_RATIO_10_13K			1088			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_10_13K		262//248			// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_10_13K		2198
#define	PV_CURR_RATIO_10_13K		221				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_10_13K	2335			// 1~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_10_13K		2048
#define	PV_VOLT_RATIO_10_13K		176				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_10_13K		91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_10_13K		305				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_10_13K			2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>7
#define	NPE_RATIO_10_13K			439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_10_13K			3750			//	3750 /6250 = 0.6	
#define 	PWM_DBTIME_10_13K		144				// 1.2us @120M
#define	RELAY_PWM_10_13K			3900			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_10_13K		75				// 75%
#define 	CURR_RLYMAX_10_13K		544				// 17A * 32 
#define 	BTCURR_LIMIT_10_13K		416				// 13 *32
#define 	TEMP_OVER_INV_10_13K		TempP93oC		//  
#define 	TEMP_OVER_BST_10_13K		TempP93oC		//
#define 	TEMP_OVER_AMB_10_13K		TempP93oC		// 
#define 	TEMP_DERAT_INV_10_13K		TempP80oC		//  
#define 	TEMP_DERAT_BST_10_13K	TempP80oC	
#define 	TEMP_DERAT_AMB_10_13K	TempP80oC	
#define 	DRAT_IVBST_STEP_10_13K	4000			//400W
#define 	DRAT_IVBST_BKSTEP_10_13K	2000			// 200W
#define 	DRAT_AMB_STEP_10_13K		4000			//400W
#define 	DRAT_AMB_BKSTEP_10_13K	2000			// 200W

#define 	BUSSOFT_CNTL_Kp_10_13K	1025
#define 	BUSSOFT_CNTL_Ki_10_13K	60

#define 	PVVOLT_CNTL_Kp_10_13K		3000
#define 	PVVOLT_CNTL_Ki_10_13K		24

#define 	BTCURR_CNTL_Kp_10_13K		245
#define 	BTCURR_CNTL_Ki_10_13K		56

#define 	ACTPOWR_CNTL_Kp_10_13K	816
#define 	ACTPOWR_CNTL_Ki_10_13K	88

#define 	IVVOLT_CNTL_HiRKd_10_13K	720
#define 	IVVOLT_CNTL_FstDKd_10_13K	240

#define 	INVCURR_CNTL_Kd_10_13K	2000
#define 	INVCURR_CNTL_Kq_10_13K	173
#define 	INVCURR_CNTL_Kpi_10_13K	64000

#define 	IVBUSVOLT_CNTL_Kp_H_10_13K	7000
#define 	IVBUSVOLT_CNTL_Ki_H_10_13K	18
#define 	IVBUSVOLT_CNTL_Kp_L_10_13K	7000
#define 	IVBUSVOLT_CNTL_Ki_L_10_13K	18

#define 	INVCURR_CNTL_KPR_10_13K		666
#define 	INVCURR_CNTL_KPR1_10_13K		51
#define 	INVCURR_CNTL_KPR2_10_13K		972

/**************XG 15-22KW Parallel Tube Para***********************/
#define 	PWM_DBTIME_PARALLEL_TUBE_15_25K		180				// 1.5us @120M

/*********************XG 15-22KW Para******************************/
#define PV_CURR_RATIO_15_22K        221             // 32~1A    Ipv =[(Vsample-372)*2*221]>>10
#define BTCURR_LIMIT_15_22K			896				// 14*2*32	Chuangwei_PV_String 14A
//#define BTCURR_LIMIT_15_22K        832            // 26*32
//#define BTCURR_LIMIT_15_22K        784              // 24.5*32   Isample=26A,Iactual=27.54A

/*********************XG 25-30KW plus******************************/
#define PV_CURR_RATIO_25_30K        	352             // 32~1A    Ipv =[(Vsample-372)*2*221]>>10
#define BTCURR_LIMIT_25_30K		1280				// 20*2*32	Chuangwei_PV_String 20A
#define	AC_CURR_RATIO_25_30K		862				// 32~1A	Iac =[(Vsample-2048)*2*862]>>10
#define	DCI_RATIO_25_30K			1493				// 1000~1A	Idci=[(Vsample-2048)*2*1493]>>11
#define	STR_CURR_RATIO_25_30K	840				// 100~1A	Istr=[(Vsample-683)*2*703]>>10
#define 	STR_CURR_BIAS_25_30K        1301          	  //  Theoretical:1.6667V,Actual:1.60V
#define 	CURR_RLYMAX_25_30K		480//1920				// 20A * 32
#define 	IVVOLT_CNTL_HiRKd_25_30K_PLUS		720
#define 	IVVOLT_CNTL_FstDKd_25_30K_PLUS		290//240
#define 	TEMP_OVER_INV_25_30K		TempP95oC		//  
#define 	TEMP_OVER_BST_25_30K		TempP95oC		//  
#define 	TEMP_OVER_AMB_25_30K		TempP101oC		//

#define 	TEMP_DERAT_INV_25_30K	TempP92oC		//  
#define 	TEMP_DERAT_BST_25_30K	TempP92oC	
#define 	TEMP_DERAT_AMB_25_30K	TempP97oC
/*********************XG 25KW Para******************************/
#define	AC_CURR_RATIO_15_25K		609				// 32~1A	Iac =[(Vsample-2048)*2*609]>>10
#define	DCI_RATIO_15_25K			1493				// 1000~1A	Idci=[(Vsample-2048)*2*1493]>>11
#define	STR_CURR_RATIO_15_25K		703//248			// 100~1A	Istr=[(Vsample-683)*2*703]>>10
#define STR_CURR_BIAS_15_25K        2198            //  Theoretical:1.6667V,Actual:1.60V
//#define	PVA_CURR_RATIO_25K		221				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define PV_CURR_RATIO_25K       353             // 32~1A    Ipv =[(Vsample-372)*2*353]>>10,20211224Add
#define	GFCI_CURR_RATIO_15_25K      8805			// 10~1mA  	Igfci=[(Vsample-1457)*2*38635]>>11,20220513Revised
#define	GFCI_CURR_BIAS_15_25K		2555            // V_HardwareBias = 1.87V

#define	PV_VOLT_RATIO_15_25K		187				// 10~1V	Vpv =[(Vsample-0)*2*187]>>7
#define	BUS_VOLT_RATIO_15_25K		91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_15_25K		325				// 32~1V	Vac =[(Vsample-2048)*2*325]>>6
#define	ISO_RATIO_15_25K			2885				// 10~1V	Viso=[(Vsample-361)*2*180]>>11
#define	NPE_RATIO_15_25K			439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_15_25K			3750//3437//4812			//	3750/6250 = 0.6
#define 	PWM_DBTIME_15_25K		144				// 1.2us @120M
#define	RELAY_PWM_15_25K			4500			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_15_25K		75				// 75%
#define 	CURR_RLYMAX_15_25K		544				// 17A * 32
#define     BTCURR_LIMIT_25K        1344             // 14*3*32	Chuangwei_PV_String 14A
//#define     BTCURR_LIMIT_25K        1248             // 39 *32
//#define     BTCURR_LIMIT_25K        1200             // 37.5 *32  Isample=38.96A,Iactual=40.43A
#define 	TEMP_OVER_INV_15_25K		TempP105oC		//
#define 	TEMP_OVER_BST_15_25K		TempP105oC		//
#define 	TEMP_OVER_AMB_15_25K		TempP85oC		//
#define 	TEMP_DERAT_INV_15_25K		TempP100oC		//
#define 	TEMP_DERAT_BST_15_25K	TempP100oC
#define 	TEMP_DERAT_AMB_15_25K	TempP80oC
#define 	DRAT_IVBST_STEP_15_25K	2000			// 800W
#define 	DRAT_IVBST_BKSTEP_15_25K	1000			// 400W
#define 	DRAT_AMB_STEP_15_25K		1000			// 800W
#define 	DRAT_AMB_BKSTEP_15_25K	1000			// 400W

#define 	BUSSOFT_CNTL_Kp_15_25K	1025
#define 	BUSSOFT_CNTL_Ki_15_25K	60

#define 	PVVOLT_CNTL_Kp_15_25K		3000
#define 	PVVOLT_CNTL_Ki_15_25K		100 // 24

#define 	BTCURR_CNTL_Kp_15_25K		2000
#define 	BTCURR_CNTL_Ki_15_25K		300//56
#define 	FAST_BTCURR_CNTL_Ki_15_25K		400//56

#define 	ACTPOWR_CNTL_Kp_15_25K	816
#define 	ACTPOWR_CNTL_Ki_15_25K	88

#define 	FAST_ACTPOWR_CNTL_Kp_15_25K	50
#define 	FAST_ACTPOWR_CNTL_Ki_15_25K	80

#define 	IVVOLT_CNTL_HiRKd_15_25K		0
#define 	IVVOLT_CNTL_FstDKd_15_25K		0

#define 	INVCURR_CNTL_Kd_15_25K	2100
#define 	INVCURR_CNTL_Kq_15_25K	173
#define 	INVCURR_CNTL_Kpi_15_25K	300000

#define 	IVBUSVOLT_CNTL_Kp_H_15_25K	10000
#define 	IVBUSVOLT_CNTL_Ki_H_15_25K	32
#define 	IVBUSVOLT_CNTL_Kp_L_15_25K	3000
#define 	IVBUSVOLT_CNTL_Ki_L_15_25K	10
#define 	TAIWAN_IVBUSVOLT_CNTL_Kp_H_15_25K	18000
#define 	TAIWAN_IVBUSVOLT_CNTL_Ki_H_15_25K	10

#define 	INVCURR_CNTL_KPR_15_25K		666
#define 	INVCURR_CNTL_KPR1_15_25K		51
#define 	INVCURR_CNTL_KPR2_15_25K		972
/*********************XG 25-40KW Para******************************/
#define	AC_CURR_RATIO_25_40K		1271//			// 32~1A	Iac =[(Vsample-2048)*2*1083]>>10
#define	DCI_RATIO_25_40K			76//2388			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_25_40K	562//703				// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_25_40K		1301//1821//2198
#define	PV_CURR_RATIO_25_40K		220				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_25_40K	8805//2856			// 10~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_25_40K		2554//2226
#define	PV_VOLT_RATIO_25_40K		187				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_25_40K	91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_25_40K		325				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_25_40K			180//2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>11
#define	NPE_RATIO_25_40K			707//1404//439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_25_40K			4687			//	4687 /6250 = 0.75	
#define 	OCP_CAP_PLUS_25_40K		2813			//	2813 /6250 = 0.45	
#define 	PWM_DBTIME_25_40K		144				// 1.2us @120M
#define	RELAY_PWM_25_40K			4500			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_25_40K		75				// 75%
#define 	CURR_RLYMAX_25_40K		800//960		//36000/230*1.414/3*1.2*1.2*0.9=90A ;90A* 32 		//30A * 32
#define 	BTCURR_LIMIT_25_40K		1024//32A*32=1024//864//832				// 26 *32       // chuangwei:27*32
#define 	TEMP_OVER_INV_25_40K		TempP95oC		//  
#define 	TEMP_OVER_BST_25_40K		TempP95oC		//  
#define 	TEMP_OVER_AMB_25_40K		TempP90oC		//

#define 	TEMP_DERAT_INV_25_40K	TempP85oC		//  
#define 	TEMP_DERAT_BST_25_40K	TempP90oC	
#define 	TEMP_DERAT_AMB_25_40K	TempP85oC

#define 	DRAT_IVBST_STEP_25_40K	2000			// 200W
#define 	DRAT_IVBST_BKSTEP_25_40K	1000			// 100W
#define 	DRAT_AMB_STEP_25_40K		2000			// 200W
#define 	DRAT_AMB_BKSTEP_25_40K	1000			// 100W

#define 	BUSSOFT_CNTL_Kp_25_40K	1025
#define 	BUSSOFT_CNTL_Ki_25_40K	60

#define 	PVVOLT_CNTL_Kp_25_40K		3000
#define 	PVVOLT_CNTL_Ki_25_40K		100//24

#define 	BTCURR_CNTL_Kp_25_40K		2000
#define 	BTCURR_CNTL_Ki_25_40K		300//56

#define 	ACTPOWR_CNTL_Kp_25_40K	816
#define 	ACTPOWR_CNTL_Ki_25_40K	88

#define 	IVVOLT_CNTL_HiRKd_25_40K		720
#define 	IVVOLT_CNTL_FstDKd_25_40K		290//240

#define 	INVCURR_CNTL_Kd_25_40K	1200    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_25_40K	173
#define 	INVCURR_CNTL_Kpi_25_40K	32000   //20210831Revised 64000 To 32000

#define 	INVCURR_CNTL_Kd_50K_PLUS	2100    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_50K_PLUS	173
#define 	INVCURR_CNTL_Kpi_50K_PLUS	32000   //20210831Revised 64000 To 32000

#define 	IVBUSVOLT_CNTL_Kp_H_25_40K	24000
#define 	IVBUSVOLT_CNTL_Ki_H_25_40K	32
#define 	IVBUSVOLT_CNTL_Kp_L_25_40K	24000
#define 	IVBUSVOLT_CNTL_Ki_L_25_40K	32

#define 	INVCURR_CNTL_KPR_25_40K		666
#define 	INVCURR_CNTL_KPR1_25_40K		51
#define 	INVCURR_CNTL_KPR2_25_40K		972
/*********************XG Plus 50KW Sample Ratio******************************/
#define 	TEMP_OVER_INV_50K_PLUS	TempP95oC		
#define 	TEMP_OVER_BST_50K_PLUS	TempP95oC		  
#define 	TEMP_OVER_AMB_50K_PLUS	TempP100oC		

#define 	TEMP_DERAT_INV_50K_PLUS	TempP88oC		 
#define 	TEMP_DERAT_BST_50K_PLUS	TempP88oC	
#define 	TEMP_DERAT_AMB_50K_PLUS	TempP95oC

#define 	OCP_CAP_PLUS_50K_PLUS		3438			//	3438 /6250 = 0.55	
/*********************XG 50-70KW Sample Ratio******************************/
#define 	AC_CURR_RATIO_60_70K	1195	// 32~1A	Iac =[(Vsample-2048)*2*1195]>>10
#define 	DCI_RATIO_50_70K		60		// 32~1A	Idci=[(Vsample-2048)*2*60]>>11
#define 	STR_CURR_RATIO_50_70K	703		// 32~1A	Istr=[(Vsample-2276)*2*703]>>10
#define 	STR_CURR_BIAS_50_70K	2276
#define 	PV_CURR_RATIO_60_70K	352		// 32~1A	Ipv =[(Vsample-372)*2*352]>>10
#define 	GFCI_CURR_RATIO_50_70K	4244	// 10~1mA  	Igfci=[(Vsample-2048)*2*248]>>11
#define 	GFCI_CURR_BIAS_50_70	2436
#define 	PV_VOLT_RATIO_50_70K	194		// 10~1V	Vpv =[(Vsample-0)*2*188]>>7
#define 	BUS_VOLT_RATIO_50_70K	91		// 10~1V 	Vbus=[(Vsample-0)*2*88]>>7
#define		AC_VOLT_RATIO_50_70K	339		// 32~1V	Vac =[(Vsample-2048)*2*286]>>6
#define 	ISO_RATIO_50_70K		2628	// 10~1V	Viso=[(Vsample-361)*2*2615]>>11
#define 	NPE_RATIO_50_70K		1405	// 10~1V	Vnpe=[(Vsample-2048)*2*1532]>>7

#define 	OCP_CAP_50_70K			4250		//	4250 /6250 = 0.68
#define 	PWM_DBTIME_50_70K		192				// 1.6us @120M
#define	RELAY_PWM_50_70K		3900			// 3900 / 6000 = 0.65
#define	RELAY_M0_Duty_50_70K		55				// 75%
#define 	CURR_RLYMAX_50_70K		1600			//50A * 32
#define 	BTCURR_LIMIT_50_70K		1120			// 35 *32
#define 	TEMP_OVER_INV_50_70K		1050		//  105degree
#define 	TEMP_OVER_BST_50_70K		1050		//  105degree
#define 	TEMP_OVER_AMB_50_70K		950			//  95degree
#define 	TEMP_DERAT_INV_50_70K		1000		//  100degree
#define 	TEMP_DERAT_BST_50_70K		1000		//  100degree
#define 	TEMP_DERAT_AMB_50_70K		900			//  90degree

#define 	DRAT_IVBST_STEP_50_70K	20000			// 2KW
#define 	DRAT_IVBST_BKSTEP_50_70K	10000			// 1KW
#define 	DRAT_AMB_STEP_50_70K		20000			// 2KW
#define 	DRAT_AMB_BKSTEP_50_70K	10000			// 1KW

#define 	BUSSOFT_CNTL_Kp_50_70K	1025
#define 	BUSSOFT_CNTL_Ki_50_70K	60

#define 	PVVOLT_CNTL_Kp_50_70K		3450
#define 	PVVOLT_CNTL_Ki_50_70K		100

#define 	BTCURR_CNTL_Kp_50_70K		2225
#define 	BTCURR_CNTL_Ki_50_70K		300

#define 	ACTPOWR_CNTL_Kp_50_70K	816
#define 	ACTPOWR_CNTL_Ki_50_70K	88

#define 	IVVOLT_CNTL_HiRKd_50_70K		720
#define 	IVVOLT_CNTL_FstDKd_50_70K		240

#define 	INVCURR_CNTL_Kd_50_70K	1016
#define 	INVCURR_CNTL_Kq_50_70K	173
#define 	INVCURR_CNTL_Kpi_50_70K	20000

#define 	IVBUSVOLT_CNTL_Kp_H_50_70K	4800
#define 	IVBUSVOLT_CNTL_Ki_H_50_70K	532
#define 	IVBUSVOLT_CNTL_Kp_L_50_70K	4800
#define 	IVBUSVOLT_CNTL_Ki_L_50_70K	532

#define 	INVCURR_CNTL_KPR_50_70K		666
#define 	INVCURR_CNTL_KPR1_50_70K		51
#define 	INVCURR_CNTL_KPR2_50_70K		972

/*********************XG 33-50KW Pro Para******************************/
#define	AC_CURR_RATIO_33_50K		1271//			// 32~1A	Iac =[(Vsample-2048)*2*1271]>>10
#define	DCI_RATIO_33_50K			2388//2388			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_33_50K	839//703				// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_33_50K		1301//1821//2198
#define	PV_CURR_RATIO_33_50K		350				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_33_50K	8805//2856			// 10~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_33_50K		2554//2226
#define	PV_VOLT_RATIO_33_50K		187				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_33_50K	91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_33_50K		325				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_33_50K			180//2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>11
#define	NPE_RATIO_33_50K			707//1404//439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_33_50K			4687			//	4687 /6250 = 0.75	
#define 	OCP_CAP_PRO_33_50K		2813			//	2813 /6250 = 0.45	
#define 	PWM_DBTIME_33_50K		144				// 1.2us @120M
#define	RELAY_PWM_33_50K			4500			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_33_50K		75				// 75%
#define 	CURR_RLYMAX_33_50K		800//2880//960		//36000/230*1.414/3*1.2*1.2*0.9=90A ;90A* 32 		//30A * 32
#define 	BTCURR_LIMIT_33_50K		1280//40A*32=1280//864//832				// 26 *32       // chuangwei:27*32
#define 	TEMP_OVER_INV_33_50K		TempP95oC		//  
#define 	TEMP_OVER_BST_33_50K		TempP95oC		//  
#define 	TEMP_OVER_AMB_33_50K		TempP90oC		//

#define 	TEMP_DERAT_INV_33_50K	TempP85oC		//  
#define 	TEMP_DERAT_BST_33_50K	TempP90oC	
#define 	TEMP_DERAT_AMB_33_50K	TempP85oC

#define 	DRAT_IVBST_STEP_33_50K	2000			// 200W
#define 	DRAT_IVBST_BKSTEP_33_50K	1000			// 100W
#define 	DRAT_AMB_STEP_33_50K		2000			// 200W
#define 	DRAT_AMB_BKSTEP_33_50K	1000			// 100W

#define 	BUSSOFT_CNTL_Kp_33_50K	1025
#define 	BUSSOFT_CNTL_Ki_33_50K	60

#define 	PVVOLT_CNTL_Kp_33_50K		3000
#define 	PVVOLT_CNTL_Ki_33_50K		100//24

#define 	BTCURR_CNTL_Kp_33_50K		2000
#define 	BTCURR_CNTL_Ki_33_50K		300//56

#define 	ACTPOWR_CNTL_Kp_33_50K	816
#define 	ACTPOWR_CNTL_Ki_33_50K	88

#define 	IVVOLT_CNTL_HiRKd_33_50K		720
#define 	IVVOLT_CNTL_FstDKd_33_50K		290//240

#define 	INVCURR_CNTL_Kd_33_50K	1200    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_33_50K	173
#define 	INVCURR_CNTL_Kpi_33_50K	32000   //20210831Revised 64000 To 32000

#define 	INVCURR_CNTL_Kd_33_50K_PRO	2100    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_33_50K_PRO	173
#define 	INVCURR_CNTL_Kpi_33_50K_PRO	32000   //20210831Revised 64000 To 32000

#define 	IVBUSVOLT_CNTL_Kp_H_33_50K	24000
#define 	IVBUSVOLT_CNTL_Ki_H_33_50K	32
#define 	IVBUSVOLT_CNTL_Kp_L_33_50K	24000
#define 	IVBUSVOLT_CNTL_Ki_L_33_50K	32

#define 	INVCURR_CNTL_KPR_33_50K		666
#define 	INVCURR_CNTL_KPR1_33_50K		51
#define 	INVCURR_CNTL_KPR2_33_50K		972

#define 	TEMP_OVER_INV_33_50K_PRO	TempP98oC		
#define 	TEMP_OVER_BST_33_50K_PRO	TempP95oC		  
#define 	TEMP_OVER_AMB_33_50K_PRO	TempP100oC		

#define 	TEMP_DERAT_INV_33_50K_PRO	TempP92oC		 
#define 	TEMP_DERAT_BST_33_50K_PRO	TempP92oC	
#define 	TEMP_DERAT_AMB_33_50K_PRO	TempP95oC

#define 	OCP_CAP_PRO_33_40K_PRO		3250			//	3250 /6250 = 0.52	
#define 	OCP_CAP_PRO_50K_PRO		3250			//	3250 /6250 = 0.52	

/*********************XG 60KW Pro Para******************************/
#define	AC_CURR_RATIO_60K		1532//			// 32~1A	Iac =[(Vsample-2048)*2*1532]>>10
#define	DCI_RATIO_60K			76//2388			// 1000~1A	Idci=[(Vsample-2048)*2*35]>>11
#define	STR_CURR_RATIO_60K	839//703				// 100~1A	Istr=[(Vsample-683)*2*79]>>10
#define STR_CURR_BIAS_60K		1301//1821//2198
#define	PV_CURR_RATIO_60K		350				// 32~1A	Ipv =[(Vsample-372)*2*221]>>10
#define	GFCI_CURR_RATIO_60K	8805//2856			// 10~1mA  	Igfci=[(Vsample-2226)*2*287]>>11
#define	GFCI_CURR_BIAS_60K		2554//2226
#define	PV_VOLT_RATIO_60K		187				// 10~1V	Vpv =[(Vsample-0)*2*176]>>7
#define	BUS_VOLT_RATIO_60K	91				// 10~1V 	Vbus=[(Vsample-0)*2*91]>>7
#define	AC_VOLT_RATIO_60K		325				// 32~1V	Vac =[(Vsample-2048)*2*305]>>6
#define	ISO_RATIO_60K			180//2886				// 10~1V	Viso=[(Vsample-361)*2*180]>>11
#define	NPE_RATIO_60K			707//1404//439				// 10~1V	Vnpe=[(Vsample-2048)*2*439]>>7

#define 	OCP_CAP_60K			4687			//	4687 /6250 = 0.75	
#define 	OCP_CAP_PRO_60K		2813			//	2813 /6250 = 0.45	
#define 	PWM_DBTIME_60K		144				// 1.2us @120M
#define	RELAY_PWM_60K			4500			// 3900 / 6000 = 0.55
#define	RELAY_M0_Duty_60K		75				// 75%
#define 	CURR_RLYMAX_60K		2880//960		//36000/230*1.414/3*1.2*1.2*0.9=90A ;90A* 32 		//30A * 32 
#define 	BTCURR_LIMIT_60K		1280//32A*32=1024//864//832				// 26 *32       // chuangwei:27*32
#define 	TEMP_OVER_INV_60K		TempP95oC		//  
#define 	TEMP_OVER_BST_60K		TempP95oC		//  
#define 	TEMP_OVER_AMB_60K		TempP90oC		//

#define 	TEMP_DERAT_INV_60K	TempP85oC		//  
#define 	TEMP_DERAT_BST_60K	TempP90oC	
#define 	TEMP_DERAT_AMB_60K	TempP85oC

#define 	DRAT_IVBST_STEP_60K	2000			// 200W
#define 	DRAT_IVBST_BKSTEP_60K	1000			// 100W
#define 	DRAT_AMB_STEP_60K		2000			// 200W
#define 	DRAT_AMB_BKSTEP_60K	1000			// 100W

#define 	BUSSOFT_CNTL_Kp_60K	1025
#define 	BUSSOFT_CNTL_Ki_60K	60

#define 	PVVOLT_CNTL_Kp_60K		3000
#define 	PVVOLT_CNTL_Ki_60K		100//24

#define 	BTCURR_CNTL_Kp_60K		2000
#define 	BTCURR_CNTL_Ki_60K		300//56

#define 	ACTPOWR_CNTL_Kp_60K	816
#define 	ACTPOWR_CNTL_Ki_60K	88

#define 	IVVOLT_CNTL_HiRKd_60K		720
#define 	IVVOLT_CNTL_FstDKd_60K		290//240

#define 	INVCURR_CNTL_Kd_60K	1200    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_60K	173
#define 	INVCURR_CNTL_Kpi_60K	32000   //20210831Revised 64000 To 32000

#define 	INVCURR_CNTL_Kd_60K_PRO	1500    //20210831Revised 2000 To 1200
#define 	INVCURR_CNTL_Kq_60K_PRO	173
#define 	INVCURR_CNTL_Kpi_60K_PRO	14000   //20210831Revised 64000 To 32000

#define 	IVBUSVOLT_CNTL_Kp_H_60K	16000
#define 	IVBUSVOLT_CNTL_Ki_H_60K	32
#define 	IVBUSVOLT_CNTL_Kp_L_60K	24000
#define 	IVBUSVOLT_CNTL_Ki_L_60K	32

#define 	INVCURR_CNTL_KPR_60K		666
#define 	INVCURR_CNTL_KPR1_60K		51
#define 	INVCURR_CNTL_KPR2_60K		972

#define 	TEMP_OVER_INV_60K_PRO	TempP95oC		
#define 	TEMP_OVER_BST_60K_PRO	TempP95oC		  
#define 	TEMP_OVER_AMB_60K_PRO	TempP100oC		

#define 	TEMP_DERAT_INV_60K_PRO	TempP88oC		 
#define 	TEMP_DERAT_BST_60K_PRO	TempP88oC	
#define 	TEMP_DERAT_AMB_60K_PRO	TempP95oC

#define 	OCP_CAP_PRO_60K_PRO		3625			//	4687 /6250 = 0.75	
/*********************XG PV Number & Index define******************************/
#define 	PVA							0
#define 	PVB							1
#define 	PVC							2
#define 	PVD							3

#define     STR0                        0
#define     STR1                        1
#define     STR2                        2
#define     STR3                        3
#define     STR4                        4
#define     STR5                        5
#define     STR6                        6
#define     STR7                        7
#define     STR8                        8
#define     STR9                        9
#define     STR10                       10
#define     STR11                       11

#define 	PV_MAX_NUM				4
//#define 	STR_NUM_MAX				(2*PV_MAX_NUM)
#define     STR_NUM_MAX             (3*PV_MAX_NUM)

/*********************Bus Voltage Max & Min Value define******************************/
#define	BUS_OCP_HALF_VOLT_MAX		5100//5000
#define	BUS_OCP_VOLT_MAX				10000//9800
#define	BUS_HALF_VOLT_MAX			5200	//2250
#define	BUS_VOLT_MAX			10500//10000	//4500
#define	BUS_HALF_VOLT_MIN				1500	//750
#define	BUS_VOLT_MIN					3000	//1500
#define	BUS_HALF_VOLT_RECOVERY		5050//4900	//2100
#define	BUS_HALF_DELTA				1500	// 150V
#define cBus5V 							50
#define cBus12V5 				125
#define cBus30V 					300
#define cBus50V 					500
#define cBus220V 				2200
#define cBus250V 				2500
#define cBus950V 				9500
/****************************Logic 0 & 1   define***********************************/
#define	TRUE	   	 				1
#define	FALSE	    				0

#define	ON    						1
#define	OFF   						0

#define	DISABLE    					0
#define	ENABLE     					1

/****************************Fault Recorve Time***********************************/
#define	CLR_FAULT_TIME				250				// 20ms*250=5000ms=5s
#define SET_FAULT_TIME              250             // 20ms*250=5000ms=5s

#define WAIT_2SEC_BY20MS			100				// 20ms*100 = 2000ms = 2s

/****************************Voltage***********************************/


#define VOLT_100V					1000			// 50V
#define VOLT_120V					1200			// 50V
/********************PV Input Model******************************/
/********************AFCI******************************/
#define CLR_MODEL_NOT   0
#define CLR_MODEL_OnlineOnly    1
#define CLR_MODEL_SelfFault_Online  2
#define CLR_MODEL_Offline   3
#define CLR_MODEL_Auto  4

typedef enum
{
	INDEPENDENT, PARALLEL, DC_SOURCE
}ENUM_MPPT_MODE;


typedef union
{
	Uint64 all;

	struct
	{
		Uint64	FanFault				:1;		// bit1
		Uint64	SPD						:1;		// bit2
		Uint64  STRCurrFault            :1;     // bit3
		Uint64  MeterCommFault          :1;     // bit4
		Uint64	ACOverVolt				:1;	    // bit5
		Uint64	Reserve46_48			:59;	// Warning 6-64
	}bit;
}UN_WARN_BIT;

typedef union
{
	Uint16 all;

	struct
	{
		Uint16	UnderVolt					:1;		// SubFault-01
		Uint16	OverVolt					:1;		// SubFault-02
		Uint16	UnbalanceVolt				:1;		// SubFault-03
		Uint16	Reserve03_16				:13;		//
	}bit;
}UN_BUS_VOLT_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	PanelReserve			:1;		// SubFault-01
    	Uint16	OverVolt				:1;		// SubFault-02
    	Uint16	PanelShort				:1;		// SubFault-03
		Uint16	Reserve03_16			:13;	//
    }bit;
}UN_PV_VOLT_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	InvOverTemp				:1;		// SubFault-01
    	Uint16	BoostOverTemp			:1;		// SubFault-02
    	Uint16	RadiatorOverTemp		:1;		// SubFault-01
    	Uint16	AmOverTemp				:1;		// SubFault-02
		Uint16	Reserve03_16			:12;	//
    }bit;
}UN_TEMP_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
        Uint16  ACVolt              :1;     // SubFault-01
        Uint16	BusVolt				:1;		// SubFault-02
    	Uint16	ISOVolt				:1;		// SubFault-03
    	Uint16  PVVolt              :1;     // SubFault-04
    	Uint16  GFCI                :1;     // SubFault-05
    	Uint16  ACCurr              :1;     // SubFault-06
    	Uint16  BTCurr              :1;     // SubFault-07
		Uint16	Reserve08_16		:9;	//
    }bit;
}UN_SAMPLE_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	UnderVolt1				:1;		// SubFault-01
    	Uint16	OverVolt1				:1;		// SubFault-02
    	Uint16	UtilityLoss				:1;		// SubFault-03
    	Uint16	UnderVolt2				:1;		// SubFault-04
    	Uint16	OverVolt2				:1;		// SubFault-05
    	Uint16	UnderStartVolt			:1;		// SubFault-06
    	Uint16	OverStartVolt			:1;		// SubFault-07
    	Uint16	OverVoltIsr				:1;		// SubFault-08
    	Uint16	IslandIsr				:1;		// SubFault-09
    	Uint16	UnderVoltIsr			:1;		// SubFault-10
    	Uint16	UnderVolt3				:1;		// SubFault-11
    	Uint16	PllFault				:1;		// SubFault-12
		Uint16	Reserve13_16			:4;	//
    }bit;
}UN_GRID_VOLT_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	UnderFreq1				:1;		// SubFault-01
    	Uint16	OverFreq1				:1;		// SubFault-03
    	Uint16	UnderFreq2				:1;		// SubFault-02
    	Uint16	OverFreq2				:1;		// SubFault-04
    	Uint16	UnderStartFreq			:1;		// SubFault-05
    	Uint16	OverStartFreq			:1;		// SubFault-06
    	Uint16	FreqAutoCheckFail		:1;		// SubFault-07
	Uint16	Reserve03_16			:9;	//
    }bit;
}UN_GRID_FREQ_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	INVHardWare				:1;		// SubFault-01
    	Uint16	INVSoftWare				:1;		// SubFault-02
    	Uint16	BTHardWare				:1;		// SubFault-03
    	Uint16	BTSoftWare				:1;		// SubFault-04
    	Uint16	PowerHardWare			:1;		// SubFault-05
    	Uint16	BusOverHardWare		:1;		// SubFault-06
	Uint16	Reserve07_16			:10;	//
    }bit;
}UN_OVER_CURR_FAULT;
typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	RDCISoftWare			:1;		// SubFault-01
    	Uint16	SDCISoftWare			:1;		// SubFault-02
    	Uint16	TDCISoftWare			:1;		// SubFault-03
	Uint16	Reserve04_16			:13;	//
    }bit;
}UN_DCI_OVER_CURR_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	DspRxF107Sci		:1;		// SubFault-01
    	Uint16	F107RxDspSci		:1;		// SubFault-02
    	Uint16	F042Sci				:1;		// SubFault-03
    	Uint16	Reserve4_6			:3;		// SubFault-04
    	Uint16	AFCISci				:1;		// SubFault-07
	Uint16	Reserve8_16		:9;	//
    }bit;
}UN_COMMUNICATE_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	OverRatedCurr		:1;		// SubFault-01	300mA ~ 0.3S
    	Uint16	Jump30mA			:1;		// SubFault-02	30mA ~ 0.3S
    	Uint16	Jump60mA			:1;		// SubFault-03	60mA ~ 0.15S
    	Uint16	Jump150mA			:1;		// SubFault-04	150mA ~ 0.04S
    	Uint16	Reserve04_16		:12;	//
    }bit;
}UN_GFCI_FAULT;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	OverLimitStr1		:1;		// SubFault-01 --SubFault-08
    	Uint16	OverLimitStr2		:1;	
	Uint16	OverLimitStr3		:1;	
	Uint16	OverLimitStr4		:1;	
	Uint16	OverLimitStr5		:1;	
	Uint16	OverLimitStr6		:1;	
	Uint16	OverLimitStr7		:1;	
	Uint16	OverLimitStr8		:1;	

	Uint16	OverLimitStr9		:1;		// SubFault-09--SubFault-016
    	Uint16	OverLimitStr10		:1;	
	Uint16	OverLimitStr11		:1;	
	Uint16	OverLimitStr12		:1;	
	Uint16	OverLimitStr13		:1;	
	Uint16	OverLimitStr14		:1;	
	Uint16	OverLimitStr15		:1;	
	Uint16	OverLimitStr16		:1;	
	
    }bit;
}UN_AFCI_FAULT;

	typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	SelfCheckStr1		:1;	
    	Uint16	SelfCheckStr2		:1;	
	Uint16	SelfCheckStr3		:1;	
    	Uint16	SelfCheckStr4		:1;	
	Uint16	SelfCheckStr5		:1;	
    	Uint16	SelfCheckStr6		:1;
	Uint16	SelfCheckStr7		:1;	
    	Uint16	SelfCheckStr8		:1;	

	 Uint16	SelfCheckStr9		:1;	
    	Uint16	SelfCheckStr10		:1;	
	Uint16	SelfCheckStr11		:1;	
    	Uint16	SelfCheckStr12		:1;	
	Uint16	SelfCheckStr13		:1;	
    	Uint16	SelfCheckStr14		:1;
	Uint16	SelfCheckStr15		:1;	
    	Uint16	SelfCheckStr16		:1;		
    }bit;
}UN_AFCI_SELF_CHECK;
typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	DC_SPD				:1;		// SubWarn-01
    	Uint16	AC_SPD				:1;		// SubWarn-02
		Uint16	Reserve03_16		:14;	//
    }bit;
}UN_SPD_WARN;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	FAN1_INTR				:1;		// SubWarn-01
    	Uint16	FAN2_INTR				:1;		// SubWarn-02
    	Uint16	FAN3_INTR				:1;		// SubWarn-03
    	Uint16	FAN4_EXTR				:1;		// SubWarn-04
		Uint16	Reserve05_16			:12;	//
    }bit;
}UN_FAN_WARN;

typedef union
{
    Uint16 all;

    struct
    {
    	Uint16	ExternalFlash			:1;		// SubFault-01
    	Uint16	EepromWrite				:1;		// SubFault-02
    	Uint16	EepromRead				:1;		// SubFault-03
    	Uint16	GFCIModuleFault			:1;		// SubFault-04
		Uint16	Reserve01_16			:12;	//
    }bit;
}UN_HARDWARE_FAULT;

typedef union
{
    Uint64 all;

    struct
    {
    	Uint64	PVVolt					:1;		// Fault-01
    	Uint64	Reserve2				:1;		// Fault-02
    	Uint64	BusVolt					:1;		// Fault-03
    	Uint64	Reserve4				:1;		// Fault-04
    	Uint64	OverCurr				:1;		// Fault-05
    	Uint64	OverTemp				:1;		// Fault-06
    	Uint64	ISO						:1;		// Fault-07
    	Uint64	Driver					:1;		// Fault-08

    	Uint64	CommunicateFault		:1;		// Fault-09
    	Uint64	GFCIOverCurr			:1;		// Fault-10
    	Uint64	RelayCheck				:1;		// Fault-11
    	Uint64	Reserve12_13			:2;		// Fault 12-13
    	Uint64	DCIOverCurr				:1;		// Fault-14
    	Uint64	Reserve15_16			:2;		// Fault 15-16

    	Uint64	Reserve17_18			:2;		// Fault 17-18
    	Uint64	SampleDiffer			:1;		// Fault-19
    	Uint64	Reserve20_24			:5;		// Fault 20-24

    	Uint64	Reserve25_30			:6;		// Fault 25-30
    	Uint64	GridVolt				:1;		// Fault-31
	Uint64	Reserve32				:1;		// Fault-32

	Uint64	GridFreq				:1;		// Fault-33
	Uint64	Reserve34_36			:3;		// Fault 34-36
	Uint64	TurnOff					:1;		// Fault-37
	Uint64	AFCIFault				:1;		// Fault 38
	Uint64	AFCISelfCheck			:1;		// Fault 39
	Uint64	Reserve40				:1;		// Fault 40

	   	Uint64	AutoTestFail			:1;		// Fault 41
		Uint64	NPE						:1;		// Fault 42
		Uint64	GFCIsensorcheck			:1;		// Fault-43
		Uint64	STRSelfCheck			:1;		// Fault-44
		Uint64	SPS						:1;		// Fault-45  standby power supply
		Uint64	Reserve46_48			:3;		// Fault 46-48

	Uint64	Reserve49_64			:16;	// Fault 49-64
    }bit;
}UN_FAULT_BIT;

typedef struct
{
	Uint16			uwFaultCode;
	Uint16			uwWarnCode;
	Uint16			uwFaultSubCode;
	Uint16			uwWarnSubCode;
	UN_FAULT_BIT	unFaultBit;
	UN_WARN_BIT		unWarnBit;
	UN_HARDWARE_FAULT	unHardwareFault;
/*************************Fault------Subcode*************************/
	UN_PV_VOLT_FAULT	unPVVoltFault;			// Fault-01
	UN_BUS_VOLT_FAULT	unBusVoltFault;			// Fault-03	
	UN_OVER_CURR_FAULT	unOverCurrFault;		// Fault-05
	UN_TEMP_FAULT	unTempFault;				// Fault-06
	UN_COMMUNICATE_FAULT	unCommunicateFault;	// Fault-09
	UN_GFCI_FAULT	unGFCIFault;				// Fault-10	GFCI-Fault
	UN_DCI_OVER_CURR_FAULT  unDCIIFault;				// Fault-14	DCI-Fault
	UN_SAMPLE_FAULT	unSampleFault;				// Fault-19
	UN_GRID_VOLT_FAULT	unGridVoltFault;			// Fault-31
	UN_GRID_FREQ_FAULT	unGridFreqFault;			// Fault-33
	UN_AFCI_FAULT	unAFCIFault;				// Fault-38	AFCI-Fault
	UN_AFCI_SELF_CHECK	unAFCISelfCheck;			// Fault-39	AFCI-selfcheck
	/*************************warning------Subcode*************************/
	UN_FAN_WARN			unFanWarn;				// Warning-01
	UN_SPD_WARN			unSPDWarn;				// Warning-02

}ST_SYS_FAULT_REG;

typedef union
{
	Uint32 all;

	struct
	{
		Uint32  bInvStart			:1;		// bit0
		Uint32  GoToInvOnce       	:1;		// bit1
		Uint32	InvPwmEnable      	:1;		// bit2
		Uint32	InvPwmEnableOld   	:1;		// bit3

		Uint32  MachineCheckEnd     :1;		// bit4
		Uint32  SampleOffsetFinished:1;		// bit5
		Uint32  GFCISelfCheckFinished:1;		// bit6
		Uint32  ISOCheckFinished	:1;		// bit7

		Uint32  TurnOnOffCheck	   	:1;		// bit8
		Uint32  PvVoltLow           :1;		// bit9
		Uint32  PvVoltHigh          :1;		// bit10

		Uint32  rsv          		:21;		// bit11-bit31
	}bit;
}UN_SYS_FLAG;



typedef struct
{
    int16 wBTCurrLimitMax[PV_MAX_NUM];
   	int16 wBTCurrRefLimitMax;
   	int16 wBTOverCurrFaultMax;					// software Boost Peak OCP of More String
    int16 wBTOverCurrFaultAvg;					// software Boost Avg OCP of More String
    int16 wBTOverCurrFaultMax1;                // software Boost Peak OCP of Fewer String
    int16 wBTOverCurrFaultAvg1;                  // software Boost Avg OCP of Fewer String

	Uint16 uwInvPeriod;
	Uint16 uwInvSwithFreq;

	// inv current
	int16 wInvCurrFaultMax;						// software INV OCP
	int16 wInvCurrMaxLimit;						// Id Ref LImit
	int16 wReactiveCurrMaxLimit;					// Iq Ref Limit

	int32 dACVoltSqrtSumFaultMax;
	int32 dACVoltSqrtSumFaultMin;
	int16 wACVoltFaultMax;
	int16 wACVoltFaultMin;
	int32 Rated_CapCurrPeak; 					//Reactive power compensation //mgs/20220907

	// DCI
	Uint16 uwDCILimit100T;

	// PLL
	Uint16 uwPLLStepMax;
	Uint16 uwPLLStepMin;

	Uint16 uwMachineType;
	Uint16 uwPVNumber;
	Uint16 uwMachineMax;
	Uint16 uwMachineMin;

	// Sample Ratio
	int16	wACCurrRatio;
	int16	wDCIRatio;
	int16	wStrCurrRatio;
	int16	wStrCurrBias;						// HardwareBias --- VoltageRise
	int16	wBTCurrRatio1;
	int16	wBTCurrRatio2;
	int16	wGFCIRatio;
	int16	wGFCIBias;							// HardwareBias --- VoltageRise
	int16	wPVVoltRatio;
	int16	wBusVoltRatio;
	int16	wACVoltRatio;
	int16	wISOVoltRatio;
	int16	wNPEVoltRatio;

	ENUM_MPPT_MODE eMpptMode;

	int16	wPVVoltLimitMax; 
	Uint16	uwPWMBoostDutyMax; 
	Uint16	uwPWMBoostPeriod; 

	Uint16	uwFastDCMode;

}ST_SYS_CFG;

typedef struct
{
   
	int16	wHardInvOCPCap;
	int16	wEPWMDeadBand;
	int16	wRelayPwmCap;
	int16	wRelayM0Duty;
	int16	wInvCurrRlyChkMax;	
	int16	wInvTempOver;
	int16	wBoostTempOver;
	int16	wAmbTempOver;
	int16	wInvTempDerate;
	int16	wBoostTempDerate;
	int16	wAmbTempDerate;
	int16	wInvBstDerateStep;
	int16	wInvBstDerateBackStep;
	int16	wAmbDerateStep;
	int16	wAmbDerateBackStep;
	int16	wISOInternalRes;
	int16	wISOInternalRes_N;

	int16	wIntrFan4SetFault;
	int16	wIntrFan4ClrFault;

	int16	wExtrFan123SetFault;
	int16	wExtrFan123ClrFault;
	
}ST_MACHINE_CFG;
typedef struct
{
    Uint16  bCommSet_SVGEn;

    Uint16  bLowPVAndSVGEn;
    Uint16  bEnterSVGStateFlag;
    Uint16  bQuitSVGStateFlag;

    Uint16  SVGSwitchWaitTime;
    Uint16  SVGSwitchWaitTime_Cnt;
    Uint16  SVGSwitchWaitTime_Cnt1;

    Uint16  wCommSet_SVGReactive;
    Uint16  bBTStartPWMEn;

}ST_SVG_PARA;
extern ST_SVG_PARA SvgParam;

extern ST_MACHINE_CFG stMachineCfg;
extern ST_SYS_CFG stSysCfg;


extern ST_SYS_FAULT_REG stSysFaultReg;
extern UN_SYS_FLAG unSysFlag;

extern void VariableInit(void);

#endif /* INCLUDE_CFG_SYSPARA_INIT_H */
