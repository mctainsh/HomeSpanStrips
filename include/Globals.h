#pragma once

#define MY_VERSION "6.83"

// Duration of the fade
#define POWER_FADE_MS 5000.0

//#define PIXEL_COUNT 300

// NOTE : W2815 (12V using need to swap red and green in DEV_RebLed.h)
//#define IS_W2815_12V false

#define NEO_RGB ((0 << 6) | (0 << 4) | (1 << 2) | (2)) ///< Transmit as R,G,B
//#define NEO_RBG ((0 << 6) | (0 << 4) | (2 << 2) | (1)) ///< Transmit as R,B,G
//#define NEO_GRB ((1 << 6) | (1 << 4) | (0 << 2) | (2)) ///< Transmit as G,R,B
//#define NEO_GBR ((2 << 6) | (2 << 4) | (0 << 2) | (1)) ///< Transmit as G,B,R
//#define NEO_BRG ((1 << 6) | (1 << 4) | (2 << 2) | (0)) ///< Transmit as B,R,G
//#define NEO_BGR ((2 << 6) | (2 << 4) | (1 << 2) | (0)) ///< Transmit as B,G,R

// Maximum power is 255. 50 is a good value
//  88 = 2.9A on 308 LEDs
// 188 = 3.8A on 308 LEDs smart
//#define MAX_POWER	(127)

// Which input switch pin on the Arduino is connected to the NeoPixels?
// Version 1 = 2
// Version 2 = 1
// Version 3 = 6
// LOLIN S2 Mini = 18
#define NEOPIXEL_PIN  6	

// Which input switches the power onto the NeoPixels power?
// Version 3 = 5
#define STRIP_POWER 5	    		

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 10
// Version 2 = 10
// Version 3,5 = 10
// LOLIN S2 Mini = 15
#define STATUS_LED_PIN 10

// Red LED Pin to show status  9 Is blue, 10 is red
// Version 1 = 9
// Version 2 = 9
// Version 3 = 9
// LOLIN S2 Mini = 0
#define CONTROL_SWITCH_PIN 9

const char* MANUFACTURER = "Secure Hub";

// Make the following unique for each device (Bridge and paring setup code)
// Default Paring Code "46637726"
//const char* SERIAL_NO	= "JRM.008.2";		// Home bedroom LEDS = 108
//const char* BRIDGE_NAME = "SH Bridge 01";		
//const char* PARING_CODE = "88880001";	
//#define PIXEL_COUNT 108

//const char* BRIDGE_NAME = "SH Bridge 02";		// Flat floor bed 300 LEDS=300
//const char* PARING_CODE = "88880002";


//const char* BRIDGE_NAME = "SH Bridge 03";
//const char* PARING_CODE = "88880003";

//const char* SERIAL_NO	= "JRM.008.3";		// Big strip back wall and mirror
//const char* BRIDGE_NAME = "SH Lego 01";	// Claudia's Lego #1
//const char* PARING_CODE = "88880010";
//#define PIXEL_COUNT 600

//const char* BRIDGE_NAME = "Stranger Things";	// Claudia's Lego #2 Stranger times
//const char* PARING_CODE = "88880011";

//const char* BRIDGE_NAME = "Farm Strip";		// Farm bed head light
//const char* PARING_CODE = "88880012";

//const char* BRIDGE_NAME = "Lego Workshop";	// Lego workshop
//const char* PARING_CODE = "88880015";

//const char* SERIAL_NO	  = "JRM.008.3";		// Chimney
//const char* BRIDGE_NAME = "Chimney";			
//const char* PARING_CODE = "88880016";
//const char* SERIAL_NO   = "JRM.008.17";
//const char* BRIDGE_NAME = "KitchenCabinet";	// Kitchen Cabinet
//const char* PARING_CODE = "88880017";

//const char* SERIAL_NO   = "JRM.008.19";	// Master Bedroom Top Lights
//const char* BRIDGE_NAME = "Christmas Lights";	
//const char* PARING_CODE = "88880019";

//const char* SERIAL_NO   = "JRM.008.20";	// Farm Master bedroom
//const char* BRIDGE_NAME = "Farm Room Lights";	
//const char* PARING_CODE = "88880020";

//const char* SERIAL_NO   = "JRM.008.21";	// Claudia's TV
//const char* BRIDGE_NAME = "Claudia's TV Lights";	
//const char* PARING_CODE = "88880021";

//const char* SERIAL_NO   = "JRM.008.23";	// Farm Curtain TV
//const char* BRIDGE_NAME = "Farm Master Curtain 2";	
//const char* PARING_CODE = "88880023";

// const char* SERIAL_NO   = "JRM.008.25"; 	// Master bedroom curtain
// const char* BRIDGE_NAME = "Master Curtain 1";
// const char* PARING_CODE = "46637726";
// #define PIXEL_COUNT 300
// #define MAX_BRIGHTNESS 255

//const char* SERIAL_NO   = "JRM.008.26";	// Long 12V strip
//const char* BRIDGE_NAME = "Long 12V";	
//const char* PARING_CODE = "88880026";
//#define PIXEL_COUNT 600
//#define IS_W2815_12V

//const char* SERIAL_NO   = "JRM.008.27"; 	// Farm 12V strip
//const char* BRIDGE_NAME = "Long 12V";
//const char* PARING_CODE = "88880027";
//#define PIXEL_COUNT 600
//#define IS_W2815_12V 

const char* SERIAL_NO   = "JRM.008.28"; // Study S1
const char* BRIDGE_NAME = "Study S1";
const char* PARING_CODE = "88880028";
#define MAX_BRIGHTNESS 255
#define PIXEL_COUNT (160*5) // 141

// const char* SERIAL_NO   = "JRM.008.29"; // V6 5V #1 (Study S2)
// const char* BRIDGE_NAME = "HSV6 5V No1";
// const char* PARING_CODE = "88880029";
// #define MAX_BRIGHTNESS 255
// #define PIXEL_COUNT 300
// #define IS_W2815_12V 


extern Adafruit_NeoPixel g_strip;
extern void TurnOnStrip(bool on);