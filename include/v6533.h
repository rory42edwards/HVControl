
//*********************************************************************
//                                                                     
//                ------    C. A. E. N.   S.p.A.    ------             
//                                                                     
//   Name        : V65xx.H		                                       
//   Project     : V65xxController   
//                                                                     
//   Description : Header file         
//                                                                     
//   Date        : July 2012                                          
//   Release     : 1.0                                                 
//   Author      : C.Landi                                             
//                                                                     
//*********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <CAENComm.h>
#include "console.h"


//*****************************************************************
//                                                                      
//   Channel 'status' bit (if asserted) :             
//                                                                      
//   bit  0  ->  On						  ON
//   bit  1  ->  Ramp Up                  RUP                     
//   bit  2  ->  Ramp Down                RDW                     
//   bit  3  ->  Over Current             OVC                     
//   bit  4  ->  Over Voltage             OVV                     
//   bit  5  ->  Under Voltage            UNV                     
//   bit  6  ->  Maxv                     MAXV                    
//   bit  7  ->  MaxI                     MAXI                    
//   bit  8  ->  Trip                     TRIP                    
//   bit  9  ->  Over Power               OVP                     
//   bit 10  ->  Disabled	              DISABLED                
//   bit 11  ->  Interlock                INTLCK                  
//   bit 12  ->  Calibration Error        CAL_CHEKERR
//   bit 13  ->  NC 
//   bit 14  ->  NC 
//   bit 15  ->  NC 
//                                                                      
//*****************************************************************

//*****************************************************************
// Channel data structure
//*****************************************************************

typedef struct hvch
{
uint16_t	Vset ;
uint16_t	Iset ;
uint16_t	SVmax ;
uint16_t	RampUp ;
uint16_t	RampDown ;
uint16_t	TripTime ;
uint16_t	Power ;
uint16_t	PowerDown ;
uint16_t	ImonRange ;
uint16_t	Vmon ;
uint16_t	Imon ;
int16_t		Temp ;
uint16_t	Polarity ;
uint16_t	ImDualRange ;
uint16_t	Status ;
uint16_t	par_changed ;		
} HVCH;

//*****************************************************************
// Board 'Alarm' bit (if asserted) :             
//                                                                      
//   bit  0  ->  Ch0 Alarm						                      
//   bit  1  ->  Ch1 Alarm	                   
//   bit  2  ->  Ch2 Alarm	                     
//   bit  3  ->  Ch3 Alarm	                    
//   bit  4  ->  Ch4 Alarm	                    
//   bit  5  ->  Ch5 Alarm		                     
//   bit  6  ->  NC                                     
//   bit  7  ->  NC                                       
//   bit  8  ->  Board Power FAIL                     
//   bit  9  ->  Board Over Power               
//   bit 10  ->  MaxV Calibration Error            
//   bit 11  ->  MaxI Calibration Error            
//   bit 12  ->  NC 
//   bit 13  ->  NC 
//   bit 14  ->  NC 
//   bit 15  ->  NC 
//
//*****************************************************************

//*****************************************************************
// Board data structure                                
//*****************************************************************

typedef struct brd
{
uint8_t		Model ;
char		Name[8] ;
char		Description [22] ;
uint16_t	NumCh ;
uint16_t	SerNum ;

uint16_t	VRES ;
uint16_t	VSETMAX ;
uint16_t	IRESH ;
uint16_t	ISETMAXH ;
uint16_t	IRESL ;
uint16_t	ISETMAXL ;
uint16_t	RAMPMAX ;

uint16_t	MaxV ;
uint16_t	MaxI ;
uint16_t	Alarm ;						
uint16_t	uCFwRelease ;
uint16_t	FPGAFwRelease ;
uint8_t		par_changed ;
} BRD ;


//*****************************************************************
// #define         
//*****************************************************************

#define	NUMCH					6 

#define	V6519					0 
#define	V6521					1 
#define	V6521H					2 
#define	V6533					3 
#define	V6534					4 

#define	UP						0
#define DOWN					1
#define LEFT					2
#define RIGHT					3

//*****************************************************************
// For all V65XX board    
//*****************************************************************

#define RAMPMIN 				1
#define TRIPMAX  				10000 
#define TRIPRES  				10 

//*****************************************************************
// V6519 500V 3mA/300uA     
//*****************************************************************

#define	V6519_VRES				100				// V Resolution      : 0.01 V
#define V6519_VSETMAX			50000			// 500 * VRES
#define V6519_IRESH				20 				// I High Resolution : 0.05 uA
#define V6519_ISETMAXH 			62000 			// 3100 * IRESH
#define V6519_IRESL				200				// I Low Resolution  : 0.005 uA
#define V6519_ISETMAXL 			6200 			// 310 * IRESH
#define V6519_RAMPMAX	 		100

