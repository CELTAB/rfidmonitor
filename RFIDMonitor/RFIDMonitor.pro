TEMPLATE = subdirs

SUBDIRS += \
    CoreLibrary \
    ReadingModule \
#    PersistenceModule \
#    ExportModule \
#    MainServiceModule \
    Main \
    CommunicationModule \
    SynchronizationModule

CONFIG += ordered
