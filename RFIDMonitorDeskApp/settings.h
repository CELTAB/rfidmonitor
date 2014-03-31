#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVariant>

class Settings
{
private:
    QSettings m_settings;
    Settings();
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

    static Settings *instance();

    QVariant setting(const QString &key);
    void setSetting(const QString &key, const QVariant &value);
};

#endif // SETTINGS_H
