#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QCoreApplication>
#include <QSettings>
#include <QVariant>

/**
 * \class Settings
 * \brief The Settings class provides the way to use the configuration file of application.
 *
 * This class use the QSetting to work.
 *
 * All the configuration attributes/settings are defined in the "settings.ini" file inside
 * the application dir. This class offers a way to get value from a setting, create a new
 * setting, update a setting and define a default value for a setting if it not exists yet.
 *
 * This class can be used by any other class that use the CoreLibrary.
 *
 * The configuration file (settings.ini) can also be edited by user using a text editor,
 * and QSetting will note any external change in the configuration file, keeping the state
 * updated.
 *
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    /**
     * \enum SaveType
     * \brief The enum SaveType defines the kind of save operation to settings.
     *
     * KNewSetting for saving a new setting (It musn't already exists)
     * KUpdate for updating a existing setting (It must exists)
     */
    enum SaveType {KNewSetting = 0, KUpdate};
    static Settings *instance();
    explicit Settings(QObject *parent = 0);
    QVariant setting(QString key);
    bool saveSetting(QString key, QVariant value, SaveType saveType = KNewSetting);
    void setDefaultSetting(QString key, QVariant defaultValue);
    ~Settings();
private:
    QString m_fileSettings;
    QSettings *m_settings;
};

#endif // SETTINGS_H
