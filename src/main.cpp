#include <Arduino.h>

/*********************************************************************************
   Setup notes
    1) Setup WIFI credentials
 	2) Increase Partition size with Tools -> Partition size -> Minimal Spiffs (Or Huge APP)
	3) Clear memory (ONLY IF NEW). Tools -> Erase All Flast before upload -> Enable 
	4) Set Library versions
		HomeSpan 2.1.0
		Adafruit_NeoPixel 1.12.4

	TODO Edit HomeSpan.cpp
		#include <mutex>   // JRM Fix IDE change error
		#include <algorithm>

 ********************************************************************************/

#include <Adafruit_NeoPixel.h>
#include "HomeSpan.h"
#include "DEV_RainbowStrip.h"
#include "DEV_RgbLED.h"
#include "Globals.h"
#include "Credentails.h"

Adafruit_NeoPixel g_strip(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

boolean _powerOn = false;

DEV_RainbowStrip* _pRainbowStrip = NULL;
DEV_RgbLED* _pRgbStrip = NULL;

///////////////////////////////////////////////////////////////////////////////////////
// Turn on a single pixel
void Set(int pin, int r, int g, int b)
{
	g_strip.setPixelColor(pin, g_strip.Color(r, g, b));
	g_strip.show();
}

///////////////////////////////////////////////////////////////////////////////////////
// Make a number from 0 to 255 that pulses based on time
int MakePulseColour(int offset = 0)
{
	int c = ((millis() / 5) + offset) % 512;
	return (c > 256) ? 512 - c : c;
}

///////////////////////////////////////////////////////////////////////////////////////
// Startup
void setup()
{
	Serial.begin(115200);

	TurnOnStrip(true);
	g_strip.begin();						// INITIALIZE NeoPixel strip object (REQUIRED)
	g_strip.show();							// Turn OFF all pixels ASAP
	//g_strip.setBrightness(25);	// Set BRIGHTNESS to about 1/5 (max = 255)
	g_strip.setBrightness(150);	// Set BRIGHTNESS to about 1/5 (max = 255)

	Set(1, 255, 0, 0);
	delay(250);

	//	pinMode(STATUS_LED_PIN, OUTPUT);
	int n = 0;
	while(true)
	{
		int nV = analogRead(1);
		float volts = 2 * nV * 3.3 / 4095.0; // Convert to voltage
		Serial.printf("Analog read from GPIO 1: %d %d %.2f V                           %d\n", n, nV, volts, millis());

		// Blink the status LED
		Set(n++, 255, 255, 255);
		delay(100);

		if( n > 143)
		{
			n--;
			while( n > 0)
			{
				Set(--n, 0, 0, 0);
				delay(10);
			}
		}
	}

	// Setup homespan defaults
	homeSpan.setStatusPin(STATUS_LED_PIN);		 // 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);				 // Turn off status LED after 30 seconds
	homeSpan.setControlPin(CONTROL_SWITCH_PIN);	 // 18 is nearest GND, 9 is PRG Button

	// Start the bridge
	homeSpan.begin(Category::Bridges, BRIDGE_NAME);
	
	// Setup the parting code (Should be unique on the network)
	homeSpan.setPairingCode(PARING_CODE);

	Set(2, 0, 255, 0);
	delay(250);

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Manufacturer(MANUFACTURER);
	new Characteristic::FirmwareRevision(MY_VERSION);
	new Characteristic::Model("HomeSpanV6");

	Set(3, 0, 0, 255);
	delay(250);

	// Create a Dimmable Rainbow strip
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Rainbow LED");
	_pRainbowStrip = new DEV_RainbowStrip();

	Set(4, 255, 255, 0);
	delay(250);

	// Create simple colour strip
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Identify();
	new Characteristic::Name("Light strip");
	_pRgbStrip = new DEV_RgbLED();

	Set(5, 255, 0, 255);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Main loop
void loop()
{
	homeSpan.poll();

	// Skip if we are not ready
	if (_pRainbowStrip == NULL || _pRgbStrip == NULL)
		return;

	// If one is powering on and one off
	// .. Cancel the power off and accept power ON
	if (_pRainbowStrip->PoweringOn() && _pRgbStrip->PoweringOn())
		_pRgbStrip->PowerDown();

	if (_pRainbowStrip->PoweringOn())
		_pRgbStrip->PowerDown();

	if (_pRgbStrip->PoweringOn())
		_pRainbowStrip->PowerDown();

	delay(50);
	//if( _pRainbowStrip->PoweringOn() && _pRgbStrip->PoweringOn())
	//	_pRgbStrip-PowerDown();


	// Set the power level
	_pRainbowStrip->SetPowerlevel();
	_pRgbStrip->SetPowerlevel();

	if (_pRainbowStrip->_powerOn || _pRgbStrip->_powerOn)
		TurnOnStrip(true);

	// Turn off status lights at startup
	if (!_pRainbowStrip->_powerOn && !_pRgbStrip->_powerOn && millis() > 5000)
	{
		_pRainbowStrip->PowerDown();
		_pRgbStrip->PowerDown();
		TurnOnStrip(false);
	}

	// Draw the strip
	if (_pRainbowStrip->_powerOn)
	{
		_pRainbowStrip->Show();
	}
	else if (_pRgbStrip->_powerOn)
	{
		_pRgbStrip->Show(false);
	}


	// If stuff is still pending blink
	//Set(0, MakePulseColour(), 0, 0);


	//Set(1, _pRainbowStrip->_powerOn ? 255 : 0, 0, 0);
	//Set(2, _pRgbStrip->_powerOn ? 255 : 0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void TurnOnStrip(bool on)
{
	if (_powerOn == on)
		return;
	pinMode(STRIP_POWER, on ? OUTPUT : INPUT);

	Serial.println(on ? "Turn OFF" : "Turn OFF");

	if (on)
		digitalWrite(STRIP_POWER, HIGH);
	_powerOn = on;
}
