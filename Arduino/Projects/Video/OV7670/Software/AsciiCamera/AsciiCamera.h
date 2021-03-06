//*****************************************************************************
//
//   (c) programify.com
//   Open Source Project
//
//   ASCII Camera Display                                         AsciiCamera.h
//
//*****************************************************************************

//*****************************************************************************
//                                                            Programming Notes
//*****************************************************************************

//*****************************************************************************
//                                                               Hardware Notes
//*****************************************************************************

//*****************************************************************************
//                                                                  Development
//*****************************************************************************
/*
 *   01-01-19  Started development.
 */

//-----------------------------------------------------------------------------
//                                                          Compiler Directives
//-----------------------------------------------------------------------------

#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-fpermissive"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Common Libraries
#include  <SPI.h>
#include  <Wire.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - Localised Libraries
#include  "libs\ov7670.h"
#include  "vgalib.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - Configuration Constants
#define   IMAGE_WIDTH         320       // QVGA
#define   IMAGE_HEIGHT        240       // QVGA

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Manifest Constants

//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                                    Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                                  Global Data
//-----------------------------------------------------------------------------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals
