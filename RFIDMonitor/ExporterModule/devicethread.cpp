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


/*******************************************
 libudev example.

 This example prints out properties of
 each of the hidraw devices. It then
 creates a monitor which will report when
 hidraw devices are connected or removed
 from the system.

 This code is meant to be a teaching
 resource. It can be used for anyone for
 any reason, including embedding into
 a commercial product.

 The document describing this file, and
 updated versions can be found at:
    http://www.signal11.us/oss/udev/

 Alan Ott
 Signal 11 Software
 2010-05-22 - Initial Revision
 2010-05-27 - Monitoring initializaion
              moved to before enumeration.
*******************************************/

#include <QCoreApplication>
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <logger.h>

#include <QDebug>

#include <QProcess>
#include <QFileInfo>
#include <QRegularExpression>

#include "devicethread.h"
#include "export/exportlocaldata.h"

/*!
 * \brief DeviceThread is responsible for listening if a device is connected or disconnected.
 * When a device is connected it will verify and figure out the device's path and call a function to export data into the device.
 *
 * \param parent
 */
DeviceThread::DeviceThread(QObject *parent) :
    QObject(parent)
{
    setObjectName("DeviceThread");
}

DeviceThread *DeviceThread::instance()
{
    // if already exist a instance of this class, returns. otherwise get a new instance
    static DeviceThread *instance = 0;
    if(! instance){
        instance = new DeviceThread();
    }
    return instance;
}

/*!
 * \brief deviceAddedCallback function is called when a new device is connected in computer.
 * Also inspect all devices mounted in /media directory and verify if is possible to write into a device and emit the exportToDevice signal and pass the path by parameter.
 * If the device is writable, passes (like parameter) a path to device or passes an empty string.
 *
 * \sa exportToDevice
 */
void deviceAddedCallback(const char *)
{
    QString m_module("ExportModule");
    QFile m_mounts;
    m_mounts.setFileName("/proc/mounts");

    Logger::instance()->writeRecord(Logger::severity_level::info, m_module, Q_FUNC_INFO, "Device Detected");

    QString devicePath("");
    bool notMatched = true;

    // wait five(5) seconds before write data into device. A secure time to device be already mounted by the system
    QTimer timer;
    timer.start(5000);
    while(timer.remainingTime() > 0)
        ;

    // cat command
    QProcess catCom;
    catCom.start(QString("cat " + m_mounts.fileName()));
    catCom.waitForFinished(-1);

    QTextStream mountedDev(catCom.readAllStandardOutput());

    // regular expression to use only devices mounted in /media directory
    QRegularExpression regexCode;
    regexCode.setPattern("/dev/[a-z]{3}([0-9]{1})?\\s/media/(.*)\\s");
                //        /dev/sda /media/usb0 vfat rw

    QString line;
    while(!mountedDev.atEnd()){
        line = mountedDev.readLine();
        QRegularExpressionMatch match = regexCode.match(line);

        if(match.hasMatch()) {

            QString dev(match.captured(0));
            QStringList infoDevice = dev.split(" ");

            Logger::instance()->writeRecord(Logger::severity_level::info, m_module, Q_FUNC_INFO, QString("Inspectin device: %1").arg(infoDevice.at(1)));
            QFileInfo device(infoDevice.at(1));
            // check if the device found is writable
            if(device.isWritable()){
                Logger::instance()->writeRecord(Logger::severity_level::info, m_module, Q_FUNC_INFO, QString("Using device: %1. Mount point: %2. File System: %3").arg(infoDevice.at(0)).arg(infoDevice.at(1)).arg(infoDevice.at(2)));
                devicePath = infoDevice.at(1);
                notMatched = false;
            } else {
                Logger::instance()->writeRecord(Logger::severity_level::info, m_module, Q_FUNC_INFO, QString("%1 is not writable").arg(device.fileName()));
            }
        }
    }
    // If was not found any device in /media write an info log record
    if(notMatched){
        Logger::instance()->writeRecord(Logger::severity_level::info, m_module, Q_FUNC_INFO, QString("EXPORT ERROR: No media found to export data"));
        return;
    }

    // call function responsible to export data to device just connected
    emit DeviceThread::instance()->exportToDevice(devicePath);
}

