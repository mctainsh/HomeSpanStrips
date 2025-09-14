#include "assert.h"
#pragma once

#include "esp32-hal.h"
#include "Arduino.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dimmable and colour changing
struct DEV_Base : Service::LightBulb
{
	SpanCharacteristic *POWER; // reference to the On Characteristic
	SpanCharacteristic *V;	   // reference to the Brightness Characteristic

	bool _powerOn = false;

	bool _changeComplete = true;
	const char *_name = "Base";

	DEV_Base()
		: Service::LightBulb()
	{
	}

	virtual void Show(bool startup = true)
	{
		assert(0);
	}

	/////////////////////////////////////////////////////////////////////////
	// Check if this light is in the process of powering. This must be called
	// .. before SetPowerLevel() incase _changeComplete get cleared too
	// .. soon. Used to prevent both lights being on at the same time
	bool IsPoweringOn()
	{
		return !_changeComplete && _powerOn;// _currentPowerLevel < _finalPowerLevel;
	}

	////////////////////////////////////////////////////////////////////////
	// Power down the light, Used when one light is already on and we want to
	// .. power down the other light
	void ForcePowerDown()
	{
		Serial.printf("ForcePowering down %s %d\n", _name, POWER->getVal());
		_changeComplete = true;
		_powerOn = false;
	
		// Tell Apple it is powered off
		POWER->setVal(false);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Set the correct power level for the strip. 
	//	Return true if power level processed
	bool SetPowerlevel()
	{
		float voltage = GetVoltage();

		// Has a change completed and power OK
		if (_changeComplete && IsVoltageOK(voltage))
			return false;

		Serial.printf("Set power %.0f%% -> %.0f%% %fV\n", _currentPowerLevel, _finalPowerLevel, voltage);

		if (_currentPowerLevel > _finalPowerLevel)
		{
			// Dropping voltage
			_currentPowerLevel -= 5; // Decrease the power level by 2%
			Serial.printf("1 %.0f%% -> %.0f%%\n", _currentPowerLevel, _finalPowerLevel);
			_currentPowerLevel = MAX( 0, _currentPowerLevel);
			_currentPowerLevel = MAX( _finalPowerLevel, _currentPowerLevel);
			Serial.printf("1 %.0f%% -> %.0f%%\n", _currentPowerLevel, _finalPowerLevel);
		}
		else
		{
			// Going up or already set
			if (!IsVoltageOK(voltage))
			{
				Serial.printf("Pre-check : Voltage too low. Stopping power UP\n");
				_currentPowerLevel -= 5; // Decrease the power level by 10%
				if (_currentPowerLevel < 0)
					_currentPowerLevel = 0;
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

		//// Turn off power if we are at zero
		//TurnOnStrip((_currentPowerLevel > 0));

		// Set the strip brightness
		g_strip.setBrightness((int)(_currentPowerLevel / 100.0 * MAX_BRIGHTNESS));

		Show();
		return true;
	}

	////////////////////////////////////////////////////////////////////////
	// Check the voltage and return true if it is okay
	static bool IsVoltageOK(float volts)
	{
		if (volts > 5.0)  // 4.5V is the minimum voltage for the strip
			return true;

		Serial.printf("Power too low %.2fV\n", volts);
		return false;
	}

	////////////////////////////////////////////////////////////////////////
	// Read the voltage
	static float GetVoltage()
	{
		int nV = analogRead(VOLTAGE_SENSE_PIN);
		return 2 * nV * 3.3 / 4095.0; // Convert to voltage
	}

	////////////////////////////////////////////////////////////////////////
	// Called when updates change. Read the base setting
	void UpdateBase()
	{
		_powerOn = POWER->getVal();

		Serial.printf("Power: %s\n", _name);
		Serial.printf("\tNow : %s, Level: %.0f%%\n", _powerOn ? "ON" : "OFF", _currentPowerLevel);
		if (POWER->updated())
		{
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
			_finalPowerLevel = V->getNewVal();
			_changeComplete = false;
			Serial.printf("\tTo  : %s, Level: %.0f%%\n", _powerOn ? "ON" : "OFF", _finalPowerLevel);
		}
	}
};
