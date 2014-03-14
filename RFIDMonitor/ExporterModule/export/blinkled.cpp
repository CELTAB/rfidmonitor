/****************************************************************************
**
** WWW.FISHMONITORING.COM.BR
**
** Copyright (C) 2013
**                     Gustavo Valiati <gustavovaliati@gmail.com>
**                     Luis Valdes <luisvaldes88@gmail.com>
**                     Thiago R. M. Bitencourt <thiago.mbitencourt@gmail.com>
**
** This file is part of the FishMonitoring project
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; version 2
** of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QDebug>
#include <logger.h>

#include "blinkled.h"
#include "gpiointerface.h"

const int BlinkLed::KRedLed = 11; // define GPIO pin 11 for red led
const int BlinkLed::KGreenLed = 24; // define GPIO pin 24 for green led

BlinkLed::BlinkLed(QObject *parent) :
    QObject(parent)
{
    m_module = "ExportModule";

    setLogger(&writeLog);

    GPIOExport(KRedLed);
    GPIOExport(KGreenLed);
    GPIODirection(KRedLed, KOutput);
    GPIODirection(KGreenLed, KOutput);
    GPIOWrite(KRedLed, 0);
    GPIOWrite(KGreenLed, 0);

    m_redLedState = 0;
    m_greenLedState = 0;
}

BlinkLed::~BlinkLed()
{
    // turn off leds
    GPIOWrite(KRedLed, 0);
    GPIOWrite(KGreenLed, 0);

    // release access to leds
    GPIOUnexport(KRedLed);
    GPIOUnexport(KGreenLed);
}

void BlinkLed::writeLog(char *message)
{
    Logger::instance()->writeRecord(Logger::error, "ExportModule", "Error from GPIOInterface.c", QString::fromLocal8Bit(message));
}

void BlinkLed::blinkRedLed(int value)
{
    GPIOWrite(KRedLed, value);
}

void BlinkLed::blinkGreenLed(int value)
{
    GPIOWrite(KGreenLed, value);
}

void BlinkLed::blinkRedLedSlot()
{
    // blink red led. change the current status and write value
    m_redLedState = !m_redLedState;
    GPIOWrite(KRedLed, m_redLedState);
}

void BlinkLed::blinkGreenLedSlot()
{
    // blink green led. change the current status and write value
    m_greenLedState = !m_greenLedState;
    GPIOWrite(KGreenLed, m_greenLedState);
}


