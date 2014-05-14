TEMPLATE = subdirs

SUBDIRS += \
    CoreLibrary \
    ReaderRFM008BModule \
    ReaderMRI2000Module \
    PersisterModule \
    ExporterModule \
    Main \
    CommunicatorModule \
    SynchronizerModule

CONFIG += ordered
