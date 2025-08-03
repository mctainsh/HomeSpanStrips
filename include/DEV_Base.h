#include "assert.h"
#pragma once

#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"

// extern float _currentPowerLevel = 0;
// extern unsigned long _timeOfPowerChange;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimmable and colour changing
struct DEV_Base : Service::LightBulb
{
	SpanCharacteristic *POWER; // reference to the On Characteristic
	SpanCharacteristic *V;	   // reference to the Brightness Characteristic

	bool _powerOn = false;
	float _finalPowerLevel = 0; // Power level to move toward

	float _currentBrightness = 0;	// Last saved brightness value
	float _lastGoodBrightness = 25; // Last good brightness value

	bool _changeComplete = true;
	const char *_name = "Base";

	// unsigned int _powerDownCountTimeout = 0; // Timeout for power down. ONly call power time

	DEV_Base()
		: Service::LightBulb()
	{
	}

	virtual void Show(bool startup = true)
	{
		assert(0);
	}

	bool IsPoweringOn()
	{
		return _powerOn && !_changeComplete;
	}

	bool IsPoweringOff()
	{
		return !_powerOn && !_changeComplete;
	}

	void PowerDown()
	{
		Serial.printf("Powering down %s %d\n", _name, POWER->getVal());
		_changeComplete = true;
		_powerOn = false;
		//_currentPowerLevel = 0;
		_finalPowerLevel = 0;
		// if(POWER->getVal())

		// Tell Apple it is powered off
		POWER->setVal(false);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Get the powerlevel for the
	void SetPowerlevel()
	{
		float voltage = GetVoltage();

		// Has a change started
		if (_changeComplete || !CheckVoltage(voltage))
			return;

		Serial.printf("Set power %.0f%% -> %.0f%% %fV\n", _currentPowerLevel, _finalPowerLevel, voltage);

		if (_currentPowerLevel > _finalPowerLevel)
		{
			// Dropping voltage
			_currentPowerLevel -= 1;
			Serial.printf("1 %.0f%% -> %.0f%%\n", _currentPowerLevel, _finalPowerLevel);
			if (_currentPowerLevel < _finalPowerLevel)
				_currentPowerLevel = _finalPowerLevel;
			Serial.printf("1 %.0f%% -> %.0f%%\n", _currentPowerLevel, _finalPowerLevel);
		}
		else
		{
			// Going up or allready set
			if (!CheckVoltage(voltage))
			{
				Serial.printf("Pre-check : Voltage too low. Stopping power UP\n");
				_currentPowerLevel -= 5; // Decrease the power level by 10%
				_currentPowerLevel -= 5; // Decrease the power level by 10%
				if (_currentPowerLevel < 0)
					_currentPowerLevel = 1;
				_changeComplete = true;
			}

			// Increasing voltage
			_currentPowerLevel += 1;
			if (_currentPowerLevel > _finalPowerLevel)
				_currentPowerLevel = _finalPowerLevel;
		}

		// Check if we have reached the final power level
		if (_currentPowerLevel == _finalPowerLevel)
			_changeComplete = true;

		// Turn off power if we are at zero
		TurnOnStrip((_currentPowerLevel > 0));

		// Settled in voltage check
		g_strip.setBrightness((int)(_currentPowerLevel / 100.0 * MAX_BRIGHTNESS));

		// Not sure this will get caught
		if (!CheckVoltage(GetVoltage()))
		{
			Serial.printf("Post-check : Voltage too low. Stopping power UP\n");
			_currentPowerLevel -= 10; // Increase the power level by 10%
			_changeComplete = true;
			g_strip.setBrightness((int)(_currentPowerLevel / 100.0 * MAX_BRIGHTNESS));
		}

		Show();

		// unsigned long age = millis() - _timeOfPowerChange;

		// // Proportion of age (0 to 1)
		// float proportion = (float)age / POWER_FADE_MS;
		// float endValue = _finalPowerLevel / 100.0 * MAX_BRIGHTNESS;

		// // Apply the new level value
		// float brightness;
		// if (_powerOn)
		// 	brightness = endValue * proportion;
		// else
		// 	brightness = 0; // This is messed up by shutdown login in main _min(_lastBrightness, (MAX_BRIGHTNESS * (1.0 - proportion)));

		// // Check the CPU power supply voltage before setting the brightness up
		// bool clipped = false;
		// if (_powerOn)
		// {
		// 	int nV = analogRead(1);
		// 	float volts = 2 * nV * 3.3 / 4095.0; // Convert to voltage
		// 	Serial.printf("Analog read from GPIO 1: %d %.2fV\n", nV, volts);
		// 	if (volts < 4.0)
		// 	{
		// 		Serial.printf("Power too low %.2fV, setting brightness to 0\n", volts);
		// 		brightness = _lastGoodBrightness;
		// 		clipped = true;
		// 	}
		// 	else
		// 	{
		// 		_lastGoodBrightness = MAX( 25, _currentBrightness);
		// 	}
		// }

		// g_strip.setBrightness((int)brightness);
		// _currentBrightness = brightness;

		// // Is the change complete
		// if (age > POWER_FADE_MS)
		// {
		// 	_changeComplete = true;
		// 	float lastBrightness = _powerOn ? endValue : 0;
		// 	g_strip.setBrightness( clipped ? _lastGoodBrightness : lastBrightness);
		// 	TurnOnStrip(_powerOn);
		// }

		// Show();
	}

	////////////////////////////////////////////////////////////////////////
	// Check the voltage and return true if it is okay
	bool CheckVoltage(float volts)
	{
		if (volts > 4.5)
			return true;

		Serial.printf("Power too low %.2fV\n", volts);
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	// Read the voltage
	float GetVoltage()
	{
		int nV = analogRead(1);
		return 2 * nV * 3.3 / 4095.0; // Convert to voltage
	}

	////////////////////////////////////////////////////////////////////////
	// Read the base setting
	void UpdateBase()
	{
		_powerOn = POWER->getVal();
		//_finalPowerLevel = V->getVal();
		Serial.printf("Power: %s\n", _name);
		Serial.printf("\tNow : %s, Level: %.0f%%\n", _powerOn ? "ON" : "OFF", _currentPowerLevel);
		if (POWER->updated())
		{
			_timeOfPowerChange = millis();
			_powerOn = POWER->getNewVal();
			if (_powerOn)
				_finalPowerLevel = V->getNewVal();
			else
				_finalPowerLevel = 0;
			_changeComplete = false;
			Serial.printf("\tTo  : %s, Level: %.0f%%\n", _powerOn ? "ON" : "OFF", _finalPowerLevel);
		}

		if (V->updated())
		{
			_timeOfPowerChange = millis();
			_finalPowerLevel = V->getNewVal();
			_changeComplete = false;
			Serial.printf("\tTo  : %s, Level: %.0f%%\n", _powerOn ? "ON" : "OFF", _finalPowerLevel);
		}
	}
};
