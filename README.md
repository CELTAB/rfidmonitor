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

### RFIDMonitor Platform
The RFIDMonitor Platform ystem is responsible for communicates with the Collecting Point to exchange information and to save all the collected data into the database. Also offers an RestFul API to consume those data and a web-based user interface for administration. [RFIDMonitor Platform](https://github.com/CELTAB/rfidmonitor-platform).

For more information/documentation go to wiki.celtab.org.br/index.php/RFIDMonitor

## Downloads

* [Ambiente de monitoramento para o Canal da Piracema](http://wiki.celtab.org.br/images/7/7c/Proposta.pdf).

* [PrepareEnvironmentRFIDMonitor.tar.gz](https://github.com/CELTAB/rfidmonitor/releases/download/v0.8/PrepareEnvironmentRFIDMonitor.tar.gz).

