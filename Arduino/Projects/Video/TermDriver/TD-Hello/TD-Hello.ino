//*****************************************************************************
//
//   (c) programify.com
//   Open Source Project
//
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
//                                                                  Development
//*****************************************************************************
/*
 *   25-12-18  Started development.
 *   26-12-18  Added display of character set.
 *   28-12-18  Modified to use VGA functions in an external library.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------
#include  <Arduino.h>

#include  "libs\vgalib.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-fpermissive"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Manifest Constants
#define   LIMIT_COLOURCHANGE  9000 // Change colour every 'x' blocks painted.
#define   MAX_COLOUR          COLOUR_WHITE
#define   MIN_COLOUR          COLOUR_RED

//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                                    Functions
//-----------------------------------------------------------------------------
void      UpdateBlocks   (void) ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Functions

//-----------------------------------------------------------------------------
//                                                                  Global Data
//-----------------------------------------------------------------------------
char      gszMonitor [120] ;

int       gaiAve [6] ;

int       giColour ;               // Current colour for random blocks.
int       giCount ;                // Number of blocks painted with current colour.

long      galSum [6] ;

long      glSamples ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Volatile Data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Classes

//- - - - - - - - - - - - - - - - - - - - - - - - - - Composite Data Structures
VGADISPLAY     g_vga ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals
BYTE      gabBlocks [] = { 0xDB, 0xB0, 0xB1, 0xB2, 0xDB, 0xDB } ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals


//=============================================================================
//                                                                        setup
//-----------------------------------------------------------------------------
void setup (void)
{
// Init globals
     giColour = MIN_COLOUR ;
     randomSeed (0) ;
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
     // Update random segment
          UpdateBlocks () ;
     // Further moderate speed of display
          //delay (100) ;
     // Hide cursor if required delay is relatively long
          //for (iCount = 0 ; iCount < 50 ; iCount ++)
               //VgaCursorPos (13, 9 + iIndex) ;
     }
}

//-----------------------------------------------------------------------------
//                                                                 UpdateBlocks
//-----------------------------------------------------------------------------
void UpdateBlocks (void)
{
     BYTE      abChar [2] ;
     int       iCol ;
     int       iIndex ;
     int       iRow ;

// Init
     abChar [1] = 0x00 ;
// Generate X/Y offsets for lower half of screen (128x24)
     iCol = random (2, 128) ;
     iRow = random (25, 49) ;
// Generate character to display
     iIndex = random (0, 5) ;
     abChar [0] = gabBlocks [iIndex] ;
// Update display
     VgaSetColour (giColour, COLOUR_BLACK) ;
     VgaCursorPos (iCol, iRow) ;
     Serial.print ((char *) & abChar) ;
// Colour selector
     giCount ++ ;
     if (giCount >= LIMIT_COLOURCHANGE)
     {
          randomSeed (0) ;
          giCount = 0 ;
          giColour ++ ;
          if (giColour > MAX_COLOUR)
               giColour = MIN_COLOUR ;
     }
}
