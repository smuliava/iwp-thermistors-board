/*
 * Thermistor.h
 *
 *  Created on: Jun 8, 2025
 *      Author: smuli
 */

#ifndef SRC_HEATING_SENSORS_THERMISTOR_H_
#define SRC_HEATING_SENSORS_THERMISTOR_H_

#include <stdint.h>

#define BETTA_DEFAULT 3977.0
#define R_BALANCE_DEFAULT 10000.0

class Thermistor {
public:
	Thermistor(volatile uint32_t *adcValue, double rThermistor, double betta = BETTA_DEFAULT, double rBalance = R_BALANCE_DEFAULT);
	double getTempKelvin();
	double getTempCelsius();

private:
	double bettaAt25;
	double rThermistor;
	double rBalance;
	double betta;
	volatile uint32_t *adcValue;

	double getCurrentThermistorResistance();
};

#endif /* SRC_HEATING_SENSORS_THERMISTOR_H_ */
