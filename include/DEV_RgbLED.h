#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"

// How often the strip should be refreshed
#define REFRESH_MS 5000

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimable and colour changing
struct DEV_RgbLED : DEV_Base
{
	SpanCharacteristic *H = NULL;
	SpanCharacteristic *S = NULL;
	uint32_t Colour = 0;

	ulong _refreshStripTime = 0;

	DEV_RgbLED()
	  : DEV_Base()
	{
		POWER = new Characteristic::On(false, true);
		H = new Characteristic::Hue(0, true);			// instantiate the Hue Characteristic with an initial value of 0 out of 360
		S = new Characteristic::Saturation(0, true);	// instantiate the Saturation Characteristic with an initial value of 0%
		V = new Characteristic::Brightness(100, true);	// instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(5, 100, 1);							// sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
		//Characteristic::Selector effect{ 1, true };
		//SetColour(H->getVal(), S->getVal());
		update();
	}

	void SetColour(float h, float s)
	{
		// Here we call a static function of LedPin that converts HSV to RGB.
		// Parameters must all be floats in range of H[0,360], S[0,1], and V[0,1]
		// R, G, B, returned [0,1] range as well
		//Colour = g_strip.ColorHSV((uint16_t)h, (uint8_t)(s / 100.0 * 255), 255);

		float r, g, b;
		LedPin::HSVtoRGB(h, s / 100.0, 1, &r, &g, &b);	// since HomeKit provides S and V in percent, scale down by 100
//LedPin::HSVtoRGB(h, s / 100.0, _powerLevel / 100.0, &r, &g, &b);  // since HomeKit provides S and V in percent, scale down by 100
//LedPin::HSVtoRGB(h, s / 100.0, 1, &r, &g, &b);	// since HomeKit provides S and V in percent, scale down by 100

// Swap RED and Green for W2815 (12V)
#ifdef IS_W2815_12V
		Colour = g_strip.Color(g * 255, r * 255, b * 255);
#else
		Colour = g_strip.Color(r * 255, g * 255, b * 255);
#endif

		_refreshStripTime = millis() - REFRESH_MS;
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Update the changed values
	boolean update()
	{
		UpdateBase();

		float h = H->getVal<float>();  // get and store all current values.  Note the use of the <float> template to properly read the values
		float s = S->getVal<float>();

		if (H->updated())
			h = H->getNewVal<float>();

		if (S->updated())
			s = S->getNewVal<float>();

		SetColour(h, s);

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Only update colour every 5 seconds
	void Show(bool startup = true) override
	{
		if (!startup && millis() - _refreshStripTime < REFRESH_MS)
			return;

		_refreshStripTime = millis();

		for (int n = 0; n < PIXEL_COUNT; n++)
			g_strip.setPixelColor(n, Colour);
		g_strip.show();
	}
};
