
//*****************************************************************
//                                                                     
//                ------    C. A. E. N.   S.p.A.    ------             
//                                                                     
//   Name        : V65xxController.c                                        
//   Project     : V65xxController        
//                                                                     
//   Description : Controll program for VME V65XX High Voltage boards.		   
//                                                                     
//   Date        : July 2012                                     
//   Release     : 1.0                                                 
//   Author      : C.Landi                                             
//                                                                                                                                          
//*****************************************************************

#include "v6533.h"

//*****************************************************************
// #define.                            
//*****************************************************************

#define   NORM		0
#define   BARR		1

#define   VMFIELD   0
#define   ISFIELD   6
#define   IMFIELD   1
#define   TMFIELD   2
#define   STFIELD   3
#define   PWFIELD   4
#define   VSFIELD   5
#define   SVFIELD   7
#define   RUFIELD   8
#define   RDFIELD   9
#define   TRFIELD   10
#define   PDFIELD   11
#define   IRFIELD   12


//*****************************************************************
// Global declaration.                            
//*****************************************************************

HVCH		Channel[NUMCH] ;
BRD			Board ;

//*****************************************************************

//*****************************************************************
// Name        : Getstr.                                                
//                                                                      
// Description : Get string procedure.  
//				 Only numbers and '.' are accepted.
//                                                                      
// Input	   : len	 ->  max string length                   
//				 f_char  ->  0 : normal, 1 : first char entered 	
// Output      : Numeric value 
//				 -1		 ->  no valid value entered
// Global      : None.    
// Release     : 1.0                                                    
//                                                                      
//*****************************************************************

float GetValue(uint8_t len, int8_t f_char)
{
int32_t	i,j,point_pos=-1 ;
int8_t	buff[20],c;
float	fval=0 ;

#define   LF        0xa
#define   BS        0x8
#define   DEL       0x7f

for (i=0; i<20; i++)
	buff[i] = 0 ;

if (f_char)
	{
	buff[0] = f_char ;
	con_printf("%c",f_char) ;
	if (f_char == '.')
		point_pos = 0 ;
	i = 1 ;
	}
else
	i= 0 ;

for (;;)
	{
	c = con_getch() ;
	
	switch (c)
		{
		case LF  :
		case CR  :	buff[i] = 0 ;                 
					if (i == 0)
						return -1 ;

					if (point_pos >= 0)
						{						
						for (j=0; j<point_pos; j++)
							fval = fval * 10 + (float)(buff[j] - '0') ;
						for (j=point_pos+1; j<i; j++)
							fval = fval * 10 + (float)(buff[j] - '0') ;
						for (j=1; j<i-point_pos; j++)
							fval = fval / 10 ;
						}
					else
						for (j=0; j<i; j++)
							fval = fval* 10 +(float)(buff[j] - '0') ;

					return fval ;

		case BS  :
		case DEL :	if (i)
						{
						con_printf ("%c%c%c",BS,BLANK,BS) ;
						i-- ;
						buff[i] = 0 ;
						}
					break ;

		case '0' :
		case '1' :
		case '2' :
		case '3' :
		case '4' :
		case '5' :
		case '6' :
		case '7' :
		case '8' :
		case '9' :
		case '.' :	if (i < len)
						{
						buff[i] = c ;                 
						con_printf("%c",c) ;
						if (c == '.')
							point_pos = i ;
						i++ ;
						}
					break ;

		default	 :	break ;
    }
  }

return fval ;
}


//*****************************************************************
// Name        : DispHeader                                            
//                                                                 
// Description : Displays the CAEN header.
//				                                                  
// Input       : None.                                         
// Output      : None.                                         
// Global      : Board.                                         
// Release     : 1.0.                                             
//                                                                
//*****************************************************************

void DispHeader(void)
{
con_printf (" C.A.E.N.  %s  %s   uC Rel %u.%02u  FPGA Rel %u.%u  SN %u",
		Board.Name,
		Board.Description,
		Board.uCFwRelease >> 8, Board.uCFwRelease & 0xff,
		Board.FPGAFwRelease >> 8, Board.FPGAFwRelease & 0xff,
		Board.SerNum ) ;
}


//*****************************************************************
// Name        : Welcome                                            
//                                                                 
// Description : Displays the main menu.
//				                                                  
// Input       : None.                                         
// Output      : None.                                         
// Global      : None.                                         
// Release     : 1.0.                                             
//                                                                
//*****************************************************************

static void Welcome(void)
{
int32_t i ;	

char V65XXWEL[5][60] =
	{
	" ##   ##     #####     #####     ##   ##    ##   ## " ,
	" ##   ##    ##         ##         ## ##      ## ##  " ,
	" ##   ##    ######     #####       ###        ###   " ,
	"  ## ##     ##   ##        ##     ## ##      ## ##  " ,
	"   ###       #####     #####     ##   ##    ##   ## " ,
	} ;

char V65XXMEN[2][65] =
	{
	" C : Channel           Display/Modify channels ",
	" B : Board             Display Board Status    ",
	} ;
	
clrscr();
for (i=0; i<5; i++)  
	con_printf ("%s\n",V65XXWEL[i]) ;
con_printf ("\n\n") ;
DispHeader() ;

con_printf ("\n\n B O A R D   M E N U \n\n") ;

for (i=0; i<2; i++)
	con_printf ("%s\n",V65XXMEN[i]) ;

con_printf ("\n\n Q : Quit") ;
con_printf_xy (1,23," Select Item  ") ;
}


