#ifndef PACKET_H
#define PACKET_H

#include <QObject>
#include <QDateTime>
#include <QVariant>

class QSqlRecord;

class Packet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant md5hash
        READ md5hash
        WRITE setMd5hash)
    Q_PROPERTY(QVariant md5hash
               READ md5hash
               WRITE setMd5hash)
    Q_PROPERTY(QVariant idBegin
               READ idBegin
               WRITE setIdBegin)
    Q_PROPERTY(QVariant itemCount
               READ itemCount
               WRITE setItemCount)
    Q_PROPERTY(QVariant jsonData
               READ jsonData
               WRITE setJsonData)
    Q_PROPERTY(QVariant status
               READ status
               WRITE setStatus)

public:
    enum class Status {KSynchronized = 0, KPending};
    explicit Packet(QObject *parent = 0);
    explicit Packet(const QSqlRecord &record, QObject *parent = 0);

    QVariant md5hash() const;
    void setMd5hash(QVariant value);

    QVariant dateTime() const;
    void setDateTime(QVariant value);

    QVariant idBegin() const;
    void setIdBegin(QVariant value);

    QVariant itemCount() const;
    void setItemCount(QVariant value);

    QVariant jsonData() const;
    void setJsonData(QVariant value);

    QVariant status() const;
    void setStatus(QVariant value);

private:
    QString m_md5hash;
    QDateTime m_dateTime;
    qlonglong m_idbegin;
    qlonglong m_itemCount;
    QByteArray m_jsondata;
    Status m_status;
};

#endif // PACKET_H
