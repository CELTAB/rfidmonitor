#ifndef RFIDMONITORMANIPULATORWIDGET_H
#define RFIDMONITORMANIPULATORWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QStandardItemModel>

#include "../json/rfidmonitorsettings.h"

namespace Ui {
class RFIDMonitorManipulatorWidget;
}

class RFIDMonitorManipulatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RFIDMonitorManipulatorWidget(QWidget *parent = 0);
    ~RFIDMonitorManipulatorWidget();

    /**
     * @brief closeConnection called by the user, through the main window
     * with the Close Connection button.
     *
     * This will close the current main connection.
     */
    void closeConnection();

    /**
     * @brief The FormState enum determines the state of the form.
     */
    enum FormState {
        KBlocked = 1, /**< The form must be totally entirely blocked.
                            No interaction can be done.*/
        KEditing, /**< The form stay in editing state. Can change fields
                        and send the new configuration.*/
        KWithObject, /**< The form stay in read only mode, and contains
                            the configuration in the fields.*/
        KEmpty /**< The form stay in start state. Empty and ready to receive
                    new values.*/
    };

private:
    Ui::RFIDMonitorManipulatorWidget *ui;
    QStandardItemModel *m_treeViewModel;

    /**
     * @brief loadConfigurationFromJson Fill the fields with the configuration new
     * configuration.
     *
     * @param obj is the json object that contains the new configuration.
     */
    void loadConfigurationFromJson(const QJsonObject &obj);

    /**
     * @brief clearForm cleans all fields of the form.
     */
    void clearForm();

    /**
     * @brief formEnabled determines if the form must be entirely enabled or not.
     * @param enabled true if must be enabled or false if not.
     */
    void formEnabled(bool enabled);

    /**
     * @brief sendCurrentConfiguration builds a new json object with the content
     * from the fields, and send it to NetworkCommunication to delivery it to
     * the rasp.
     */
    void sendCurrentConfiguration();

    /**
     * @brief persistLocalyCurrentConfig NOT IMPLEMENTED YET. persists the new
     * configuration sent, to keep a history of it that can be reused later.
     * @param mac identifies the device.
     * @param json is the configuration sent to the rasp.
     */
    void persistLocalyCurrentConfig(const QString &mac, const QByteArray &json);

    /**
     * @brief m_raspSettings holds the instance of RFIDMonitorSettings to handle
     * the configuration reading and writting.
     */
    json::RFIDMonitorSettings *m_raspSettings;

    /**
     * @brief changeFormState called to change the state of the form.
     * @param state is the new state.
     */
    void changeFormState(FormState state);

public slots:

    /**
     * @brief raspDisconnected called when the current connection is closed.
     *
     * This will change the form to state KBlocked.
     */
    void raspDisconnected();

    /**
     * @brief newConfigFromRaspArrived handle the new configuration arrived.
     *
     * This will change the form to state KWithObject.
     *
     * @param json is the new configuration in json format.
     */
    void newConfigFromRaspArrived(const QJsonObject json);

    /**
     * @brief newConfigStatusFromRasp handle the answer from rasp about the
     * configuration sent to it.
     *
     * This will change the form to state KWithObject if the setting
     * the new configuration was successfully
     *
     * @param obj is the status answer in json format, that show the success
     * or the failure of defining the new configuration.
     */
    void newConfigStatusFromRasp(QJsonObject obj);

    /**
     * @brief btRetrieveConfigFromRaspClicked called when the user click
     * in the button to retrive the current configuration from rasp.
     */
    void btRetrieveConfigFromRaspClicked();

    /**
     * @brief btSendToRaspClicked called when the user click in the button
     * to send the new configuration from the form to the rasp.
     */
    void btSendToRaspClicked();

    /**
     * @brief btEditClicked called when the user wants to edit the configuration
     * in the form.
     *
     * This will change the form to state KEditing.
     */
    void btEditClicked();

    /**
     * @brief btCancelClicked called when the user wants to cancel the operation.
     *
     * This function reset the form state to KEmpty and cleans all the fields.
     */
    void btCancelClicked();
};

#endif // RFIDMONITORMANIPULATORWIDGET_H
