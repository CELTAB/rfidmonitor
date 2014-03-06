#include <QCoreApplication>

#include <QRegularExpression>
#include <QProcess>

#include "mtapplication.h"
#include "impl/reading.h"
#include "impl/synchronization.h"
#include "impl/packager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //    MTApplication::instance()->registerService(Constants::Reading, new Reading);
    //    MTApplication::instance()->registerService(Constants::Synchronizing, new Synchronization);
    //    MTApplication::instance()->registerService(Constants::Packaging, new Packager);

    //    MTApplication::instance()->init();


    QProcess media;
    media.start("cat /proc/mounts");
    media.waitForFinished(-1);
    QString allMounted(media.readAllStandardOutput());

    QRegularExpression regexCode;
    regexCode.setPattern("/dev/[a-z]{3}[0-9]{1}?\\s/media/(.*)\\s");
    QRegularExpressionMatch match = regexCode.match(allMounted);

    foreach (QString i, match.capturedTexts()) {
        qDebug() << i;
    }

    return a.exec();
}
