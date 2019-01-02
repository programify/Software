//*****************************************************************************
//
//   (c) programify.com
//   Open Source Project
//
//   TermDriver VGA Library                                          vgalib.cpp
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
 *   28-12-18  Started development.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------
#include  <Arduino.h>

#include  "vgalib.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Data Types

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Manifest Constants

//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                            Private Functions
//-----------------------------------------------------------------------------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Public Functions

//-----------------------------------------------------------------------------
//                                                                 Library Data
//-----------------------------------------------------------------------------
VGADISPLAY * lib_pvga ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Volatile Data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Classes

//- - - - - - - - - - - - - - - - - - - - - - - - - - Composite Data Structures

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals


//=============================================================================
//                                                                      VgaInit
//-----------------------------------------------------------------------------
void VgaInit (IN VGADISPLAY * pvga)
{
     lib_pvga = pvga ;
// Initialize VGA data structure
     memset ((BYTE *) lib_pvga, 0x00, sizeof (VGADISPLAY)) ;
     lib_pvga->bMode = VGAMODE_80x25 ;

     VgaClearScreen () ;
}

//=============================================================================
//                                                               VgaClearScreen
//-----------------------------------------------------------------------------
void VgaClearScreen (void)
{
     sprintf (lib_pvga->szCommand, "%c[2J", 0x1B) ;
     Serial.print (lib_pvga->szCommand) ;
}

//=============================================================================
//                                                                 VgaCursorPos
//-----------------------------------------------------------------------------
void VgaCursorPos (int iCol, int iRow)
{
     sprintf (lib_pvga->szCommand, "%c[%d;%dH", 0x1B, iRow, iCol) ;
     Serial.print (lib_pvga->szCommand) ;
}

//=============================================================================
//                                                                 VgaSetColour
//-----------------------------------------------------------------------------
void VgaSetColour (BYTE bFore, BYTE bBack)
{
     lib_pvga->bBack = bBack ;
     lib_pvga->bFore = bFore ;

     sprintf (lib_pvga->szParams, "%d;%d", bFore, bBack + 10) ;
     VgaCommand (lib_pvga->szParams, 'm') ;
}

//=============================================================================
//                                                                VgaCursorShow
//-----------------------------------------------------------------------------
void VgaCursorShow (BOOLEAN bfShow)
{
     BYTE      bShow ;

     if (bfShow)
          sprintf (lib_pvga->szCommand, "%c[%c", 0x1B, 5) ;
     else
          sprintf (lib_pvga->szCommand, "%c[l", 0x1B) ;

     Serial.print (lib_pvga->szCommand) ;
}

//=============================================================================
//                                                                  VgaBrightBg
//-----------------------------------------------------------------------------
void VgaBrightBg (BOOLEAN bfEnable)
{
// Save new bold state
     lib_pvga->bfBoldBg = bfEnable ;
// Use SGR command to set increased intensity (or reset attributes)
     if (bfEnable)
          VgaCommand ("5", 'm') ;
     else
          VgaCommand ("0", 'm') ;
// Restore colours lost to reset
     if (! bfEnable)
          VgaSetColour (lib_pvga->bFore, lib_pvga->bBack) ;
}

//=============================================================================
//                                                                  VgaBrightFg
//-----------------------------------------------------------------------------
void VgaBrightFg (BOOLEAN bfEnable)
{
// Save new bold state
     lib_pvga->bfBoldFg = bfEnable ;
// Use SGR command to set increased intensity (or reset attributes)
     if (bfEnable)
          VgaCommand ("1", 'm') ;
     else
          VgaCommand ("0", 'm') ;
// Restore colours lost to reset
     if (! bfEnable)
          VgaSetColour (lib_pvga->bFore, lib_pvga->bBack) ;
}

//=============================================================================
//                                                                   VgaCommand
//-----------------------------------------------------------------------------
void VgaCommand (IN char * cpOption, char cCommand)
{
     sprintf (lib_pvga->szCommand, "%c[", 0x1B) ;
     Serial.print (lib_pvga->szCommand) ;
     if (cpOption)
          Serial.print (cpOption) ;
     Serial.print ((char) cCommand) ;
}

//=============================================================================
//                                                                      VgaMode
//-----------------------------------------------------------------------------
/*
 *   VGAMODE_80x25       80x25 landscape mode. Default.
 *   VGAMODE_128x48      128x48 landscape mode.
 *   VGAMODE_96x64_P     96x64 portrait mode.
 */
void VgaMode (BYTE bMode)
{
     lib_pvga->bMode = bMode ;

     sprintf (lib_pvga->szCommand, "%c[", 0x1B) ;
     Serial.print (lib_pvga->szCommand) ;
     Serial.print (bMode) ;
     Serial.print ("h") ;
}

//=============================================================================
//                                                                  VgaVideoOut
//-----------------------------------------------------------------------------
void VgaVideoOut (BOOLEAN bfVideoOut)
{
     lib_pvga->bfVideoOut = bfVideoOut ;

     sprintf (lib_pvga->szCommand, "%c[", 0x1B) ;
     Serial.print (lib_pvga->szCommand) ;
     if (bfVideoOut)
          Serial.print ((BYTE) 255) ;
     else
          Serial.print ((BYTE) 0) ;
     Serial.print ("S") ;
}

//=============================================================================
//                                                               VgaShowCharSet
//-----------------------------------------------------------------------------
void VgaShowCharSet (int iCol, int iRow)
{
     int       iCode ;
     int       iX ;
     int       iY ;

// Display column headings
     VgaCursorPos (iCol + 3, iRow) ;
     VgaSetColour (COLOUR_GREEN, COLOUR_BLACK) ;
     VgaBrightFg (false) ;
     for (iCode = 0x00 ; iCode < 0x100 ; iCode += 0x10)
     {
          sprintf (lib_pvga->szParams, "%02X ", iCode) ;
          Serial.print (lib_pvga->szParams) ;
     }
// Display row headings
     for (iCode = 0 ; iCode < 0x10 ; iCode ++)
     {
          VgaCursorPos (iCol, iRow + iCode + 1) ;
          sprintf (lib_pvga->szParams, "+%1X ", iCode) ;
          Serial.print (lib_pvga->szParams) ;
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
               sprintf (lib_pvga->szParams, "%c", (char) iCode) ;
               Serial.print (lib_pvga->szParams) ;
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

//=============================================================================
//                                                               VgaShowColours
//-----------------------------------------------------------------------------
void VgaShowColours (int iCol, int iRow)
{
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
          sprintf (lib_pvga->szParams, " %3d ", iBack) ;
          Serial.print (lib_pvga->szParams) ;
     }
}
