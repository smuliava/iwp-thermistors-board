/*
 * Thermistor.cpp
 *
 *  Created on: Jun 8, 2025
 *      Author: smuli
 */

#include "Thermistor.h"
#include <tgmath.h>

#define ADC_RESOLUTION 4095.0
#define R_THERMISTOR_DEFAULT 10000.0
#define TEMP_25_KELVINS 298.15

// The Steinhart-Hart equation for thermistor resistance is:
// 1/T = A + B ln(R) + C [ln(R)]^3
//
// The simplified (beta) equation assumes C=0 and is:
// 1/T = A + (1/Beta) ln(R)
//
// The parameters that can be configured in RRF are R25 (the resistance at 25C), Beta, and optionally C.


Thermistor::Thermistor(volatile uint32_t *adcValue, string pinName, double rThermistor, double betta, double rBalance) {
	this->adcValue = adcValue;
	this->bettaParameter = betta;
	this->rBalance = rBalance;
	this->thermistorResistanceAt25 = rThermistor;
	this->bettaAt25 = betta * TEMP_25_KELVINS;
	this->setPinNameValue(pinName);
};

double Thermistor::getCurrentThermistorResistance() {
	const uint32_t adcValue = *this->adcValue;
	return this->rBalance * ((ADC_RESOLUTION / adcValue) - 1);
}

double Thermistor::getTempKelvin() {
	const double currentThermistorResistence = this->getCurrentThermistorResistance();
	return this->bettaAt25 / (this->bettaParameter + (TEMP_25_KELVINS * (log(currentThermistorResistence / this->thermistorResistanceAt25))));
}

double Thermistor::getTempCelsius() {
	return this->getTempKelvin() - 273.15;
}

void Thermistor::setThermistorResistanceAt25Value(const double value) {
	this->thermistorResistanceAt25 = value;
}

void Thermistor::setSeriesResistorValue(const double value) {
	this->rBalance = value;
}

void Thermistor::setBettaParameterValue(const double value) {
	this->bettaParameter = value;
}

void Thermistor::setCCoefficientValue(const double value) {
	this->cCoefficient = value;
}

void Thermistor::setPinNameValue(const string value) {
	this->pinName = value;
}

const string Thermistor::getPinNameValue() {
	return this->pinName;
}

void Thermistor::updateTemperature() {
	this->lastKnowTemperatureC = this->getTempCelsius();
}

double Thermistor::getLastKnownTemperatureC() {
	return this->lastKnowTemperatureC;
}

void Thermistor::setSensorNumberValue(const uint8_t value) {
	this->sensorNumber = value;
	this->isInitialized = true;
}

const uint8_t Thermistor::getSensorNumberValue() {
	return this->sensorNumber;
}


