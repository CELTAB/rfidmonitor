# RFIDMonitor
This project aims to provide a software solution for the researchers of the **Itaipu dam**. The system is capable of reading data from the Texas Instruments hardware. It currenly works on Linux x86_64 & arm.

The project is divided in two fundamental parts:
* Embedded
  * RFIDMonitor
  * RFIDDaemon
* Desktop
  * DeskApp
The combination of the **RFIDMonitor** and **RFIDDaemon** is called **RFIDCollector**.

### Embedded
#### RFIDMonitor
The goal of this project is to establish a serial connection with the RFID Reader and provide the mechanisms to send the received information to a remote computer through TCP/IP sockets. Is important to understand that the **RFIDMonitor** process does not make the connection with the remote computer, it can only send and receive information from the ouside world from a IPC channel with the **RFIDDaemon** process.

#### RFIDDaemon
To be able to start, stop and restart the **RFIDMonitor** we decided to create this project in a way that we can make open some communication channels with the outside world (TCP/UDP sockets) that are always listening to events, because the system is configured through a desktop application called **DeskApp**.

### Desktop
### DeskApp 
To be able to configure the system we created a humble application that provides the means to configure the **RFIDCollector**, these configurations are:
* Network interface (Wi-Fi SSID/Password)
* Collector id
* Collector name
* Serial device
* Default services (Reader, Persister, Exporter, Synchronizer, Communicator, Packager)
* MAC address (this one is readonly)
* Server address/port

For more information/documentation go to [wiki.celtab.org.br](wiki.celtab.org.br).

## Downloads

* [Ambiente de monitoramento para o Canal da Piracema](http://sourceforge.net/projects/rfidmonitor/files/docs/Ambiente%20de%20monitoramento%20para%20o%20Canal%20da%20Piracema.pdf/download).

* [autoCrossQtRFIDMonitor.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/autoCrossQtRFIDMonitor.tar.gz/download).

* [qt-everywhere-opensource-src-5.1.1.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/qt-everywhere-opensource-src-5.1.1.tar.gz/download).

* [libudev-dev_arm.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/libudev-dev_arm.tar.gz/download).

* [gcc-4.7-linaro-rpi-gnueabihf.tbz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/gcc-4.7-linaro-rpi-gnueabihf.tbz/download).

* [boost-raspi.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/boost-raspi.tar.gz/download).

* [cross-compile-tools.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/cross-compile-tools.tar.gz/download).

* [2013-02-09-wheezy-raspbian.tar.gz](http://sourceforge.net/projects/rfidmonitor/files/crosscompilation-resources/2013-02-09-wheezy-raspbian.tar.gz/download).

