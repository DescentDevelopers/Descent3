/*
 * $Logfile: /DescentIII/main/NewPyroGauges.h $
 * $Revision: 5 $
 * $Date: 12/29/97 5:44p $
 * $Author: Samir $
 *
 *	New Pyro GX Gauge rendering functions
 *
 * $Log: /DescentIII/main/NewPyroGauges.h $
 *
 * 5     12/29/97 5:44p Samir
 * Took out references to grViewport and old 2d library.
 *
 * 4     11/14/97 5:30p Samir
 * Now we get more information about the gauge's world position.
 *
 * 3     11/11/97 1:27p Samir
 * Weapon gauges partially working.   Still needs work.
 *
 * 2     11/04/97 6:24p Samir
 * Semi functioning shield gauges.
 *
 * 1     10/30/97 3:27p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef NEWPYRO_H
#define NEWPYRO_H

struct tGauge;
struct tGaugePos;

#include "vecmat.h"

//	rendering functions

//	shield gauge handlers (one for number, one for status)
void NewPyroInitGauges();
void NewPyroShieldGauge(tGauge *gauge, tGaugePos *pos);
void NewPyroShipGauge(tGauge *gauge, tGaugePos *pos);
void NewPyroEnergyDigitalGauge(tGauge *gauge, tGaugePos *pos);
void NewPyroEnergyAnalogGauge(tGauge *gauge, tGaugePos *pos);
void NewPyroPrimaryWeaponGauge(tGauge *gauge, tGaugePos *pos);
void NewPyroSecondaryWeaponGauge(tGauge *gauge, tGaugePos *pos);

#endif