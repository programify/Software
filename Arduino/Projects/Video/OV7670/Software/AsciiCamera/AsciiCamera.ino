//*****************************************************************************
//
//   (c) programify.com
//   Open Source Project
//
//   ASCII Camera Display                                       AsciiCamera.ino
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
#include  "AsciiCamera.h"

#pragma   GCC diagnostic ignored "-Wwrite-strings"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Manifest Constants
#define   DISPLAY_LEFTCOL     9
#define   DISPLAY_TOPROW      4

#define   PIN_XCLK            7         // 10MHz.
#define   PIN_RESET           33        // Camera reset.
#define   PIN_VSYNC           52        // VGA frame timing.
#define   PIN_PCLK            32        // 
#define   PIN_HREF            31        // Horizontal timing.

#define   SCALE_NORMAL        0x01
#define   SCALE_REVERSE       0x02

#define   STYLE_BLOCKS        0x01
#define   STYLE_CHARMAX72     0x02
#define   STYLE_CHARMIN10     0x03

//-----------------------------------------------------------------------------
//                                                                   Structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                                                                    Functions
//-----------------------------------------------------------------------------
BOOLEAN   BrightScale    (BYTE bStyle, BYTE bDirection) ;
void      CameraInit     (int iPinXClock) ;
void      CameraSetRegs  (const struct regval_list reglist[]) ;
void      CaptureImage   (WORD wg, WORD hg) ;
void      ImageDump      (WORD wg, WORD hg) ;
void      SetColour      (void) ;
void      SetResolution  (void) ;
int       UartPutchar    (BYTE c) ;
void      WireWrite      (BYTE regID, BYTE regDat) ;

//-----------------------------------------------------------------------------
//                                                                  Global Data
//-----------------------------------------------------------------------------
BYTE      gbDownScale ;
BYTE      gbScaleMax ;

char      gacScale [80] ;

char      gszMonitor [80] ;

BYTE      gabImage [IMAGE_HEIGHT][IMAGE_WIDTH] ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global Classes

//- - - - - - - - - - - - - - - - - - - - - - - - - - Composite Data Structures
VGADISPLAY     g_vga ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Literals

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Externals


//=============================================================================
//
//                                                                        setup
//
//-----------------------------------------------------------------------------
void setup (void)
{
     Serial.begin (115200) ;
     while (! Serial) ;

     Wire.begin () ;

// Global inits
     BrightScale (STYLE_CHARMIN10, SCALE_NORMAL) ;
// Show screen heading
     VgaInit (& g_vga) ;
     VgaMode (VGAMODE_128x48) ;
     VgaSetColour (COLOUR_WHITE, COLOUR_BLUE) ;
     VgaBrightFg (false) ;
     Serial.print (" AsciiCamera (c) programify.com ") ;
     VgaSetColour (COLOUR_GREEN, COLOUR_BLACK) ;
     //VgaBrightFg (true) ;

     CameraInit (PIN_XCLK) ;
     SetResolution () ;
     SetColour () ;
     WireWrite (0x11, 6) ;
}

//=============================================================================
//                                                                         loop
//-----------------------------------------------------------------------------
void loop (void)
{
     CaptureImage (IMAGE_WIDTH, IMAGE_HEIGHT) ;
     ImageDump    (IMAGE_WIDTH, IMAGE_HEIGHT) ;
}

//-----------------------------------------------------------------------------
//                                                                 CaptureImage
//-----------------------------------------------------------------------------
void CaptureImage (WORD wg, WORD hg)
{
     WORD      y ;
     WORD      x ;

/*
     BYTE      bVSync ;
     BYTE      bPrev ;

     bPrev = 99 ;
     while (true)
     {
          bVSync = digitalRead (PIN_VSYNC) ;

          if (bVSync != bPrev)
          {
               Serial.println (bVSync) ;
               bPrev = bVSync ;
          }
     }
*/
// Wait for high
     while (! (REG_PIOB_PDSR & (1 << 21))) ;
// Wait for low
     while (REG_PIOB_PDSR & (1 << 21)) ;

     y = hg ;
     while (y --)
     {
          x = wg ;
          while (x --)
          {
          // wait for low
               while (REG_PIOD_PDSR & (1 << 10)) ;

               gabImage [y][x] = (REG_PIOC_PDSR & 0xFF000) >> 12 ;

          // wait for high
               while (! (REG_PIOD_PDSR & (1 << 10))) ;

          // wait for low
               while (REG_PIOD_PDSR & (1 << 10)) ;
          /// Fetch second byte ?
          // wait for high
               while (! (REG_PIOD_PDSR & (1 << 10))) ;
          }
     }
}

