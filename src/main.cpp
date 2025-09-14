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

float _finalPowerLevel = 0;		  // Power level to move toward
float _currentPowerLevel = 0;	  // Powerlevel we have currently set
//unsigned long _timeOfPowerChange; // Time of the last change in power level

#include <Adafruit_NeoPixel.h>
#include "HomeSpan.h"
#include "DEV_RainbowStrip.h"
#include "DEV_RgbLED.h"
#include "Globals.h"
#include "Credentails.h"

Adafruit_NeoPixel g_strip(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool _powerOn = false;	// Strip power is on
bool _firstLoop = true; // First loop after setup

DEV_RainbowStrip *_pRainbowStrip = NULL;
DEV_RgbLED *_pRgbStrip = NULL;

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
	esp_log_level_set("*", ESP_LOG_VERBOSE); // Set log level to INFO for all components

	// Disable the watchdog timer
	// esp_task_wdt_delete(NULL); // Delete the default task watchdog
	// esp_task_wdt_deinit();	   // Deinitialize the watchdog timer

	Serial.begin(115200); // Initialize serial communication for debugging
	delay(100);			  // Wait for a short time to ensure the serial connection is established
	Serial.setDebugOutput(true);
	Serial.printf("Starting %s. Cores:%d\n", MY_VERSION, configNUM_CORES);

	// Record if WDT tripped
	esp_reset_reason_t reason = esp_reset_reason();
	Serial.printf("Watch Dog Timer (WDT) RESET Reason : %d"); // ESP_RST_TASK_WDT means task watchdog triggered

	// Strip setup
	g_strip.begin();		  // INITIALIZE NeoPixel strip object (REQUIRED)
	g_strip.setBrightness(0); // Clear
	g_strip.clear();		  // ..
	g_strip.show();			  // Turn OFF all pixels ASAP

	// If the last restart was brown out, turn off the strip for 5 seconds
	if (reason == ESP_RST_BROWNOUT || reason == ESP_RST_PWR_GLITCH || reason == ESP_RST_POWERON)
	{
		Serial.println("Brownout detected, waiting 10 seconds before starting");
		pinMode(STATUS_LED_PIN, OUTPUT);
		const int BLINK_COUNT = 20;
		for (int n = 1; n < BLINK_COUNT; n++)
		{
			Serial.printf("Blink %d\n", n);
			digitalWrite(STATUS_LED_PIN, HIGH);
			delay((BLINK_COUNT - n) * 50);
			digitalWrite(STATUS_LED_PIN, LOW);
			delay(n * 50);
		}
	}

	TurnOnStrip(true);
	g_strip.show();
	g_strip.setBrightness(10); // Set BRIGHTNESS to about 5% (max = 255)

	// One red pixel
	Set(1, 255, 0, 0);
	delay(250);

	//// Testing
	//// const int PIN_COUNT = 143;
	// const int PIN_COUNT = 300;
	// int n = 0;
	// while (true)
	// {
	// 	g_strip.setBrightness(MIN(255, n)); // Set BRIGHTNESS to about 1/5 (max = 255)
	// 	int nV = analogRead(1);
	// 	float volts = 2 * nV * 3.3 / 4095.0; // Convert to voltage
	// 	Serial.printf("Analog read from GPIO 1: %d %d %.2f V                           %d\n", n, nV, volts, millis());

	// 	// Blink the status LED
	// 	Set(n++, 255, 255, 255);
	// 	delay(50);

	// 	if (n > PIN_COUNT || volts < 3.5)
	// 	{
	// 		n--;
	// 		while (n > 0)
	// 		{
	// 			Set(--n, 0, 0, 0);
	// 			delay(10);
	// 		}
	// 	}
	// }

	// Setup WiFi credentials
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to WiFi ..");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print('.');
		Set(2, 255, 255, 0);
		delay(500);
		Set(2, 255, 0, 255);
		delay(500);
	}

	// Setup homespan defaults
	homeSpan.setStatusPin(STATUS_LED_PIN);		// 9 Is blue, 10 is red
	homeSpan.setStatusAutoOff(30);				// Turn off status LED after 30 seconds
	homeSpan.setControlPin(CONTROL_SWITCH_PIN); // 18 is nearest GND, 9 is PRG Button

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

	// Turn Off the strip if nothing is on
	delay(250);
	if (!_pRainbowStrip->_powerOn && !_pRgbStrip->_powerOn)
	{
		Serial.println("Nothing is on. Turn off the strip");
		TurnOnStrip(false);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Main loop
void loop()
{
	homeSpan.poll();

	// Skip if we are not ready
	if (_pRainbowStrip == NULL || _pRgbStrip == NULL)
		return;

	// If one is powering on power off the current one
	if (_pRainbowStrip->IsPoweringOn() && _pRgbStrip->IsPoweringOn())
	{
		Serial.println("Both strips powering on. Power off RGB strip");
		_pRgbStrip->ForcePowerDown();
	}
	else if (_pRainbowStrip->IsPoweringOn())
	{
		Serial.println("Rainbow strip powering on. Power off RGB strip");
		_pRgbStrip->ForcePowerDown();
	}
	else if (_pRgbStrip->IsPoweringOn())
	{
		Serial.println("RGB strip powering on. Power off Rainbow strip");
		_pRainbowStrip->ForcePowerDown();
	}

	delay(50);

	// Set the power level for one of the strips
	if( !_pRainbowStrip->SetPowerlevel() )
		_pRgbStrip->SetPowerlevel();

	if (_pRainbowStrip->_powerOn || _pRgbStrip->_powerOn)
		TurnOnStrip(true);

	// Draw the strip
	if (_pRainbowStrip->_powerOn)
	{
		_pRainbowStrip->Show();
	}
	else if (_pRgbStrip->_powerOn)
	{
		_pRgbStrip->Show(false);
	}

	// Set the strip off if the current power level is zero
	TurnOnStrip((_currentPowerLevel > 0));
}

/////////////////////////////////////////////////////////////////////////////////////////////
void TurnOnStrip(bool on)
{
	if (_powerOn == on)
		return;
	pinMode(STRIP_POWER, on ? OUTPUT : INPUT);

	Serial.println(on ? "Turn ON" : "Turn OFF");

	if (on)
		digitalWrite(STRIP_POWER, HIGH);
	_powerOn = on;
}
