// UTouchCD.h
// ----------
//
// Since there are slight deviations in all touch screens you should run a
// calibration on your display module. Run the UTouch_Calibration sketch
// that came with this library and follow the on-screen instructions to
// update this file.
//
// Remember that is you have multiple display modules they will probably 
// require different calibration data so you should run the calibration
// every time you switch to another module.
// You can, of course, store calibration data for all your modules here
// and comment out the ones you dont need at the moment.
//

// These calibration settings works with my ITDB02-3.2S.
// They MIGHT work on your display module, but you should run the
// calibration sketch anyway.

// TFT 240x400 for rDUINOScope HX8352B / XPT2046
 #define CAL_X 0x00250EDEUL
 #define CAL_Y 0x03C1C114UL
 #define CAL_S 0x0018F0EFUL

// TFT 240x400 for rDUINOScope V1.2
// #define CAL_X 0x01FA07F8UL
// #define CAL_Y 0x01FDC802UL
// #define CAL_S 0x0018F0EFUL

// TFT 240x400 with ILI9527 / XPT2046
//#define CAL_X 0x0046CEFEUL
//#define CAL_Y 0x03DEC0DBUL
//#define CAL_S 0x000EF18FUL

// TFT 240x400 with HX8352B / XPT2046
//#define CAL_X 0x003CCEC4UL
//#define CAL_Y 0x03F1C039UL
//#define CAL_S 0x000EF18FUL

// 7.0" TFT 800x480 CPLD - LANDSCAPE
//#define CAL_X 0x0075CD95UL
//#define CAL_Y 0x00BDCC9DUL
//#define CAL_S 0x8031F1DFUL

// 4,3" TFT 480x272 LANDSCAPE (SSD1963 / XPT2046)
//#define CAL_X 0x03F04013UL
//#define CAL_Y 0x039EC0D0UL
//#define CAL_S 0x801DF10FUL