//-----------------------------------------------------------------------------
//                                                                    WireWrite
//-----------------------------------------------------------------------------
void WireWrite (BYTE regID, BYTE regDat)
{
     Wire.beginTransmission (I2C_ADDR) ;
     Wire.write (regID  & 0x00FF) ;
     Wire.write (regDat & 0x00FF) ;
    
     if (Wire.endTransmission (true))
     {
          ;
          //sprintf (gszMonitor, "Error: Register 0x%02X\n", regID) ;
          //Serial.print (gszMonitor) ;
     }
     delay (10) ;
}

//-----------------------------------------------------------------------------
//                                                                  UartPutchar
//-----------------------------------------------------------------------------
/*
int UartPutchar (BYTE c)
{
     while (! (UART->UART_SR & UART_SR_TXRDY)) ;
     UART->UART_THR = c ;
     return 0 ;
}
*/

//-----------------------------------------------------------------------------
//                                                                    ImageDump
//-----------------------------------------------------------------------------
void ImageDump (WORD wg, WORD hg)
{
     BYTE      bPixel ;
     char      cPixel ;
     WORD      y ;
     WORD      yInc ;
     WORD      x ;
     WORD      xInc ;

     BYTE      bCol ;
     BYTE      bRow ;


     yInc = hg / 40 ;
     xInc = wg / 96 ;

     bRow = DISPLAY_TOPROW ;
     bCol = DISPLAY_LEFTCOL ;

     for (y = 0; y < hg; y += yInc)
     {
          for (x = 0; x < wg; x += xInc)
          {
          // Convert pixel value to ASCII
               cPixel = gabImage [y][x] ;
               bPixel = cPixel / gbDownScale ;
               if (bPixel > gbScaleMax)
                    bPixel = gbScaleMax ;
               cPixel = gacScale [bPixel] ;

               bCol ++ ;
               VgaCursorPos (bCol, bRow) ;
               Serial.print (cPixel) ;
          }
          bCol = DISPLAY_LEFTCOL ;
          bRow ++ ;
          //Serial.print ("\n") ;      
     }
     //Serial.print ("\n") ;      
}

//-----------------------------------------------------------------------------
//                                                                   CameraInit
//-----------------------------------------------------------------------------
void CameraInit (int iPinXClock)
{
     DWORD     mask_PWM_pin ;

// Establish output clock signal at 10.5MHz  
     mask_PWM_pin = digitalPinToBitMask (iPinXClock) ;
     REG_PMC_PCER1  = 1 << 4 ;          // activate clock for PWM controller
     REG_PIOC_PDR  |= mask_PWM_pin ;    // activate peripheral functions for pin (disables all PIO functionality)
     REG_PIOC_ABSR |= mask_PWM_pin ;    // choose peripheral option B    
     REG_PWM_CLK    = 0 ;               // choose clock rate, 0 -> full MCLK as reference 84MHz
     REG_PWM_CMR6   = 0 << 9 ;          // select clock and polarity for PWM channel (pin7) -> (CPOL = 0)
     REG_PWM_CPRD6  = 8 ;               // initialize PWM period -> T = value/84MHz (value: up to 16bit), value=8 -> 10.5MHz
     REG_PWM_CDTY6  = 4 ;               // initialize duty cycle, REG_PWM_CPRD6 / value = duty cycle, for 8/4 = 50%
     REG_PWM_ENA    = 1 << 6 ;          // enable PWM on PWM channel (pin 7 = PWML6)
// Reset the camera
     pinMode      (PIN_RESET, OUTPUT) ;
     digitalWrite (PIN_RESET, HIGH) ;
// Eh ?
     pinMode      (pullup1, OUTPUT) ;
     digitalWrite (pullup1, HIGH) ;
// Eh ?
     pinMode      (pullup2, OUTPUT) ;
     digitalWrite (pullup2, HIGH) ;

     WireWrite (0x12, 0x80) ;
     delayMicroseconds (100) ;

     CameraSetRegs (ov7670_default_regs) ;

     WireWrite (REG_COM10, PIN_PCLK) ; // PCLK does not toggle on HBLANK.
// Eh ?
     pinMode (13, OUTPUT) ;
// Parallel data input from camera    
     pinMode (D1, INPUT) ;
     pinMode (D2, INPUT) ;
     pinMode (D3, INPUT) ;
     pinMode (D4, INPUT) ;
     pinMode (D5, INPUT) ;
     pinMode (D6, INPUT) ;
     pinMode (D7, INPUT) ;
     pinMode (D8, INPUT) ;
// Clock and v-sync input lines from camera
     pinMode (PIN_VSYNC, INPUT) ;
     pinMode (PIN_PCLK,  INPUT) ;
}

