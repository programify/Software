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
 *
 *   The cursor disappears when the screen update frequency is high enough.
 *   This can be simulated by performing a cursor position (to the same place)
 *   in a tight loop. So there must be a time limit within the hardware that
 *   hides the cursor when updating the display.
 */

//*****************************************************************************
//                                    SGR (Select Graphic Rendition) Parameters
//*****************************************************************************
/*
 *   Code Effect         Notes
 *   ==== ============== ====================================
 *   0    Reset          All attributes off.
 *   1    Bright Fg      Foreground high intensity on.
 *   5    Bright Bg      Background high intensity on.
 */

//*****************************************************************************
//                                                                  Development
//*****************************************************************************
/*
 *   25-12-18  Started development.
 *   26-12-18  Added display of character set.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------
#include  <Arduino.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

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
#define   COLOUR_BLACK        30
#define   COLOUR_RED          31
#define   COLOUR_GREEN        32
#define   COLOUR_YELLOW       33
#define   COLOUR_BLUE         34
#define   COLOUR_MAGENTA      35
#define   COLOUR_CYAN         36
#define   COLOUR_WHITE        37

#define   VGAMODE_80x25       0         // 80x25 landscape mode. Default.
#define   VGAMODE_128x48      1         // 128x48 landscape mode. Recommended for 19" screens and above.
#define   VGAMODE_96x64_P     2         // 96x64 portrait mode.

//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------
typedef struct _VGADISPLAY
{
     BOOLEAN   bfBoldBg ;
     BOOLEAN   bfBoldFg ;
     BOOLEAN   bfVideoOut ;
     BYTE      bBack ;
     BYTE      bFore ;
     BYTE      bMode ;
     int       iCol ;
     int       iRow ;
}
     VGADISPLAY ;

//-----------------------------------------------------------------------------
//                                                                    Functions
//-----------------------------------------------------------------------------
void      VgaBrightBg         (BOOLEAN bfEnable) ;
void      VgaBrightFg         (BOOLEAN bfEnable) ;
void      VgaClearScreen      (void) ;
void      VgaCommand          (IN char * cpOption, char cCommand) ;
void      VgaCursorPos        (int iCol, int iRow) ;
void      VgaCursorShow       (BOOLEAN bfShow) ;
void      VgaInit             (IN VGADISPLAY * pvga) ;
void      VgaMode             (BYTE bMode) ;
void      VgaSetColour        (BYTE bFore, BYTE bBack) ;
void      VgaShowCharSet      (int iCol, int iRow) ;
void      VgaShowColours      (int iCol, int iRow) ;

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
VGADISPLAY     g_vga ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals


//=============================================================================
//                                                                        setup
//-----------------------------------------------------------------------------
void setup (void)
{
// Init system I/O
     Serial.begin (115200) ;
// Show screen heading
     VgaInit (& g_vga) ;
     VgaMode (VGAMODE_128x48) ;
     VgaSetColour (COLOUR_WHITE, COLOUR_BLUE) ;
     VgaBrightFg (true) ;
     Serial.print (" TD-Hello ") ;
     delay (500) ;
// Display character set
     VgaShowCharSet (14, 7) ;
// Display colour bars
     VgaShowColours (2, 8) ;
}

//=============================================================================
//                                                                         loop
//-----------------------------------------------------------------------------
void loop (void)
{
     int       iCol ;
     int       iCount ;
     int       iIndex ;
     int       iReading ;
     int       iRow ;

// Count number of samples
     glSamples ++ ;
     VgaSetColour (COLOUR_WHITE, COLOUR_BLACK) ;
     VgaCursorPos (2, 3) ;
     Serial.print (glSamples) ;

// Enumerate each analog channel
     iRow = 0 ;
     for (iIndex = 0 ; iIndex <= 5 ; iIndex ++)
     {
          iReading = analogRead (iIndex);

     // Determine average for this channel
          galSum [iIndex] += (long) iReading ;
          gaiAve [iIndex]  = galSum [iIndex] / glSamples ;
     // Determine in which column results will appear
          if (iIndex > 2)
               iCol = 46 ;
          else
               iCol = 14 ;
     // Display results
          VgaCursorPos (iCol, iRow + 3) ;
          VgaSetColour (31 + iIndex, COLOUR_BLACK) ;
          VgaBrightFg (true) ;
          sprintf (gszMonitor, "A%d : ", iIndex) ;
          Serial.print (gszMonitor) ;
          VgaBrightFg (false) ;
          sprintf (gszMonitor, "%4d %4d %9ld ", iReading, gaiAve [iIndex], galSum [iIndex]) ;
          Serial.print (gszMonitor) ;
     // Cycle between three rows
          iRow ++ ;
          if (iRow > 2)
               iRow = 0 ;
     // Moderate speed of display and hide the cursor
          //delay (100) ;
          //for (iCount = 0 ; iCount < 50 ; iCount ++)
               //VgaCursorPos (13, 9 + iIndex) ;
     }
}

//-----------------------------------------------------------------------------
//                                                                      VgaInit
//-----------------------------------------------------------------------------
void VgaInit (IN VGADISPLAY * pvga)
{
// Initialize VGA data structure
     memset ((BYTE *) & pvga, 0x00, sizeof (VGADISPLAY)) ;
     g_vga.bMode = VGAMODE_80x25 ;

     VgaClearScreen () ;
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

     g_vga.bBack = bBack ;
     g_vga.bFore = bFore ;

     sprintf (szCSI, "%d;%d", bFore, bBack + 10) ;
     VgaCommand (szCSI, 'm') ;
}

//-----------------------------------------------------------------------------
//                                                                VgaCursorShow
//-----------------------------------------------------------------------------
void VgaCursorShow (BOOLEAN bfShow)
{
     BYTE      bShow ;
     char      szCSI [8] ;

     if (bfShow)
          sprintf (szCSI, "%c[%c", 0x1B, 5) ;
     else
          sprintf (szCSI, "%c[l", 0x1B) ;

     Serial.print (szCSI) ;
}

//-----------------------------------------------------------------------------
//                                                                  VgaBrightBg
//-----------------------------------------------------------------------------
void VgaBrightBg (BOOLEAN bfEnable)
{
// Save new bold state
     g_vga.bfBoldBg = bfEnable ;
// Use SGR command to set increased intensity (or reset attributes)
     if (bfEnable)
          VgaCommand ("5", 'm') ;
     else
          VgaCommand ("0", 'm') ;
// Restore colours lost to reset
     if (! bfEnable)
          VgaSetColour (g_vga.bFore, g_vga.bBack) ;
}

//-----------------------------------------------------------------------------
//                                                                  VgaBrightFg
//-----------------------------------------------------------------------------
void VgaBrightFg (BOOLEAN bfEnable)
{
// Save new bold state
     g_vga.bfBoldFg = bfEnable ;
// Use SGR command to set increased intensity (or reset attributes)
     if (bfEnable)
          VgaCommand ("1", 'm') ;
     else
          VgaCommand ("0", 'm') ;
// Restore colours lost to reset
     if (! bfEnable)
          VgaSetColour (g_vga.bFore, g_vga.bBack) ;
}

//-----------------------------------------------------------------------------
//                                                                   VgaCommand
//-----------------------------------------------------------------------------
void VgaCommand (IN char * cpOption, char cCommand)
{
     char      szCSI [8] ;

     sprintf (szCSI, "%c[", 0x1B) ;
     Serial.print (szCSI) ;
     if (cpOption)
          Serial.print (cpOption) ;
     Serial.print ((char) cCommand) ;
}

//-----------------------------------------------------------------------------
//                                                                      VgaMode
//-----------------------------------------------------------------------------
void VgaMode (BYTE bMode)
{
     char      szCSI [8] ;

     g_vga.bMode = bMode ;

     sprintf (szCSI, "%c[", 0x1B) ;
     Serial.print (szCSI) ;
     Serial.print (bMode) ;
     Serial.print ("h") ;
}

//-----------------------------------------------------------------------------
//                                                                  VgaVideoOut
//-----------------------------------------------------------------------------
void VgaVideoOut (BOOLEAN bfVideoOut)
{
     char      szCSI [8] ;

     g_vga.bfVideoOut = bfVideoOut ;

     sprintf (szCSI, "%c[", 0x1B) ;
     Serial.print (szCSI) ;
     if (bfVideoOut)
          Serial.print ((BYTE) 255) ;
     else
          Serial.print ((BYTE) 0) ;
     Serial.print ("S") ;
}

//-----------------------------------------------------------------------------
//                                                               VgaShowCharSet
//-----------------------------------------------------------------------------
void VgaShowCharSet (int iCol, int iRow)
{
     char      szString [8] ;
     int       iCode ;
     int       iX ;
     int       iY ;

// Display column headings
     VgaCursorPos (iCol + 3, iRow) ;
     VgaSetColour (COLOUR_GREEN, COLOUR_BLACK) ;
     VgaBrightFg (false) ;
     for (iCode = 0x00 ; iCode < 0x100 ; iCode += 0x10)
     {
          sprintf (szString, "%02X ", iCode) ;
          Serial.print (szString) ;
     }
// Display row headings
     for (iCode = 0 ; iCode < 0x10 ; iCode ++)
     {
          VgaCursorPos (iCol, iRow + iCode + 1) ;
          sprintf (szString, "+%1X ", iCode) ;
          Serial.print (szString) ;
     }
// Display character set
     VgaBrightFg (true) ;
     iX = iCol + 4 ;
     iY = iRow + 1 ;
     for (iCode = 0 ; iCode < 0x100 ; iCode ++)
     {
     // Ignore the escape character
          if (iCode != 0x1B)
          {
          // Display character
               VgaCursorPos (iX, iY) ;
               VgaSetColour (COLOUR_GREEN, COLOUR_BLACK) ;
               sprintf (szString, "%c", (char) iCode) ;
               Serial.print (szString) ;
          }
     // Manage where to place next character on display
          iY ++ ;
          if (iY > iRow + 16)
          {
               iY  = iRow + 1 ;
               iX += 3 ;
          }
     }
}

//-----------------------------------------------------------------------------
//                                                               VgaShowColours
//-----------------------------------------------------------------------------
void VgaShowColours (int iCol, int iRow)
{
     char      szString [8] ;
     int       iBack ;
     int       iCode ;
     int       iFore ;

// Display column headings
     for (iCode = 0 ; iCode < 16 ; iCode ++)
     {
          if (iCode < 8)
          {
               iBack = iCode + 40 ;
               iFore = COLOUR_WHITE ;
               VgaBrightBg (false) ;
               VgaBrightFg (true) ;
          }
          else
          {
               iBack = iCode + 32 ;   /// 62 ;
               iFore = COLOUR_BLACK ;
               VgaBrightFg (false) ;
               VgaBrightBg (true) ;
          }

          VgaCursorPos (iCol, iRow + iCode) ;
          VgaSetColour (iFore, iBack - 10) ;
          sprintf (szString, " %3d ", iBack) ;
          Serial.print (szString) ;
     }
}
