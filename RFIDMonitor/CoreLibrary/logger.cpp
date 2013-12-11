#include <QDebug>
#include <QDateTime>
#include <QSharedPointer>

#include "logger.h"


/*
    This source code is commented because we have som problems no cross-compiling boost for ARM architecture.
*/

Logger::Logger(QObject *parent):
    QObject(parent)
{
//    logformat = expr::stream
//            // Record format: 00001 % 05-25-2013_16:05:45 % severity % Module Name % Thread Name % Void functionName() % Any message
//            << std::hex << std::setw(4) << std::setfill('0') << line_id << std::dec << std::setfill(' ') << " % "
//            << expr::attr< std::string >("DateTime") << " % "
//            << severity << " % "
//            << expr::attr< std::string >("Module") << " % "
//            << expr::attr< std::string >("Function") << " % "
//            << expr::smessage;

    file.setFileName("RFID_log.log");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            return;
        }
    QTextStream out(&file);
    out << "\n";
}

// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, Logger::severity_level level)
{
    static const char* strings[] =
    {
        "INFO",
        "DEBUG",
        "WARNING",
        "ERROR",
        "CRITICAL",
        "FATAL"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

std::string Logger::currentDateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    return current.toString("MM-dd-yyyy_hh:mm:ss").toStdString();
}

void Logger::startDebugMode()
{
//                boost::shared_ptr< sinks::text_file_backend > debugBackend =  boost::make_shared< sinks::text_file_backend >
//                        (
//                            keywords::file_name = "logs/Log_DEBUG_%Y%m_%H%M%S.log",
//                            keywords::rotation_size = 3 * 1024 * 1024
//                        );

//                boost::shared_ptr< sink_t > sinkDebug(new sink_t(debugBackend));

//                sinkDebug->set_formatter(logformat);
//                logging::core::get()->add_sink(sinkDebug);
    writeRecord(Logger::debug, "CoreLibrary", Q_FUNC_INFO, "Start Debug Mode");
}

//void Logger::writeLastRecord(sinks::text_file_backend::stream_type& file)
//{
//    file << "\n";
//}

void Logger::initLog()
{
//    boost::shared_ptr< sinks::text_file_backend > backend =
//            boost::make_shared< sinks::text_file_backend >
//            (
//                //  Log_%Y%m_%N.log Log_201309_1.log Log_201309_2.log ...
//                keywords::file_name = "logs/Log_%Y%m_%N.log",
//                keywords::rotation_size = 3 * 1024 * 1024, // define max log file size to 5MB
//                keywords::open_mode=(std::ios::out | std::ios::app) // Verify if exist a log file to append records to it
//            );

//    // Initialize sinks
//    boost::shared_ptr< sink_t > sink(new sink_t(backend));

//    sink->set_formatter(logformat);
//    sink->set_filter(severity != debug); // write every record except records with debug severity level
//    sink->locked_backend()->auto_flush(true);

//    sink->locked_backend()->set_close_handler(&writeLastRecord);

//    logging::core::get()->add_sink(sink);

//    /* Add attributes:
//     * "LineID", "TimeStamp", "ProcessID" and "ThreadID" are registered globally
//     */
//    logging::add_common_attributes();
}

Logger *Logger::instance()
{
    static QSharedPointer<Logger> singleton(NULL);
    if(singleton.isNull()){
        singleton = QSharedPointer<Logger>(new Logger);
        singleton.data()->initLog();
    }
    return singleton.data();
}

//void Logger::writeRecord(severity_level lvl, QString moduleName,
//                         QString FunctionName,
//                         QString message)
//{
//    src::severity_logger< Logger::severity_level > m_logger;

//    m_logger.add_attribute("DateTime", attrs::make_function(&currentDateTime));
//    m_logger.add_attribute("Module", attrs::constant<std::string>(moduleName.toStdString()));
//    m_logger.add_attribute("Function", attrs::constant< std::string >(FunctionName.toStdString()));

//    BOOST_LOG_SEV(m_logger, lvl) << message.toStdString();
//}

void Logger::writeRecord(severity_level lvl, QString moduleName,
                         QString functionName,
                         QString message)
{
    // 00001 % 05-25-2013_16:05:45 % severity % Module Name % Thread Name % Void functionName() % Any message
    QTextStream out(&file);
    QString record("99999 % ");
    QString currDate = QString::fromStdString(currentDateTime());
    record.append(currDate + " % " + moduleName + " % " + functionName + " % " + message + "\n");
    out << record;
}
