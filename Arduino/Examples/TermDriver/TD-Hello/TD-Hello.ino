//*****************************************************************************
//
//   Programify Ltd
//   TermDriver Hello World                                        TD-Hello.ino
//
//*****************************************************************************
/*
 *   TermDriver listens on a serial line set to 115200 baud, 8-bits, no parity
 *   and emulates a terminal on a standard VGA connector. Serial output from 
 *   the Arduino appears on the VGA.
 *
 *   TermDriver is a full hardware emulation of a dumb terminal is fully 
 *   compatible with the DOS ANSI.SYS so there is no need for a device driver 
 *   library. TermDriver's output is native 1024x768 at 60Hz, VGA.
 *
 *   See https://en.wikipedia.org/wiki/ANSI.SYS
 *
 *   Sourced from https://coolcomponents.co.uk/products/termdriver
 */

//*****************************************************************************
//                                      Control Sequence Introducer (CSI) Codes
//*****************************************************************************
/*
 *   Source: https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_sequences
 *
 *   Terminal controls are introduced with "ESC [", followed by a code which 
 *   has an effect on the terminal's display. These are:
 *
 *   Code           Effect
 *   =============  ========================
 *   ESC [ n A      Cursor up
 *   ESC [ n B      Cursor down
 *   ESC [ n C      Cursor forward
 *   ESC [ n D      Cursor back
 *   ESC [ r;c H    Cursor position
 *   ESC [ n J      Erase display
 *   ESC [ n m      Select graphic rendition
 *   ESC [ s        Save cursor position
 *   ESC [ u        Restore cursor position
 *   -------------  ------------------------
 *   ESC [ n h      Set display mode
 *   ESC [ n S      Screen-saver
 *   =============  ========================
 */

//*****************************************************************************
//                                                                  Development
//*****************************************************************************
/*
 *   25-12-18  Started development.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------
#include  <Arduino.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Data Types

typedef   uint8_t             BOOLEAN ;
typedef   int16_t             BOOL ;
typedef   uint8_t             BYTE ;
typedef   uint32_t            DWORD ;
typedef   uint64_t            QWORD ;
typedef   int8_t              SBYTE ;
typedef   int32_t             SDWORD ;
typedef   int64_t             SQWORD ;
typedef   int16_t             SWORD ;
typedef   uint16_t            WORD ;

#define   IN
#define   OUT

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Manifest Constants

// FG Colour Codes (Add 10 to get BG code)
#define   COLOUR_BLACK          30
#define   COLOUR_RED            31
#define   COLOUR_GREEN          32
#define   COLOUR_YELLOW         33
#define   COLOUR_BLUE           34
#define   COLOUR_MAGENTA        35
#define   COLOUR_CYAN           36
#define   COLOUR_WHITE          37
#define   COLOUR_BRIGHTBLACK    90
#define   COLOUR_BRIGHTRED      91
#define   COLOUR_BRIGHTGREEN    92
#define   COLOUR_BRIGHTYELLOW   93
#define   COLOUR_BRIGHTBLUE     94
#define   COLOUR_BRIGHTMAGENTA  95
#define   COLOUR_BRIGHTCYAN     96
#define   COLOUR_BRIGHTWHITE    97


//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                                    Functions
//-----------------------------------------------------------------------------
void      VgaClearScreen      (void) ;
void      VgaCursorPos        (int iCol, int iRow) ;
void      VgaCursorShow       (BOOLEAN bfShow) ;
void      VgaSetColour        (BYTE bFore, BYTE bBack) ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Functions

//-----------------------------------------------------------------------------
//                                                                  Global Data
//-----------------------------------------------------------------------------
char      gszMonitor [120] ;

int       gaiAve [6] ;

long      galSum [6] ;

long      glSamples ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Volatile Data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Classes

//- - - - - - - - - - - - - - - - - - - - - - - - - - Composite Data Structures

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals


//=============================================================================
//                                                                        setup
//-----------------------------------------------------------------------------
void setup (void)
{
     Serial.begin (115200) ;

     VgaClearScreen () ;
     VgaSetColour (COLOUR_BRIGHTWHITE, COLOUR_BLUE) ;
     Serial.print (" TD-Hello ") ;
     delay (1000) ;
     //VgaCursorShow (false) ;
}

//=============================================================================
//                                                                         loop
//-----------------------------------------------------------------------------
void loop (void)
{
     int  iIndex ;
     int  iReading ;

// Count number of samples
     glSamples ++ ;
     VgaSetColour (COLOUR_WHITE, COLOUR_BLACK) ;
     VgaCursorPos (5, 6) ;
     Serial.print (glSamples) ;

// Enumerate each analog channel
     for (iIndex = 0 ; iIndex <= 5 ; iIndex ++)
     {
          iReading = analogRead (iIndex);

     // Determine average for this channel
          galSum [iIndex] += (long) iReading ;
          gaiAve [iIndex]  = galSum [iIndex] / glSamples ;

     // Display results
          VgaSetColour (31 + iIndex, COLOUR_BLACK) ;
          VgaCursorPos (10, 9 + iIndex) ;
          sprintf (gszMonitor, "A%d : %4d   %4d   %ld ", iIndex, iReading, gaiAve [iIndex], galSum [iIndex]) ;
          Serial.print (gszMonitor) ;

          if (iIndex > 2)
               VgaCursorShow (true) ;
          else
               VgaCursorShow (false) ;

     // Moderate speed of display
          delay (100) ;
     }
}

//-----------------------------------------------------------------------------
//                                                               VgaClearScreen
//-----------------------------------------------------------------------------
void VgaClearScreen (void)
{
     char      szCSI [8] ;

     sprintf (szCSI, "%c[2J", 0x1B) ;
     Serial.print (szCSI) ;
}

//-----------------------------------------------------------------------------
//                                                                 VgaCursorPos
//-----------------------------------------------------------------------------
void VgaCursorPos (int iCol, int iRow)
{
     char      szCSI [12] ;

     sprintf (szCSI, "%c[%d;%dH", 0x1B, iRow, iCol) ;
     Serial.print (szCSI) ;
}

//-----------------------------------------------------------------------------
//                                                                 VgaSetColour
//-----------------------------------------------------------------------------
void VgaSetColour (BYTE bFore, BYTE bBack)
{
     char      szCSI [12] ;

     sprintf (szCSI, "%c[%d;%dm", 0x1B, bFore, bBack + 10) ;
     Serial.print (szCSI) ;
}

//-----------------------------------------------------------------------------
//                                                                VgaCursorShow
//-----------------------------------------------------------------------------
void VgaCursorShow (BOOLEAN bfShow)
{
     BYTE      bShow ;
     char      szCSI [12] ;

     if (bfShow)
          sprintf (szCSI, "%c[%c", 0x1B, 5) ;
     else
          sprintf (szCSI, "%c[l", 0x1B) ;

     Serial.print (szCSI) ;
}
