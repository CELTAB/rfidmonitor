#include <QSharedPointer>

#include "settings.h"
#include "logger.h"
/**
 * \brief The Settings::Settings constructor define the settings file path and format.
 *
 */
Settings::Settings(QObject *parent) :
    QObject(parent)
{
    m_fileSettings = QCoreApplication::applicationDirPath() + "/settings.ini";
    m_settings = new QSettings(QCoreApplication::applicationDirPath() + "/settings.ini", QSettings::NativeFormat);
}

/**
 * \brief The Settings::instance  function is a singleton pattern.
 * \return Return the unique instance of this class.
 */
Settings *Settings::instance()
{
    //Singleton
    static QSharedPointer<Settings> singleton(NULL);
    if(singleton.isNull()){
        singleton = QSharedPointer<Settings>(new Settings);
    }
    return singleton.data();
}

/**
 * \brief The Settings::setting function get value of a existing setting by a key.
 * \param key is the unique identification of the setting
 * \return Return a QVariant of the value from setting, or a null QVariant if it
 * is not found.
 */
QVariant Settings::setting(QString key)
{
    //Ensure the m_settings is updated.
    m_settings->sync();

    if(m_settings->contains(key))
        return m_settings->value(key);
    else
        Logger::instance()->writeRecord(Logger::fatal, "Settings",
                                        Q_FUNC_INFO,
                                        QString("Requested Setting key does not exists. Key:").
                                        append(key));

    // return a null QVariant
    return QVariant();
}

/**
 * \brief The Settings::saveSetting function save a new setting or update a existing one.
 * \param key is the unique identification of the setting
 * \param value is the value of the setting.
 * \param saveType is the saving type: KNewSetting for a new, or KUpdate for a existing.
 * \return true if success or false if fail.
 */
bool Settings::saveSetting(QString key, QVariant value, SaveType saveType)
{
    //Check if is possible to interact with settings file.
    if(m_settings->isWritable()){
        //Ensure the m_settings is updated.
        m_settings->sync();

        if( (saveType == KNewSetting) && (m_settings->contains(key)) ){
            //If the save option orders to save a new setting and it is already defined, it musn't be saved.
            Logger::instance()->writeRecord(Logger::fatal,
                                "Settings",
                                Q_FUNC_INFO,
                                QString("Saving new setting orders to not overwrite the settings, "
                                        "and a setting with same key is already defined. It is not going to be saved."));
            return false;
        }

        Logger::instance()->writeRecord(Logger::info, "Settings",
                                        Q_FUNC_INFO,
                                        QString("Saving or Updating setting [key=%1 , value=%2]").
                                        arg(key).arg(value.toString()));
        //Save the setting.
        m_settings->setValue(key,value);

        return true;
    }else{
        Logger::instance()->writeRecord(Logger::fatal, "Settings", Q_FUNC_INFO, QString("The settings file is not writable"));
        return false;
    }
}

/**
 * \brief The Settings::setDefaultSetting function register a new setting with a default value if it is not defined yet.
 * \param key is the unique identification of the setting
 * \param defaultValue is the default value of the setting.
 */
void Settings::setDefaultSetting(QString key, QVariant defaultValue)
{
    //Check if is possible to interact with settings file.
    if(m_settings->isWritable()){
        //Ensure the m_settings is updated.
        m_settings->sync();

        if(! m_settings->contains(key)){
            Logger::instance()->writeRecord(Logger::info, "Settings",
                                            Q_FUNC_INFO,
                                            QString("Saving a new default setting: key=%1 , value=%2").
                                            arg(key).arg(defaultValue.toString()));
            m_settings->setValue(key,defaultValue);
        }
    }else{
        Logger::instance()->writeRecord(Logger::fatal, "Settings", Q_FUNC_INFO, QString("The settings file is not writable"));
    }
}

/**
 * \brief The Settings::~Settings destructor mark for deletion the m_settings object.
 */
Settings::~Settings()
{
    m_settings->deleteLater();
}