//*****************************************************************
// Name        : DispField                                             
//                                                                     
// Description : Displays the channels fields.      
//                                                                     
// Input       : ch     -> (0..LASTCH-1)                 
//               field  -> (view XXFIELD)                
//               n_b    -> NORM : normal mode                  
//                      -> BARR : highligth mode             
//               dato   -> 1	: display field value               
//                         0	: display 'blank'       
// Output      : None.                                             
// Global      : Channel[],Board.                                            
// Release     : 1.0                                                   
//*****************************************************************

void DispField(int32_t ch, int32_t field, int32_t n_b, int32_t dato)
{
int8_t		*st_disp,*pw_disp,*rdk_disp,*imr_disp ;
uint16_t	status,trip ;
int32_t		x_pos,y_pos ;
HVCH		*chan ;

const int8_t  PON[]  = "On  " ;
const int8_t  POFF[] = "Off " ;
const int8_t  LOW[]  = "Low " ;
const int8_t  HIGH[] = "High" ;
const int8_t  RUP[]  = "Up   " ;
const int8_t  RDWN[] = "Down " ;
const int8_t  OVC[]  = "OvC  " ;
const int8_t  OVV[]  = "OvV  " ;
const int8_t  UNV[]  = "UnV  " ;
const int8_t  MXV[]  = "MxV  " ;
const int8_t  MXI[]  = "MxI  " ;
const int8_t  TRP[]  = "Trip " ;
const int8_t  OVP[]  = "OvP  " ;
const int8_t  OVT[]  = "OvT  " ;
const int8_t  RAMP[] = "Ramp " ;
const int8_t  ILK[]  = "Intlk" ;
const int8_t  KILL[] = "Kill " ;
const int8_t  DIS[]  = "Dis  " ;
const int8_t  NCL[]  = "NoCal" ;
const int8_t  PWF[]  = "PwF  " ;
const int8_t  NST[]  = "     " ;

chan = &Channel[ch] ;

st_disp = (int8_t *)NST ;

if (chan -> Power)
	pw_disp = (int8_t *)PON ;
else
	pw_disp = (int8_t *)POFF ;

if (chan -> PowerDown)
	rdk_disp = (int8_t *)RAMP ;
else
	rdk_disp = (int8_t *)KILL ;

if (chan -> ImonRange == H_RANGE)
	imr_disp = (int8_t *)HIGH ;
else
	imr_disp = (int8_t *)LOW ;

status = chan -> Status ;

if (status & IS_CAL_CHKERR)
	st_disp = (int8_t *)NCL ;
else
	{
	if (status & IS_ON)
		{
		st_disp = (int8_t *)NST ;

		if (status & IS_TRIP)
			st_disp = (int8_t *)TRP ;
		else
			if (status & IS_OVC)
				st_disp = (int8_t *)OVC ;
			else
				{
				if (status & IS_MAXV)
					st_disp = (int8_t *)MXV ;
		        else
					if (status & IS_MAXI)
						st_disp = (int8_t *)MXI ;
					else
						{
						if (status & IS_UNV)
							st_disp = (int8_t *)UNV ;
						if (status & IS_OVV)
							st_disp = (int8_t *)OVV ;
						if (status & IS_UP)
							st_disp = (int8_t *)RUP ;
						if (status & IS_DOWN)
							st_disp = (int8_t *)RDWN ;
						}
				}
		}
	else
		{
		st_disp = (int8_t*)NST ;
		if (status & IS_INTLCK)
			st_disp = (int8_t *)ILK ;
		else
			if (status & IS_DISABLED)
				st_disp = (int8_t *)DIS ;
			else
				if (status & IS_TRIP)
					st_disp = (int8_t *)TRP ;
				else
					if ((status & IS_OVP) || (Board.Alarm & BD_OVP_FAIL))
						st_disp = (int8_t *)OVP ;
					else
						if (status & IS_OVT)
							st_disp = (int8_t *)OVT ;
						else
							if (Board.Alarm & BD_PWR_FAIL)
	 							st_disp = (int8_t *)PWF ;
		}
	}

x_pos = 14 + (ch * 11) ;

y_pos = 6 + field ;
if (field > STFIELD)
	y_pos++ ;

gotoxy(x_pos, y_pos) ; 

switch (field)
  {                                                             
  case  VMFIELD :	if (Board.Model == V6519)				// Vmon 
						con_printf ("%06.2f",(float)chan -> Vmon/(float)Board.VRES) ;
					else	
						con_printf ("%06.1f",(float)chan -> Vmon/(float)Board.VRES) ;
					break ;

  case  IMFIELD :	if (Board.Model != V6521)				// Imon 
						{
  						if (chan -> ImonRange == H_RANGE)             
							con_printf ("%07.2f",(float)chan -> Imon/(float)Board.IRESH) ;
						else
							con_printf ("%07.3f",(float)chan -> Imon/(float)Board.IRESL) ;
						}
					else
						{
  						if (chan -> ImonRange == H_RANGE)             
							con_printf ("%07.3f",(float)chan -> Imon/(float)Board.IRESH) ;
						else
							con_printf ("%07.4f",(float)chan -> Imon/(float)Board.IRESL) ;
						}
					break ;

  case  TMFIELD :	con_printf ("%+04d",chan -> Temp) ;
					break ;

  case  STFIELD :	con_printf ("%s",st_disp) ;				// Status 
					break ;

  case  PWFIELD :	con_printf ("%s",pw_disp) ;             // Ch On/Off
					break ;

  case  VSFIELD :	if (n_b == BARR)                        // Vset 
						{
						con_printf ("      ") ;
						gotoxy(x_pos, y_pos) ; 
						}
					if (dato)
						{
						if (Board.Model == V6519)
							con_printf ("%06.2f",(float)chan -> Vset/(float)Board.VRES) ;
						else
							con_printf ("%06.1f",(float)chan -> Vset/(float)Board.VRES) ;
						}
	                break ;

  case  ISFIELD :	if (n_b == BARR)						// Iset 
						{
						con_printf ("       ") ;
						gotoxy(x_pos, y_pos) ; 
						}
					if (dato)
						{
						if (Board.Model != V6521)			
							con_printf ("%07.2f",(float)chan -> Iset/(float)Board.IRESH) ;
						else
							con_printf ("%07.3f",(float)chan -> Iset/(float)Board.IRESH) ;
						}
	                break ;

  case  SVFIELD :	if (n_b == BARR)                        // SVmax
						{
						con_printf ("      ") ;
						gotoxy(x_pos, y_pos) ; 
						}
					if (dato)
						{
						if (Board.Model == V6519)
							con_printf ("%06.2f",(float)chan -> SVmax/(float)Board.VRES) ;
						else
							con_printf ("%06.1f",(float)chan -> SVmax/(float)Board.VRES) ;
						}
	                break ;

  case  RUFIELD :											// Ramp Up
  case  RDFIELD :	if (n_b == BARR)						// Ramp Down
						{
						if (Board.Model == V6519)
							con_printf ("  ") ;
						else
							con_printf ("   ") ;
						gotoxy(x_pos, y_pos) ;
						}
                  
					if (dato)
	                    {
						if (Board.Model == V6519)
							{
							if (field == RUFIELD)
								con_printf ("%02d",chan -> RampUp) ;
							else
								con_printf ("%02d",chan -> RampDown) ;
							}
						else
							{
							if (field == RUFIELD)
								con_printf ("%03d",chan -> RampUp) ;
							else
								con_printf ("%03d",chan -> RampDown) ;
							}
						}
					break ;

  case  TRFIELD :	if (n_b == BARR)						// Trip
						{
						con_printf ("      ") ;
						gotoxy(x_pos, y_pos) ;
						}
                  
					if (dato)
						{
						trip = chan -> TripTime ;
						if (trip == TRIPMAX)
							con_printf ("Inf. ") ;
						else
							con_printf ("%05.1f",(float)trip/(float)TRIPRES) ;
						}
					break ;

  case  PDFIELD :	con_printf ("%s",rdk_disp) ;			// Power Down ramp/kill 
	                break ;

  case  IRFIELD :	con_printf ("%s",imr_disp) ;			// Imon Range 
	                break ;

  default       :	break ;
  }

}

