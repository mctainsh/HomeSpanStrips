#include "assert.h"
#pragma once

#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimable and colour changing
struct DEV_Base : Service::LightBulb
{
	SpanCharacteristic *POWER;	// reference to the On Characteristic
	SpanCharacteristic *V;		// reference to the Brightness Characteristic

	bool _powerOn = false;
	float _powerLevel = 20;

	float _lastBrightness = 0;	// Last saved brightness value

	unsigned long _timeOfPowerChange;
	bool _changeComplete = true;

	DEV_Base()
	  : Service::LightBulb()
	{
	}

	virtual void Show(bool startup = true)
	{
		assert(0);
	}

	bool PoweringOn()
	{
		return _powerOn && !_changeComplete;
	}

	bool PoweringOff()
	{
		return !_powerOn && !_changeComplete;
	}
	void PowerDown()
	{
		_changeComplete = true;
		_powerOn = false;
		POWER->setVal(false);
	}



	/////////////////////////////////////////////////////////////////////////////
	// Get the powerlevel for the
	void SetPowerlevel()
	{
		// Has a change started
		if (_changeComplete)
			return;

		double age = millis() - _timeOfPowerChange;

		// Proportion of age (0 to 1)
		float proportion = age / POWER_FADE_MS;
		float endValue = _powerLevel / 100.0 * MAX_BRIGHTNESS;

		// Apply the new level value
		float brightness;
		if (_powerOn)
			brightness = endValue * proportion;
		else
			brightness = 0; // This is messed up by shutdown login in main _min(_lastBrightness, (MAX_BRIGHTNESS * (1.0 - proportion)));

		g_strip.setBrightness((int)brightness);

		// Is the change complete
		if (age > POWER_FADE_MS)
		{
			_changeComplete = true;
			_lastBrightness = _powerOn ? endValue : 0;
			 g_strip.setBrightness(_lastBrightness);
			TurnOnStrip(_powerOn);
		}

		Show();
	}

	////////////////////////////////////////////////////////////////////////
	// Read the base setting
	void UpdateBase()
	{
		_powerOn = POWER->getVal();
		_powerLevel = V->getVal();
		if (POWER->updated())
		{
			_powerOn = POWER->getNewVal();
			_timeOfPowerChange = millis();
			_changeComplete = false;
		}

		if (V->updated())
		{
			_powerLevel = V->getNewVal();
			_changeComplete = false;
		}
	}
};
