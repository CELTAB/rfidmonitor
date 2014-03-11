#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include <QMap>

class CoreModule;
class Service;
enum class ServiceType;

class ApplicationSettings : public QObject
{
    Q_OBJECT
public:

    static ApplicationSettings * instance();
    /*!
     * \brief installModule
     * \param module
     */
    void installModule(CoreModule *module);

    /*!
     * \brief loadDefaultServices
     */
    void loadDefaultServices();

    /*!
     * \brief servicesLoaded
     * \return
     */
    bool servicesLoaded();

    /*!
     * \brief m_defaultServices
     */
    const QMap<ServiceType, QString> & defaultServices() const;

private:
    explicit ApplicationSettings(QObject *parent = 0);

    bool m_servicesLoaded;
    QMap<ServiceType, QString> m_defaultServices;
};

#endif // APPLICATIONSETTINGS_H
