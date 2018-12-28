//*****************************************************************************
//
//   Programify Ltd
//   TermDriver VGA Library                                            vgalib.h
//
//*****************************************************************************

//*****************************************************************************
//                                                                  Development
//*****************************************************************************
/*
 *   28-12-18  Started development.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------

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
     char      szCommand [20] ;        // CSI command strings and fragments.
     char      szParams  [20] ;
     int       iCol ;
     int       iRow ;
}
     VGADISPLAY ;

//-----------------------------------------------------------------------------
//                                                             Public Functions
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Private Functions

//-----------------------------------------------------------------------------
//                                                                 Library Data
//-----------------------------------------------------------------------------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Volatile Data

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Classes

//- - - - - - - - - - - - - - - - - - - - - - - - - - Composite Data Structures

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals

