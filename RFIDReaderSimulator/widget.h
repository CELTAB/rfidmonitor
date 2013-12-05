#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class DataReader;

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;
    DataReader *m_dataReader;

private slots:
    void onChooseFileClicked();
    void onWriteClicked();
    void onClearClicked();

signals:
//    void dataReady(const QString &);
    void dataReady(QByteArray);
};

#endif // WIDGET_H