//*****************************************************************
// Name        : SetChMenu.	                                           
//                                                                     
// Description : Handles the command set for the single channel
//               'ch' o for alla channels ('group' = 1).
//                                                                     
// Input       : handle    
//				 ch    ->  (0..LASTCH-1)                   
//               field ->  (view XXFIELD)                 
//               comm  ->  set command                              
//               group ->  group mode                              
// Global      : channel[].                                            
//                                                                     
//*****************************************************************

static CAENComm_ErrorCode SetChMenu(int32_t handle, uint8_t ch, int8_t field, int8_t comm, int32_t group)
{
uint16_t	val,ISETMAX ;
uint8_t		ch_mask ;
int32_t		i ;
float		fval=0 ;
HVCH		*chan ;

if (!group)
	ch_mask = 1 << ch ;
else
	ch_mask = (1 << Board.NumCh) - 1 ;
chan = &Channel[0] ;

switch (field)
	{		
	case VSFIELD	:	if (comm == ' ')
							break ;
						DispField(ch,field,BARR,0) ;
						fval = GetValue(6,comm) ; 
					
						if (fval == -1)
							{
							DispField(ch,field,BARR,1) ;
							break ;
							}

						fval = (fval * (float)Board.VRES) + (float)0.5 ;		// Rounding to the resolution 

						if (fval > (float)Board.VSETMAX)
							val = Board.VSETMAX ;
						else
							val = (uint)fval ;
				
						if (val > chan -> SVmax)
							val = chan -> SVmax ;						
						
						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> Vset = val ;
								CAENComm_Write16(handle, 0x80+(128*i), val) ;	// Write Vset
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}

						break ;

	case ISFIELD	:	if (comm == ' ')
							break ;
						DispField(ch,field,BARR,0) ;
						fval = GetValue(7,comm) ; 
					
						if (fval == -1)
							{
							DispField(ch,field,BARR,1) ;
							break ;
							}
						
						fval = (fval * (float)Board.IRESH) + (float)0.5 ;	

						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{	
								if (chan -> ImonRange == H_RANGE)
									ISETMAX = Board.ISETMAXH ;
								else
									ISETMAX = Board.ISETMAXL ;

								if (fval > ISETMAX)
									val = ISETMAX ;
								else
									val = (uint)fval ;

								chan -> Iset = val ;						
								CAENComm_Write16(handle, 0x84+(128*i), val) ;	// Write Iset
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}

						break ;

	case PWFIELD	:	if (comm != ' ')
							break ;

						val = Channel[ch].Power ^ 1 ;
						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> Power = val ;
								CAENComm_Write16(handle, 0x90+(128*i), val) ;	// ON/OFF Ch
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}

						break ;

	case TRFIELD	:	if (comm == ' ')
							break ;
						DispField(ch,field,BARR,0) ;
						fval = GetValue(6,comm) ; 
					
						if (fval == -1)
							{
							DispField(ch,field,BARR,1) ;
							break ;
							}

						fval = (fval * (float)TRIPRES) + (float)0.5 ;		

						if (fval > (float)TRIPMAX)
							val = TRIPMAX ;
						else
							val = (uint)fval ;

						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> TripTime = val ;
								CAENComm_Write16(handle, 0x98+(128*i), val) ;	// Write Trip
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}

						break ;

	case SVFIELD	:	if (comm == ' ')
							break ;
						DispField(ch,field,BARR,0) ;
						fval = GetValue(6,comm) ; 
					
						if (fval == -1)
							{
							DispField(ch,field,BARR,1) ;
							break ;
							}

						fval = (fval * (float)Board.VRES) + (float)0.5 ;   

						if (fval > (float)Board.VSETMAX)
							val = Board.VSETMAX ;
						else
							val = (uint)fval ;

						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> SVmax = val ;
								CAENComm_Write16(handle, 0x9c+(128*i), val) ;	// Write SVmax
								DispField(i,field,NORM,1) ;

								if (val < chan -> Vset)
									{
									chan -> Vset = val ;
									CAENComm_Write16(handle, 0x80+(128*i), val) ;	// Write Vset
									DispField(i,VSFIELD,NORM,1) ;
									}
								}
							chan++ ;
							}
						
						break ;

	case RUFIELD	:
	case RDFIELD	:	if (comm == ' ')
							break ;
						DispField(ch,field,BARR,0) ;
						fval = GetValue(3,comm) ; 
					
						if (fval == -1)
							{
							DispField(ch,field,BARR,1) ;
							break ;
							}

						fval += (float)0.5 ;   
						if (fval > (float)Board.RAMPMAX)
							val = Board.RAMPMAX ;
						else
							if (fval < (float)RAMPMIN)
								val = RAMPMIN ;
							else
								val = (uint16_t)fval ;

						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								if (field == RDFIELD)
									{
									chan -> RampDown = val ;
									CAENComm_Write16(handle, 0xA0+(128*i), val) ;	// Write RampDown
									}
								else
									{
									chan -> RampUp = val ;
									CAENComm_Write16(handle, 0xA4+(128*i), val) ;	// Write RampUp
									}
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}
					
						break ;

	case PDFIELD	:	if (comm != ' ')
							break ;

						val = Channel[ch].PowerDown ^ 1 ;
						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> PowerDown = val ;
								CAENComm_Write16(handle, 0xa8+(128*i), val) ;	// Write Power Down
								DispField(i,field,NORM,1) ;
								}
							chan++ ;
							}

						break ;

	case IRFIELD	:	if ((comm != ' ') || !(Board.Alarm & BD_IMON_DUALRANGE)) 
							break ;

						val = Channel[ch].ImonRange ^ 1 ;

						for (i=0; i<Board.NumCh; i++)
							{
							if (ch_mask & (1 << i))
								{
								chan -> ImonRange = val ;
								CAENComm_Write16(handle, 0xb4+(128*i), val) ;	// Write Imon Range
								DispField(i,IMFIELD,NORM,1) ;
								DispField(i,field,NORM,1) ;
								
								if ((chan -> ImonRange == L_RANGE) && (chan -> Iset > Board.ISETMAXL))
									{											
									chan -> Iset = Board.ISETMAXL ;
									DispField(i,ISFIELD,NORM,1) ;
									}
								}
							chan++ ;
							}

						break ;

	default			:	break ;
	}

