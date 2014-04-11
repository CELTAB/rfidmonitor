#ifndef RICTLMB2B30WIDGET_H
#define RICTLMB2B30WIDGET_H

#include <QWidget>
#include <QTimer>

#include "settings.h"

namespace Ui {
class RICTLMB2B30Widget;
}

class RICTLMB2B30Widget : public QWidget
{
    Q_OBJECT

public:
    explicit RICTLMB2B30Widget(Settings::ConnectionType connType, QWidget *parent = 0);
    ~RICTLMB2B30Widget();

private:
    Ui::RICTLMB2B30Widget *ui;
    bool m_waitingForAnswer;
    bool m_waitingForCheck;
    QString m_identification;
    Settings::ConnectionType m_connectionType;
    void sendCommand(const QString &command);
    QTimer m_timeout;

public slots:
    void leIdentificationChanged(QString newText);
    void btWriteClicked();
    void newAnswerFromSerialComm(QString answer);
    void timeout();
    void rbDecimalClicked();
    void rbHexadecimalClicked();
    void incrementIdentification();
};

#endif // RICTLMB2B30WIDGET_H