/*!
 * \brief deviceRemovedCallback function is called when a device is disconnected of computer
 */
void deviceRemovedCallback()
{
    Logger::instance()->writeRecord(Logger::severity_level::info, "ExportModule", Q_FUNC_INFO, "Device Removed");
    // turn off green led when a device is removed
    emit DeviceThread::instance()->turnLedOff();
}

void writeLog(const char *str)
{
    Logger::instance()->writeRecord(Logger::severity_level::error, "ExportModule", "void start_listening", QString(str));
}

extern "C" {

void start_listening(void (*callback)(const char *), void (*rmCallback)(void), void (*logger)(const char *str))
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    struct udev_monitor *mon;
    int fd;

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        logger("Can't create udev");
        exit(1);
    }

    /* This section sets up a monitor which will report events when
   devices attached to the system change.  Events include "add",
   "remove", "change", "online", and "offline".

   This section sets up and starts the monitoring. Events are
   polled for (and delivered) later in the file.

   It is important that the monitor be set up before the call to
   udev_enumerate_scan_devices() so that events (and devices) are
   not missed.  For example, if enumeration happened first, there
   would be no event generated for a device which was attached after
   enumeration but before monitoring began.

   Note that a filter is added so that we only get events for
   "hidraw" devices. */

    /* Set up a monitor to monitor hidraw devices */
    mon = udev_monitor_new_from_netlink(udev, "udev");
    //    udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL);
    //    udev_monitor_filter_add_match_subsystem_devtype(mon, "usb", NULL);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "scsi_disk", NULL);
    udev_monitor_enable_receiving(mon);
    /* Get the file descriptor (fd) for the monitor.
   This fd will get passed to select() */
    fd = udev_monitor_get_fd(mon);


    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    /* For each item enumerated, print out its information.
   udev_list_entry_foreach is a macro which expands to
   a loop. The loop will be executed for each member in
   devices, setting dev_list_entry to a list entry
   which contains the device's path in /sys. */
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        /* Get the filename of the /sys entry for the device
       and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);


        /* The device pointed to by dev contains information about
       the hidraw device. In order to get information about the
       USB device, get the parent device with the
       subsystem/devtype pair of "usb"/"usb_device". This will
       be several levels up the tree, but the function will find
       it.*/
        dev = udev_device_get_parent_with_subsystem_devtype(
                    dev,
                    "usb",
                    "usb_device");
        if (!dev)
        {
            logger("Unable to find parent usb device.");
            exit(1);
        }
    }

    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    /* Begin polling for udev events. Events occur when devices
   attached to the system are added, removed, or change state.
   udev_monitor_receive_device() will return a device
   object representing the device which changed and what type of
   change occured.

   The select() system call is used to ensure that the call to
   udev_monitor_receive_device() will not block.

   The monitor was set up earler in this file, and monitoring is
   already underway.

   This section will run continuously, calling usleep() at the end
   of each pass. This is to demonstrate how to use a udev_monitor
   in a non-blocking way. */

    while (1) {
        /* Set up the call to select(). In this case, select() will
       only operate on a single file descriptor, the one
       associated with our udev_monitor. Note that the timeval
       object is set to 0, which will cause select() to not
       block. */
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        /* Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(fd, &fds)) {

            /* Make the call to receive the device.
           select() ensured that this will not block. */
            dev = udev_monitor_receive_device(mon);
            if (dev) {
                if(strcmp(udev_device_get_action(dev), "add") == 0) {
                    callback(udev_device_get_devpath(dev));
                    udev_device_unref(dev);
                }
                if(strcmp(udev_device_get_action(dev), "remove") == 0){
                    rmCallback();
                    udev_device_unref(dev);
                }
            }
            else {
                logger("No Device from receive_device(). An error occured.");
            }
        }
        usleep(250*1000);
        fflush(stdout);
    }
    udev_unref(udev);
}
}

void DeviceThread::startListening()
{
    writeLog("Starting Device Thread");
    // calls the function to listening device connections
    start_listening(&deviceAddedCallback, &deviceRemovedCallback, &writeLog);
}