return CAENComm_Success ;
}


//*****************************************************************
// Name        : DisplayBoard	                                           
//                                                                    
// Description : Display the board parameters.   
//                                                                     
// Input       : handle    
// Output      : CAENCommErr.                                              
// Global      : Board.                                      
// Release     : 1.0.                                                  
//                                                                     
//*****************************************************************

static CAENComm_ErrorCode DisplayBoard(int32_t handle)
{
CAENComm_ErrorCode ret ;
int32_t		ch ;
uint16_t	bd_maxv=0,bd_maxi=0,bd_alarm=0 ;
int8_t		key ;

#define X_POS		16
#define Y_MAXV		5
#define Y_MAXI		6
#define Y_PWST		11
#define Y_OVPW		12
#define Y_CHAL		14

clrscr() ;
DispHeader() ;
con_printf ("\n\n") ;

con_printf (" BOARD STATUS \n\n") ;

con_printf (" Vmax          %04u V      ",Board.MaxV) ;
if (Board.Alarm & BD_MAXV_CHKERR)
	con_printf ("[ Calibration ERR ]\n") ;
else
	con_printf ("[ Calibration OK  ]\n") ;

con_printf (" Imax          %04u uA     ",Board.MaxI) ;
if (Board.Alarm & BD_MAXI_CHKERR)
	con_printf ("[ Calibration ERR ]\n") ;
else
	con_printf ("[ Calibration OK  ]\n") ;

con_printf ("\n\n Alarm Status\n\n") ;

if (Board.Alarm & BD_PWR_FAIL)
	con_printf (" Power         FAIL\n") ;
else
	con_printf (" Power         OK  \n") ;

if (Board.Alarm & BD_OVP_FAIL)
	con_printf (" Over Power    YES\n\n") ;
else
	con_printf (" Over Power    NO \n\n") ;

for (ch=0; ch<Board.NumCh; ch++)
	{
	if (Board.Alarm & (1 << ch)) 
		con_printf (" Ch%d           ALARM\n",ch) ;
	else
		con_printf (" Ch%d           OK   \n",ch) ;
	}

con_printf_xy (1,22," Press any key to exit ... ") ;		

do
	{
	gotoxy(1,22) ;

	ret = CAENComm_Read16(handle, 0x50, &Board.MaxV) ;
	if (ret != CAENComm_Success)
		return ret ;
	
	if (bd_maxv != Board.MaxV)
		{
		bd_maxv = Board.MaxV ;
		con_printf_xy(X_POS,Y_MAXV,"%04d",bd_maxv) ;
		}

	ret = CAENComm_Read16(handle, 0x54, &Board.MaxI) ;
	if (ret != CAENComm_Success)
		return ret ;

	if (bd_maxi != Board.MaxI)
		{
		bd_maxi = Board.MaxI ;
		con_printf_xy(X_POS,Y_MAXI,"%04d",bd_maxi) ;
		}

	ret = CAENComm_Read16(handle, 0x58, &Board.Alarm) ;
	if (ret != CAENComm_Success)
		return ret ;

	if (bd_alarm != Board.Alarm)
		{
		bd_alarm = Board.Alarm ;

		if (bd_alarm & BD_PWR_FAIL)
			con_printf_xy (X_POS,Y_PWST,"FAIL") ;
		else
			con_printf_xy (X_POS,Y_PWST,"OK  ") ;

		if (bd_alarm & BD_OVP_FAIL)
			con_printf_xy (X_POS,Y_OVPW,"YES") ;
		else
			con_printf_xy (X_POS,Y_OVPW,"NO ") ;

		for (ch=0; ch<Board.NumCh; ch++)
			{
			if (bd_alarm & (1 << ch)) 
				con_printf_xy (X_POS,(Y_CHAL+ch),"ALARM") ;
			else
				con_printf_xy (X_POS,(Y_CHAL+ch),"OK   ") ;
			}
		}
		
	key = con_kbhit() ;
	}
while (!key) ;

return CAENComm_Success ;
}


