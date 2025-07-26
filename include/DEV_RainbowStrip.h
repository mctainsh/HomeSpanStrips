#include "DEV_Base.h"

///////////////////////////////////////////////////////////////////////////////////////
// Simple dimable LED
struct DEV_RainbowStrip : DEV_Base
{
	/////////////////////////////////////////////////////////////////////////////
	// Setup charactoristics
	DEV_RainbowStrip()
	  : DEV_Base()
	{
		Serial.println("Configuring Dimmable LED");	 // initialization message
		POWER = new Characteristic::On(false, true);
		V = new Characteristic::Brightness(100, true);	// instantiate the Brightness Characteristic with an initial value of 100%
		V->setRange(5, 100, 1);							// sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1%
		update();
	}

	/////////////////////////////////////////////////////////////////////////
	//Process change in settings
	boolean update()
	{
		UpdateBase();
		return true;  // return true
	}

	void Show(bool startup = true) override
	{
		int startHue = millis() * 5;
		g_strip.rainbow(startHue);
		g_strip.show();
	}
};