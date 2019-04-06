QT += testlib concurrent
QT -= gui

CONFIG += testcase

INCLUDEPATH += \
    ../../CodeAnalyzer

HEADERS += \
    ../../CodeAnalyzer/FolderProcessor.h\
    ../../CodeAnalyzer/Language.h\
    ../../CodeAnalyzer/CommonLanguage.h

SOURCES += \
    ../../CodeAnalyzer/FolderProcessor.cpp\
    ../../CodeAnalyzer/Language.cpp\
    ../../CodeAnalyzer/CommonLanguage.cpp\
    tst_folderprocessor.cpp