//*****************************************************************
// Name        : DisplayModifyChannels	                                           
//                                                                    
// Description : Handles the display and the change of channels 
//				 parameters.        
//                                                                     
// Input       : handle    
// Output      : CAENCommErr.                                              
// Global      : Channel[],Board.                                      
// Release     : 1.0.                                                  
//                                                                     
//*****************************************************************

static CAENComm_ErrorCode DisplayModifyChannels(int32_t handle)
{
uint8_t		key,ch_sel=0 ;
uint16_t	ivalue ;
uint16_t	uvalue,bd_alarm=0  ;
int32_t		ch,fine,x_pos,y_pos,X_END,field,group_mode=0 ;
HVCH		*chan ;
uint16_t	V_mon[NUMCH] ;	
uint16_t	I_mon[NUMCH] ;
int16_t		T_mon[NUMCH] ;
uint16_t	Ch_status[NUMCH] ;
uint16_t	Pw_status[NUMCH] ;	

#define X_START		12
#define Y_START		11
#define Y_END		19 
#define X_GMODE		60
#define Y_GMODE		23

X_END = X_START + (Board.NumCh-1) * 11 ;

ReadChParameters(handle) ;		

do
	{
	fine = 0 ;
	clrscr() ;
	DispHeader() ;
	con_printf ("\n\n") ;

	con_printf ("      ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		con_printf ("        Ch%d",ch) ;

	con_printf ("\n\n Pol  ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		{
		if (Channel[ch].Polarity == POS)
			con_printf ("        Pos") ;
		else
			con_printf ("        Neg") ;
		}

	con_printf ("\n Vmon  V  ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		{
		V_mon[ch] = 0 ;
		DispField(ch,VMFIELD,NORM,1) ;
		}
	con_printf ("\n Imon  uA ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		{
		I_mon[ch] = 0 ;
		DispField(ch,IMFIELD,NORM,1) ;
		}
	con_printf ("\n Temp  °C ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		{
		T_mon[ch] = 0 ;
		DispField(ch,TMFIELD,NORM,1) ;
		}
	con_printf ("\n Status   ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		{
		Ch_status[ch] = 0 ;
		DispField(ch,STFIELD,NORM,1) ;
		}
	con_printf ("\n") ;

	con_printf ("\n Pw       ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,PWFIELD,NORM,1) ;
	con_printf ("\n Vset  V  ") ;	
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,VSFIELD,NORM,1) ;
	con_printf ("\n Iset  uA ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,ISFIELD,NORM,1) ;
	con_printf ("\n Svmax V  ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,SVFIELD,NORM,1) ;
	con_printf ("\n RUp   V/S") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,RUFIELD,NORM,1) ;
	con_printf ("\n RDwn  V/S") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,RDFIELD,NORM,1) ;
	con_printf ("\n Trip  S  ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,TRFIELD,NORM,1) ;
	con_printf ("\n PwDwn    ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,PDFIELD,NORM,1) ;
	con_printf ("\n ImRng    ") ;
	for (ch=0; ch<Board.NumCh; ch++)
		DispField(ch,IRFIELD,NORM,1) ;

	x_pos = X_START ;
	y_pos = Y_START ;
	ch_sel = 0 ;

	if (group_mode)
		{
		con_printf_xy(X_GMODE,Y_GMODE,"[Group Mode]") ;
		con_printf_xy (x_pos,y_pos,"=>") ;
		}
	else
		{
		con_printf_xy(X_GMODE,Y_GMODE,"            ") ;
		con_printf_xy (x_pos,y_pos,"->") ;
		}

	con_printf_xy (1,23," G : Group Mode    H : Help     Q : Quit ") ;

	do
		{
		do
			{
			gotoxy(1,23) ;

			chan = &Channel[0] ;
			for (ch=0; ch<Board.NumCh; ch++)
				{
				CAENComm_Read16(handle, 0x88+(128*ch), &uvalue) ;		// Read Vmon
				if (uvalue != V_mon[ch])
					{
					V_mon[ch] = uvalue ;
					chan -> Vmon = uvalue ;
					DispField(ch,VMFIELD,NORM,1) ;
					}

				if (chan -> ImonRange == H_RANGE)
					CAENComm_Read16(handle, 0x8c+(128*ch), &uvalue) ;	// Read ImonH
				else
					CAENComm_Read16(handle, 0xb8+(128*ch), &uvalue) ;	// Read ImonL
				if (uvalue != I_mon[ch])
					{
					I_mon[ch] = uvalue ;
					chan -> Imon = uvalue ;
					DispField(ch,IMFIELD,NORM,1) ;
					}
	
				CAENComm_Read16(handle, 0xb0+(128*ch), &ivalue) ;		// Read temp
				if (ivalue != T_mon[ch])
					{
					T_mon[ch] = ivalue ;
					chan -> Temp = ivalue ;
					DispField(ch,TMFIELD,NORM,1) ;
					}

				CAENComm_Read16(handle, 0x94+(128*ch), &uvalue) ;		// Read Ch Status
				if (uvalue != Ch_status[ch])
					{
					Ch_status[ch] = uvalue ;
					chan -> Status = uvalue ;
					DispField(ch,STFIELD,NORM,1) ;
					}

				CAENComm_Read16(handle, 0x90+(128*ch), &uvalue) ;		// Read Power
				if (uvalue != Pw_status[ch])
					{
					Pw_status[ch] = uvalue ;
					chan -> Power = uvalue ;
					DispField(ch,PWFIELD,NORM,1) ;
					}

				chan++ ;
				}

			CAENComm_Read16(handle, 0x58, &Board.Alarm) ;
			if (bd_alarm != Board.Alarm)
				{
				bd_alarm = Board.Alarm ;
				for (ch=0; ch<Board.NumCh; ch++)
					DispField(ch,STFIELD,NORM,1) ;
				}

			}
		while (!(key = con_kbhit())) ;

		//if (key == 0xe0)				// Arrows keys 
			//{
			key = con_getch() ;
			if (key == 'w')
				key = UP ;
			if (key == 's')
				key = DOWN ;
			if (key == 'a')
				key = LEFT ;
			if (key == 'd')
				key = RIGHT ;
			//}
		else
			key = toupper(key) ;

		switch (key)
			{
			case UP		:	
			case DOWN	:
			case LEFT	:
			case RIGHT	:	con_printf_xy(x_pos,y_pos,"  ") ;
						
							if (key == UP)
								{
								if (y_pos == Y_START)
									y_pos = Y_END ;
								else
									y_pos-- ;
								}

							if (key == DOWN)
								{
								if (y_pos == Y_END)
									y_pos = Y_START ;
								else
									y_pos++ ;
								}

							if (key == LEFT)
								{
								if (x_pos == X_START)
									{
									x_pos = X_END ;
									ch_sel = Board.NumCh-1 ;
									}
								else
									{
									x_pos -= 11 ;
									ch_sel-- ;
									}
								}

							if (key == RIGHT)
								{
								if (x_pos == X_END)
									{
									x_pos = X_START ;
									ch_sel = 0 ;
									}
								else
									{
									x_pos += 11 ;
									ch_sel++ ;
									}
								}

							if (!group_mode)
								con_printf_xy(x_pos,y_pos,"->") ;
							else
								con_printf_xy(x_pos,y_pos,"=>") ;

							break ;

			case '0'	:
			case '1'	:
			case '2'	:
			case '3'	:
			case '4'	:
			case '5'	:
			case '6'	:
			case '7'	:
			case '8'	:
			case '9'	:
			case '.'	:
			case ' '	:	field = PWFIELD + (y_pos - Y_START) ;
							SetChMenu(handle, ch_sel, field, key, group_mode) ;
							break ;

			case 'G'	:	group_mode ^= 1 ;
							if (group_mode)
								{
								con_printf_xy(X_GMODE,Y_GMODE,"[Group Mode]") ;
								con_printf_xy(x_pos,y_pos,"=>") ;
								}
							else
								{
								con_printf_xy(X_GMODE,Y_GMODE,"            ") ;
								con_printf_xy(x_pos,y_pos,"->") ;
								}

							break ;

			case 'H'	:	clrscr() ;
							DispHeader() ;
							con_printf ("\n\n Keyboard Usage\n\n") ;
							con_printf (" . Use arrow keys to select the field.\n\n") ;
							con_printf (" . For numeric fields directly type the value and then enter.\n\n") ;
							con_printf (" . For 'Pw','PwDwn','ImRng' fields use 'space barr' key to toggle the value.\n\n") ;
							con_printf ("\n Press any key ...") ;
							con_getch() ;
							fine = 1 ;
							break ;

			case 'Q'	:	fine = 2 ;
							break ;

			default		:	break ;
			}
		}
	while (!fine) ;
	}
while (fine != 2) ;

return CAENComm_Success ;
}


//*****************************************************************
// Name        : ReadChParameters                                         
//                                                                 
// Description : Read the parameters values of all channels.
//				 The address range for each channel is :
//				                                                  
//				 Ch0 ->	0x0080 - 0x00B9 
//				 Ch1 ->	0x0100 - 0x0139 
//				 Ch2 ->	0x0180 - 0x01B9 
//				 Ch3 ->	0x0200 - 0x0239 
//				 Ch4 ->	0x0280 - 0x02B9 
//				 Ch5 ->	0x0300 - 0x0339 
//				                                                  
// Input       : handle                                         
// Output      : CAENComm Error.                                         
// Global      : Channel[].                                         
// Release     : 1.0.                                             
//                                                                
//*****************************************************************

CAENComm_ErrorCode ReadChParameters(int handle)
{
CAENComm_ErrorCode ret ;
uint16_t	dato ;
int32_t		ch ;
HVCH		*chan ;

chan = &Channel[0] ;
for (ch=0; ch<Board.NumCh; ch++)
	{
	ret = CAENComm_Read16(handle, 0x80+(128*ch), &dato) ;	// Vset
	if (ret == CAENComm_Success)
		chan -> Vset = dato ;
	else
		return ret ;

	CAENComm_Read16(handle, 0x84+(128*ch), &dato) ;			// Iset
	chan -> Iset = dato ;

	CAENComm_Read16(handle, 0x98+(128*ch), &dato) ;			// Trip
	chan -> TripTime = dato ;

	CAENComm_Read16(handle, 0x9c+(128*ch), &dato) ;			// Svmax
	chan -> SVmax = dato ;

	CAENComm_Read16(handle, 0xa0+(128*ch), &dato) ;			// RampDown
	chan -> RampDown = dato ;

	CAENComm_Read16(handle, 0xa4+(128*ch), &dato) ;			// RampUp
	chan -> RampUp = dato ;

	CAENComm_Read16(handle, 0xa8+(128*ch), &dato) ;			// Power Down mode
	chan -> PowerDown = dato ;

	CAENComm_Read16(handle, 0xac+(128*ch), &dato) ;			// Polarity
	chan -> Polarity = dato ;

	CAENComm_Read16(handle, 0xb4+(128*ch), &dato) ;			// Imon Range
	chan -> ImonRange = dato ;

	chan++ ;
	}

return CAENComm_Success ;
}


//*****************************************************************
// Name        : ReadParametersConfiguration                                            
//                                                                 
// Description : Reads all board and channels parameters.
//				  1. Board Channel Number	(VME addr 0x8100)
//				  2. Board Description		(VME addr 0x8102-0x8114)
//				  3. Board Model			(VME addr 0x8116-0x811c)
//				  4. Board Serial Number	(VME addr 0x811e)
//				  5. FPGA Firmware Release	(VME addr 0x8120)
//				  6. Board Vmax				(VME addr 0x50)
//				  7. Board Imax				(VME addr 0x54)
//				  8. Board Alarm			(VME addr 0x58)
//				  9. uC Firmware Release	(VME addr 0x5C)
//				 10. Channels parameters
//				 11. Board parameters
//
// Input       : handle                                         
// Output      : CAENComm Error.                                         
// Global      : Board.                                         
// Release     : 1.0.                                             
//                                                                
//*****************************************************************

static CAENComm_ErrorCode ReadParametersConfiguration(int handle)
{
CAENComm_ErrorCode ret ;
uint16_t	vme_addr,dato ;
int32_t		i ;

// Channel Number

vme_addr = 0x8100 ;
ret = CAENComm_Read16(handle, vme_addr, &Board.NumCh) ;
if (ret != CAENComm_Success)
	return ret ;

// Board Description

vme_addr = 0x8102 ;
for (i=0; i<20; i+=2)
	{
	CAENComm_Read16(handle, vme_addr+i, &dato) ;
	Board.Description[i] = dato & 0xff ; 
	Board.Description[i+1] = dato >> 8 ; 
	}

// Board Model

vme_addr = 0x8116 ;
for (i=0; i<8; i+=2)
	{
	CAENComm_Read16(handle, vme_addr+i, &dato) ;
	Board.Name[i] = dato & 0xff ; 
	Board.Name[i+1] = dato >> 8 ; 
	}

Board.Model = V6533 ;

if (!strcmp(Board.Name,"V6519P") || !strcmp(Board.Name,"V6519N") || !strcmp(Board.Name,"V6519M"))
	Board.Model = V6519 ;
if (!strcmp(Board.Name,"V6521P") || !strcmp(Board.Name,"V6521N") || !strcmp(Board.Name,"V6521M"))
	Board.Model = V6521 ;
if (!strcmp(Board.Name,"V6521HP") || !strcmp(Board.Name,"V6521HN") || !strcmp(Board.Name,"V6521HM"))
	Board.Model = V6521H ;
if (!strcmp(Board.Name,"V6533P") || !strcmp(Board.Name,"V6533N") || !strcmp(Board.Name,"V6533M"))
	Board.Model = V6533 ;
if (!strcmp(Board.Name,"V6534P") || !strcmp(Board.Name,"V6534N") || !strcmp(Board.Name,"V6534M"))
	Board.Model = V6534 ;

if (Board.Model == V6519)
	{
	Board.VRES = V6519_VRES ;
	Board.VSETMAX = V6519_VSETMAX ;
	Board.IRESH = V6519_IRESH ;
	Board.ISETMAXH = V6519_ISETMAXH ;
	Board.IRESL = V6519_IRESL ;
	Board.ISETMAXL = V6519_ISETMAXL ;
	Board.RAMPMAX = V6519_RAMPMAX ;
	}

if (Board.Model == V6521)
	{
	Board.VRES = V6521_VRES ;
	Board.VSETMAX = V6521_VSETMAX ;
	Board.IRESH = V6521_IRESH ;
	Board.ISETMAXH = V6521_ISETMAXH ;
	Board.IRESL = V6521_IRESL ;
	Board.ISETMAXL = V6521_ISETMAXL ;
	Board.RAMPMAX = V6521_RAMPMAX ;
	}

if (Board.Model == V6521H)
	{
	Board.VRES = V6521H_VRES ;
	Board.VSETMAX = V6521H_VSETMAX ;
	Board.IRESH = V6521H_IRESH ;
	Board.ISETMAXH = V6521H_ISETMAXH ;
	Board.IRESL = V6521H_IRESL ;
	Board.ISETMAXL = V6521H_ISETMAXL ;
	Board.RAMPMAX = V6521H_RAMPMAX ;
	}

if (Board.Model == V6533)
	{
	Board.VRES = V6533_VRES ;
	Board.VSETMAX = V6533_VSETMAX ;
	Board.IRESH = V6533_IRESH ;
	Board.ISETMAXH = V6533_ISETMAXH ;
	Board.IRESL = V6533_IRESL ;
	Board.ISETMAXL = V6533_ISETMAXL ;
	Board.RAMPMAX = V6533_RAMPMAX ;
	}

if (Board.Model == V6534)
	{
	Board.VRES = V6534_VRES ;
	Board.VSETMAX = V6534_VSETMAX ;
	Board.IRESH = V6534_IRESH ;
	Board.ISETMAXH = V6534_ISETMAXH ;
	Board.IRESL = V6534_IRESL ;
	Board.ISETMAXL = V6534_ISETMAXL ;
	Board.RAMPMAX = V6534_RAMPMAX ;
	}

// Board Serial Number

vme_addr = 0x811e;
CAENComm_Read16(handle, vme_addr, &Board.SerNum) ;

// FPGA Firmware Release

vme_addr = 0x8120 ;
CAENComm_Read16(handle, vme_addr, &Board.FPGAFwRelease) ;

// Board Vmax 

vme_addr = 0x50 ;
CAENComm_Read16(handle, vme_addr, &Board.MaxV) ;

// Board Imax

vme_addr = 0x54 ;
CAENComm_Read16(handle, vme_addr, &Board.MaxI) ;

// Board Alarm

vme_addr = 0x58 ;
CAENComm_Read16(handle, vme_addr, &Board.Alarm) ;

// uC Firmware Release

vme_addr = 0x5C ;
CAENComm_Read16(handle, vme_addr, &Board.uCFwRelease) ;

ReadChParameters(handle) ;

return CAENComm_Success ;
}


//*****************************************************************
// Name        : Main.                                            
//                                                                 
// Description : Main program.                                     
//                                                                
// Input       : None.                                         
// Output      : None.                                         
// Global      : None.                                         
// Release     : 1.0.                                             
//                                                                
//*****************************************************************

int main()
{ 
int32_t		handle,res,conet = 0,link,connectionType ;
const void* link2 = (const void*) link;
char		key,fine,buff[20],type[10] ;
uint16_t	value ;
uint32_t	vme_address=0 ;
CAENComm_ErrorCode ret ;

con_init();

do
	{
	fine = 0 ;
	clrscr() ;
	/*con_printf(" **********************************\n") ;
	con_printf(" *****   V65XX VME HV Board   *****\n") ;
	con_printf(" **********************************\n\n") ;
	con_printf("\n Enter the type of the connection to Board [USB|OPTLINK]: ") ;
	res = con_scanf ("%s",&type) ;

	if (strcmp(type,"USB") == 0)
		connectionType = CAENComm_USB;
	else
		{
		if (strcmp(type,"OPTLINK") == 0) 
			{
		//	connectionType = CAENComm_PCI_OpticalLink;
			}
		else 
			{
			con_end();
			con_printf("\n\n !!! Wrong Connection type !!! Press any key ... ") ;
			con_getch() ;
			return -1;
			}
		}

	con_printf("\n\n Enter the link number used: ") ;
	res = con_scanf ("%d",&link) ;*/
	/*if (connectionType == CAENComm_PCI_OpticalLink)
		{
		con_printf("\n\n Enter the Conet node used: ") ;
		res = con_scanf ("%d",&conet) ;
		}*/
	//con_printf("\n\n Enter Board VME Base Address [xxxxxxxx]: ") ;
	//res = con_scanf ("%x",&vme_address) ;
    vme_address = 0x32160000;
    connectionType = CAENComm_USB;
    link = 0;
	//clrscr() ;
	
	CAENComm_OpenDevice(connectionType,link,conet,vme_address, &handle) ;

	// Check if board is wating for firmware update :VME Addr 0x0..3 = "FwUp"

	ret = CAENComm_Read16(handle, 0x0, &value) ;		
	if (ret != CAENComm_Success)
		{
		con_end();
		con_printf("\n\n !!! Board connection ERROR !!! Press any key ... ") ;
		con_getch() ;

		return -1;
		}

	buff[0] = value & 0xff ;
	buff[1] = value >> 8 ;

	ret = CAENComm_Read16(handle, 0x4, &value) ;		
	if (ret != CAENComm_Success)
		{
		con_end();
		return -1;
		}

	buff[2] = value & 0xff ;
	buff[3] = value >> 8 ;
	buff[4] = 0 ;

	if (strcmp(buff,"UpDt") == 0)
		{
		con_printf("\n\n !!! Firmware ERROR !!! Press any key ... ") ;
		con_getch() ;

		return -1;
		}

	//*****************************************************************

	ReadParametersConfiguration(handle) ;

	Welcome() ;

	do
		{	
		key = con_kbhit() ;				
		if (key)
			{
			switch (key)
				{	
				case 'c' :
				case 'C' :	DisplayModifyChannels(handle) ;
							Welcome() ;
							break ;

				case 'b' :
				case 'B' :	DisplayBoard(handle) ;
							Welcome() ;
							break ;

				case 'q' :
				case 'Q' :	fine = 1 ;
							break ;

				default  :	Welcome() ;
							break ;
				}
			}
		}
	while (!fine) ;
	}	
while (!fine) ;

if (vme_address)
	ret = CAENComm_CloseDevice(handle);

con_end();
return 0;
}
