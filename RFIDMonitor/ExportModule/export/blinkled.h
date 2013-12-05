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

#ifndef BLINKLED_H
#define BLINKLED_H

#include <QObject>

/*!
 * \brief The BlinkLed class access gpio interface functions to manipulate leds
 *
 * Create and configure access to the GPIO pins on raspberryPi. Offer functions to turn on/off and blink leds.
 */
class BlinkLed : public QObject
{
    Q_OBJECT
public:
    const static int KRedLed; // pinage used to red led
    const static int KGreenLed; // pinage used to green led

    /*!
     * \brief The Direction enum defines if pin will be used to read or write a value.
     */
    enum Direction {KInput = 0, KOutput};

    /*!
     * \brief BlinkLed class constructor prepare the access to the pins that correspond leds
     *
     * make access to which pins will be used, configure it's direction (only output in this case), write a zero value to turn  off leds and define led status with the same value.
     * \param parent
     */
    explicit BlinkLed(QObject *parent=0);


    static void writeLog(char *message);

    /*!
      * \brief BlinkLed destructor will release the access to the pins.
      */
    ~BlinkLed();

    /*!
     * \brief blinkRedLed turn on/off the red led
     * \param value zero turn off the led and non-zero turn it on
     */
    void blinkRedLed(int value);

    /*!
     * \brief blinkGreenLed turn on/off the green led
     * \param value zero turn off the led and non-zero turn it on
     */
    void blinkGreenLed(int value);

public slots:

    /*!
     * \brief blinkRedLedSlot will change the status of red led. if the led is on turns it off and vice-versa
     */
    void blinkRedLedSlot();
    /*!
     * \brief blinkGreenLedSlot will change the status of green led. if the led is on turns it off and vice-versa
     */
    void blinkGreenLedSlot();

private:
    QString m_module;
    int m_redLedState; // status of red led
    int m_greenLedState; // status of green led
};

#endif // BLINKLED_H
