/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>

// PIC24FJ128GA202 Configuration Bit Settings

// CONFIG4
// Deep Sleep Watchdog Timer Postscale Select bits (1:68719476736 (25.7 Days))
#pragma config DSWDTPS = DSWDTPS1F      
// DSWDT Reference Clock Select (DSWDT uses LPRC as reference clock)
#pragma config DSWDTOSC = LPRC          
// Deep Sleep BOR Enable bit (DSBOR Disabled)
#pragma config DSBOREN = OFF            
// Deep Sleep Watchdog Timer Enable (DSWDT Disabled)
#pragma config DSWDTEN = OFF            
// DSEN Bit Enable (Deep Sleep operation is always disabled)
#pragma config DSSWEN = OFF             
// USB 96 MHz PLL Prescaler Select bits (PLL Disabled)
#pragma config PLLDIV = DISABLED        
// Alternate I2C1 enable bit (I2C1 uses SCL1 and SDA1 pins)
#pragma config I2C1SEL = DISABLE        
/* PPS IOLOCK Set Only Once Enable bit (The IOLOCK bit can be set and cleared 
 * using the unlock sequence) */
#pragma config IOL1WAY = OFF            

// CONFIG3
// Write Protection Flash Page Segment Boundary (Page 127 (0x1FC00))
#pragma config WPFP = WPFP127           
// SOSC Selection bits (Digital (SCLKI) mode)
#pragma config SOSCSEL = OFF            
/* Window Mode Watchdog Timer Window Width Select (Watch Dog Timer Window Width
 * is 25 percent) */
#pragma config WDTWIN = PS25_0          
// PLL Secondary Selection Configuration bit (PLL is fed by the Primary osc)
#pragma config PLLSS = PLL_PRI          
// Brown-out Reset Enable (Brown-out Reset Enable)
#pragma config BOREN = ON               
// Segment Write Protection Disable (Disabled)
#pragma config WPDIS = WPDIS            
// Write Protect Configuration Page Select (Disabled)
#pragma config WPCFG = WPCFGDIS         
/* Segment Write Protection End Page Select (Write Protect from WPFP to the last
 * page of memory) */
#pragma config WPEND = WPENDMEM         

// CONFIG2
// Primary Oscillator Select (HS Oscillator Enabled)
#pragma config POSCMD = HS              
// WDT Clock Source Select bits (WDT uses LPRC)
#pragma config WDTCLK = LPRC            
// OSCO Pin Configuration (OSCO/CLKO/RA3 functions as CLKO (FOSC/2))
#pragma config OSCIOFCN = OFF           
/* Clock Switching and Fail-Safe Clock Monitor Configuration bits (Clock 
 * switching and Fail-Safe Clock Monitor are disabled) */
#pragma config FCKSM = CSDCMD           
// Initial Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config FNOSC = PRI              
/* Alternate Comparator Input bit (C1INC is on RB13, C2INC is on RB9 and C3INC 
 * is on RA0) */
#pragma config ALTCMPI = CxINC_RB       
// WDT always uses LPRC as its clock source
#pragma config WDTCMX = LPRC            
// Internal External Switchover (Disabled)
#pragma config IESO = OFF               

// CONFIG1
// Watchdog Timer Postscaler Select (1:256)
#pragma config WDTPS = PS256            
// WDT Prescaler Ratio Select (1:128)
#pragma config FWPSA = PR128            
// Windowed WDT Disable (Standard Watchdog Timer)
#pragma config WINDIS = OFF             
// Watchdog Timer Enable (WDT controlled with the SWDTEN bit)
#pragma config FWDTEN = SWON            
/* Emulator Pin Placement Select bits (Emulator functions are shared with 
 * PGEC1/PGED1) */
#pragma config ICS = PGx1               
// Low power regulator control (Disabled - regardless of RETEN)
#pragma config LPCFG = OFF              
// General Segment Write Protect (Write to program memory not allowed)
#pragma config GWRP = ON                
// General Segment Code Protect (Code protection is enabled)
#pragma config GCP = ON                 
// JTAG Port Enable (Disabled)
#pragma config JTAGEN = OFF             

/* Note that WDT is set up for 1.024s according to CONFIG 1 bits WDTPS and FWPSA
 * according to Table 9-2 in WDT FRM document and SWDTEN control according 
 * to FWDTEN bits. */