//*****************************************************************
// V6521 6000V 300uA/30uA     
//*****************************************************************

#define	V6521_VRES				10				// V Resolution      : 0.1 V
#define V6521_VSETMAX			60000			// 6000 * VRES
#define V6521_IRESH				200				// I High Resolution : 0.005 uA
#define V6521_ISETMAXH 			62000 			// 310 * IRESH
#define V6521_IRESL				2000			// I Low Resolution  : 0.0005 uA
#define V6521_ISETMAXL 			6200 			// 31 * IRESH
#define V6521_RAMPMAX	 		500

//*****************************************************************
// V6521H 6000V 20uA/2uA     
//*****************************************************************

#define	V6521H_VRES				10				// V Resolution      : 0.1 V
#define V6521H_VSETMAX			60000			// 6000 * VRES
#define V6521H_IRESH			1000			// I High Resolution : 0.001 uA  
#define V6521H_ISETMAXH 		21000 			// 21 * IRESH
#define V6521H_IRESL			10000			// I Low Resolution  : 0.0001 uA
#define V6521H_ISETMAXL 		2100 			// 2.1 * IRESH
#define V6521H_RAMPMAX	 		500

//*****************************************************************
// V6533 4000V 3mA/300uA     
//*****************************************************************

#define	V6533_VRES				10				// V Resolution      : 0.1 V
#define V6533_VSETMAX			40000			// 4000 * VRES
#define V6533_IRESH				20				// I High Resolution : 0.05 uA
#define V6533_ISETMAXH 			62000 			// 3100 * IRESH
#define V6533_IRESL				200				// I Low Resolution  : 0.005 uA
#define V6533_ISETMAXL 			6200 			// 310 * IRESH
#define V6533_RAMPMAX	 		500

//*****************************************************************
// V6534 6000V 1mA/100uA     
//*****************************************************************

#define	V6534_VRES				10				// V Resolution
#define V6534_VSETMAX			60000			// 6000 * VRES
#define V6534_IRESH				50				// I High Resolution : 0.02 uA
#define V6534_ISETMAXH 			52500 			// 1050 * IRESH
#define V6534_IRESL				500				// I Low Resolution  : 0.002 uA
#define V6534_ISETMAXL 			5250 			// 105 * IRESH
#define V6534_RAMPMAX	 		500

//*****************************************************************
// Channel Status and Board Alarm        
//*****************************************************************

#define   OFF                    0
#define   ON                     1

#define   POS                    1
#define   NEG                    0

#define   H_RANGE                0
#define   L_RANGE                1

#define   IS_ON                  1		
#define   IS_UP                  2
#define   IS_DOWN                4
#define   IS_OVC                 8
#define   IS_OVV              0x10
#define   IS_UNV              0x20
#define   IS_MAXV             0x40
#define   IS_MAXI             0x80
#define   IS_TRIP            0x100
#define   IS_OVP             0x200
#define   IS_OVT             0x400
#define   IS_DISABLED        0x800
#define   IS_INTLCK         0x1000
#define   IS_CAL_CHKERR     0x2000

//*****************************************************************

#define   BD_CH_ALARM            1       
#define   BD_PWR_FAIL        0x100
#define   BD_OVP_FAIL        0x200
#define   BD_MAXV_CHKERR     0x400		
#define   BD_MAXI_CHKERR     0x800		

#define   BD_IMON_DUALRANGE  0x8000		// Dual range imon Enabled/Disabled

//*****************************************************************
// 'define' for parameters updating                                                      
//*****************************************************************

#define   VMON_CHANGED           1      
#define   IMON_CHANGED           2
#define   ST_CHANGED             4
#define   PW_CHANGED             8
#define   VSET_CHANGED        0x10
#define   ISET_CHANGED        0x20
#define   SVMAX_CHANGED       0x40
#define   RUP_CHANGED         0x80
#define   RDWN_CHANGED       0x100
#define   TRIP_CHANGED       0x200
#define   PWDN_CHANGED       0x400
#define   IMR_CHANGED        0x800
#define   TEMP_CHANGED      0x1000

#define   MAXV_CHANGED           1  
#define   MAXI_CHANGED           2  
#define   BDSTAT_CHANGED 		 4

//*****************************************************************
// Function declaration                                                      
//*****************************************************************

CAENComm_ErrorCode ReadChParameters(int32_t handle) ;




