#ifndef SYSTEMMESSAGESWIDGET_H
#define SYSTEMMESSAGESWIDGET_H

#include <QWidget>

namespace Ui {
class SystemMessagesWidget;
}

class SystemMessagesWidget : public QWidget
{
    Q_OBJECT

public:
    enum MessageBehavior {KOnlyDialog=0, KOnlyLog, KDialogAndLog};
    enum MessageLevel {KInfo=0, KWarning, KError, KFatal};
    static SystemMessagesWidget *instance();
    ~SystemMessagesWidget();

    void writeMessage(const QString &message,
                      const MessageLevel &messageLevel= KInfo,
                      const MessageBehavior &messageBehavior = KOnlyLog);

private:
    explicit SystemMessagesWidget(QWidget *parent = 0);
    Ui::SystemMessagesWidget *ui;
};

#endif // SYSTEMMESSAGESWIDGET_H
