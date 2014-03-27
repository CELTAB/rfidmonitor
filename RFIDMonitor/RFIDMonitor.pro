TEMPLATE = subdirs

SUBDIRS += \
    CoreLibrary \
    ReaderRFM008BModule \
    PersisterModule \
    ExporterModule \
#    MainServiceModule \
    Main \
    CommunicatorModule \
    SynchronizerModule

CONFIG += ordered
