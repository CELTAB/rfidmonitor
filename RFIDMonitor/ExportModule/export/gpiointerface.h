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

#ifndef GPIOINTERFACE_H
#define GPIOINTERFACE_H

/*
   \class GPIOInterface configure a sysfs user interface to GPIOs.

   Provides control over GPIO direction and value, so is possible configure which pins will be used and for what purpose.
   To get GPIO control is necessary some steps:

   - The kernel linux have the control of GPIO, so is necessary export control to userspace:
        The control interfaces are write-only:
          /sys/class/gpio/

            "export" ... Userspace may ask the kernel to export control of
            a GPIO to userspace by writing its number to this file.

                Example:  "echo 19 > export" will create a "gpio19" node
                for GPIO #19, if that's not requested by kernel code.

    - To return control to kernel:

            "unexport" ... Reverses the effect of exporting to userspace.

                Example:  "echo 19 > unexport" will remove a "gpio19"
                node exported using the "export" file.

    - To use a expecific GPIO pin, is necessary define if will be use for input or output using signals:
        GPIO signals have paths like /sys/class/gpio/gpio42/ (for GPIO #42) and have the following read/write attributes:

        /sys/class/gpio/gpioN/

            "direction" ... reads as either "in" or "out".  This value may
            normally be written.  Writing as "out" defaults to
            initializing the value as low (0).  To ensure glitch free
            operation, values "low" and "high" may be written to
            configure the GPIO as an output with that initial value.

            Note that this attribute *will not exist* if the kernel
            doesn't support changing the direction of a GPIO, or
            it was exported by kernel code that didn't explicitly
            allow userspace to reconfigure this GPIO's direction.

    - To use a GPIO pin:

            "value" ... reads as either 0 (low) or 1 (high).  If the GPIO
            is configured as an output, this value may be written;
            any nonzero value is treated as high.

      */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief GPIOExport export the control to userspace then the GPIO pin can be used
 * \param pin - number that represents the GPIO pin
 * \return zero if export successfully, non-zero otherwise
 */
int GPIOExport(int pin);

/*!
 * \brief GPIOUnexport return the control of GPIO pin to the kernel linux
 * \param pin - number that represents the GPIO pin
 * \return zero if export successfully, non-zero otherwise
 */
int GPIOUnexport(int pin);

/*!
 * \brief GPIODirection define if the GPIO pin will be used to input or output
 * \param pin - number that represents the GPIO pin
 * \param dir - value that represents the direction, zero(0) to input and one(1) to output
 * \return zero if direction set successfully, non-zero otherwise
 */
int GPIODirection(int pin, int dir);

/*!
 * \brief GPIOWrite class that will set value to GPIO pin
 * \param pin - number that represents the GPIO pin
 * \param value to set to pin (zero (0) or one (1))
 * \return zero if write value successfully, non-zero otherwise
 */
int GPIOWrite(int pin, int value);

void setLogger(void(*logger)(char *str));

#ifdef __cplusplus
}
#endif


#endif // GPIOINTERFACE_H