//-----------------------------------------------------------------------------
//                                                                    SetColour
//-----------------------------------------------------------------------------
void SetColour (void)
{
     CameraSetRegs (yuv422_ov7670) ;
}

//-----------------------------------------------------------------------------
//                                                                SetResolution
//-----------------------------------------------------------------------------
void SetResolution (void)
{
// REG_COM3 enable scaling
     WireWrite (REG_COM3, 4) ;
     CameraSetRegs (qvga_ov7670) ;
}

//-----------------------------------------------------------------------------
//                                                                CameraSetRegs
//-----------------------------------------------------------------------------
void CameraSetRegs (const struct regval_list reglist [])
{
     int       index = 0 ;

     regval_list regpaar = reglist [index] ;

     do
     {
          WireWrite (regpaar.reg_num, regpaar.value) ;
          index ++ ;
          regpaar = reglist [index] ;
     }
          while (regpaar.reg_num != 0xFF) ;
}

//-----------------------------------------------------------------------------
//                                                                  BrightScale
//-----------------------------------------------------------------------------
BOOLEAN BrightScale (BYTE bStyle, BYTE bDirection)
{
     switch (bDirection)
     {
     // Bright levels map to optically dense characters
          case SCALE_NORMAL :
               switch (bStyle)
               {
                    case STYLE_BLOCKS :
                         gacScale [0] = 0x20 ;
                         gacScale [1] = 0xB0 ;
                         gacScale [2] = 0xB1 ;
                         gacScale [3] = 0xB2 ;
                         gacScale [4] = 0xDB ;
                         gacScale [5] = 0x00 ;
                         break ;

                    case STYLE_CHARMAX72 : strcpy (gacScale, " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$") ; break ;
                    case STYLE_CHARMIN10 : strcpy (gacScale, " .:-=+*#%@") ; break ;

                    default :
                         return false ;
               }
               break ;

     // Dark levels map to optically dense characters
          case SCALE_REVERSE :
               switch (bStyle)
               {
                    case STYLE_BLOCKS :
                         gacScale [0] = 0xDB ;
                         gacScale [1] = 0xB2 ;
                         gacScale [2] = 0xB1 ;
                         gacScale [3] = 0xB0 ;
                         gacScale [4] = 0x20 ;
                         gacScale [5] = 0x00 ;
                         break ;

                    case STYLE_CHARMAX72 : strcpy (gacScale, "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ") ; break ;
                    case STYLE_CHARMIN10 : strcpy (gacScale, "@%#*+=-:. ") ; break ;

                    default :
                         return false ;
               }
               break ;

          default :
               return false ;
     }
// Precalculate translation factors
     gbScaleMax  = strlen (gacScale) ;
     gbDownScale = 256 / gbScaleMax ;
     gbScaleMax -- ;
     return true ;
}
