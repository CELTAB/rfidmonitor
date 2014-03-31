#include "settings.h"

Settings::Settings() :
m_settings("settings.ini", QSettings::NativeFormat)
{
    setSetting("Test","test");
}

Settings *Settings::instance()
{
    static Settings *instance = 0;
    if(!instance)
        instance = new Settings;
    return instance;
}

QVariant Settings::setting(const QString &key)
{
    return m_settings.value(key);
}

void Settings::setSetting(const QString &key, const QVariant &value)
{
    m_settings.setValue(key,value);
}
