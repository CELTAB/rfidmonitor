#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
public:
    enum ConnectionType {KSerial = 0, KNetwork};
    enum TcpDataType {KHandshake = 0,
                      KConfiguration,
                      KNewReaderCommand,
                      KNewReaderAnswer,
                      KDidntUnderstand,
                      KInstruction,
                      KRFIDMonitorAnswer
                     };
    enum InteractionType {KReader = 0, KRFIDMonitor};
    Settings();
};

#endif // SETTINGS_H
