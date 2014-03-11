#include <QCoreApplication>

#include "coremodule.h"

#include "applicationsettings.h"

ApplicationSettings::ApplicationSettings(QObject *parent) :
    QObject(parent),
    m_servicesLoaded(false)
{
}

ApplicationSettings *ApplicationSettings::instance()
{
    static ApplicationSettings *singleton=0;
    if(!singleton){
        singleton = new ApplicationSettings(qApp);
    }
    return singleton;
}

void ApplicationSettings::installModule(CoreModule *module)
{
    // 1 - Check if module is not installed or is outdatedin JSON Configuration
    if(!(module->name().size()&&module->version())) {
        // if the condition above is true, the system updates the file
        foreach (Service *serv, module->services()) {
            // install all services
        }
        // update module version

    }
}

void ApplicationSettings::loadDefaultServices()
{
    // Load default Services on the system
    // Search for the default services in the JSON configuration file and try to load the instances into the map

}

bool ApplicationSettings::servicesLoaded()
{
    return m_servicesLoaded;
}

const QMap<ServiceType, QString> &ApplicationSettings::defaultServices() const
{
    return m_defaultServices;
}
