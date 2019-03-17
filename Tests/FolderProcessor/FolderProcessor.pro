QT += testlib
QT -= gui

CONFIG += testcase

INCLUDEPATH += \
    ../../CodeAnalyzer

HEADERS += \
    ../../CodeAnalyzer/FolderProcessor.h

SOURCES += \
    ../../CodeAnalyzer/FolderProcessor.cpp\
    tst_folderprocessor.cpp

