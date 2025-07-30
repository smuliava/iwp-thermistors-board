/*
 * Thermistor.h
 *
 *  Created on: Jun 8, 2025
 *      Author: smuli
 */

#ifndef SRC_HEATING_SENSORS_THERMISTOR_H_
#define SRC_HEATING_SENSORS_THERMISTOR_H_

#include <stdint.h>
#include <string>
using namespace std;

#define BETTA_DEFAULT 3977.0
#define R_BALANCE_DEFAULT 10000.0
#define R_THERMISTOR_DEFAULT 10000.0

class Thermistor {
public:
	Thermistor(volatile uint32_t *adcValue, string pinName, double rThermistor = R_THERMISTOR_DEFAULT, double betta = BETTA_DEFAULT, double rBalance = R_BALANCE_DEFAULT);

	double getTempKelvin();
	double getTempCelsius();
	double getLastKnownTemperatureC();

	void setThermistorResistanceAt25Value(double value);

	void setSeriesResistorValue(double value);

	void setBettaParameterValue(double value);

	void setCCoefficientValue(double value);

	void setSensorNumberValue(uint8_t value);

	const uint8_t getSensorNumberValue();

	void updateTemperature();


	const string getPinNameValue();
	bool isInitialized = false;


private:
	double bettaAt25;
	double thermistorResistanceAt25;
	double rBalance;
	double bettaParameter;
	double cCoefficient;
	uint8_t sensorNumber;
	string pinName;
	double lastKnowTemperatureC;


	volatile uint32_t *adcValue;

	double getCurrentThermistorResistance();
	void setPinNameValue(string value);
};

#endif /* SRC_HEATING_SENSORS_THERMISTOR_H_ */
