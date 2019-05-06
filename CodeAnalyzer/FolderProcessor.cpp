#include "FolderProcessor.h"
#include "TextUtils.h"
#include <QDir>
#include <QtConcurrent>
#include <QDirIterator>
#include <QMutex>

template<typename codeUnit_t, bool (*getCodeUnit)(QFile& file, codeUnit_t* pUnit)>
bool analyzeTextFile(QFile& file, TextFileInfo& fileInfo)
{
    bool unixNewlines = false;
    bool windowsNewlines = false;
    bool fileHasSpaceIndent = false;
    bool fileHasTabIndent = false;
    bool lineHasText = false;
    bool lineHasSpaceIndent = false;
    bool lineHasTabIndent = false;
    codeUnit_t prevPrevUnit = '\0';
    codeUnit_t prevUnit = '\0';
    codeUnit_t unit;
    while (getCodeUnit(file, &unit))
    {
        if (!isTextCodeUnit(unit)) return false;
        if (unit == '\n')
        {
            fileInfo.totalLines++;
            if (!lineHasText)
            {
                fileInfo.emptyLines++;
            }
            else
            {
                fileHasTabIndent |= lineHasTabIndent;
                fileHasSpaceIndent |= lineHasSpaceIndent;
            }
            if (prevUnit == '\r')
            {
                windowsNewlines = true;
                if (isWhitespaceCodeUnit(prevPrevUnit))
                {
                    fileInfo.linesWithTrailSpaces++;
                }
            }
            else
            {
                unixNewlines = true;
                if (isWhitespaceCodeUnit(prevUnit))
                {
                    fileInfo.linesWithTrailSpaces++;
                }
            }
            lineHasText = false;
            lineHasTabIndent = false;
            lineHasSpaceIndent = false;
        }
        else if (isWhitespaceCodeUnit(unit))
        {
            if (!lineHasText)
            {
                if (unit == ' ') lineHasSpaceIndent = true;
                if (unit == '\t') lineHasTabIndent = true;
            }
        }
        else if (unit != '\r')
        {
            lineHasText = true;
        }
        prevPrevUnit = prevUnit;
        prevUnit = unit;
    }
    if (unit == '\n')
    {
        fileInfo.trailingNewline = true;
    }
    else
    {
        fileInfo.totalLines++;
        fileInfo.trailingNewline = false;
        if (isWhitespaceCodeUnit(unit))
        {
            fileInfo.linesWithTrailSpaces++;
        }
        if (!lineHasText) fileInfo.emptyLines++;
    }
    fileInfo.newlines = windowsNewlines
        ? (unixNewlines ? Newlines::Mixed : Newlines::Windows)
        : (unixNewlines ? Newlines::Unix : Newlines::Unknown);
    fileInfo.indentation = fileHasSpaceIndent
        ? (fileHasTabIndent ? Indentation::Mixed : Indentation::Spaces)
        : (fileHasTabIndent ? Indentation::Tabs : Indentation::Unknown);
    return true;
}

template<template<class> class Collection>
void processFile(const Collection<const Language*>& languages, FolderInfoData* info, QMutex& mutex, const QDir& folder, const QFileInfo& fileInfo)
{
    const Language* language = nullptr;
    for (const Language* lang : languages)
    {
       if (lang->checkFile(fileInfo.fileName()))
       {
           language = lang;
       }
    }
    TextFileInfo tfi{folder.relativeFilePath(fileInfo.filePath()), fileInfo.suffix(), language};

    // Read file
    QFile file{fileInfo.filePath()};
    if (!file.open(QFile::ReadOnly))
    {
        QMutexLocker lock{&mutex};
        info->filesByLanguage[language]++;
        info->inaccessibleFiles.append(tfi);
        return;
    }
    bool isTextFile;
    tfi.encoding = detectEncodingAndAdvanceToFirstCodeUnit(file);
    switch (tfi.encoding) {
    case Encoding::NoBom:
        isTextFile = analyzeTextFile<quint8, getCodeUnit>(file, tfi);
        break;
    case Encoding::Utf8:
        isTextFile = analyzeTextFile<quint8, getCodeUnit>(file, tfi);
        break;
    case Encoding::Utf16BE:
        isTextFile = analyzeTextFile<quint16, getCodeUnitAndSwapOctets>(file, tfi);
        break;
    case Encoding::Utf16LE:
        isTextFile = analyzeTextFile<quint16, getCodeUnit>(file, tfi);
        break;
    case Encoding::Utf32BE:
        isTextFile = analyzeTextFile<quint32, getCodeUnitAndSwapOctets>(file, tfi);
        break;
    case Encoding::Utf32LE:
        isTextFile = analyzeTextFile<quint32, getCodeUnit>(file, tfi);
        break;
    }
    QMutexLocker lock{&mutex};
    info->filesByLanguage[language]++;
    if (!isTextFile)
    {
        info->binaryFiles.append(tfi);
        return;
    }
    info->textFiles.append(tfi);
    info->totalLines += quint64(tfi.totalLines);
    info->emptyLines += quint64(tfi.emptyLines);
    info->linesWithTrailSpaces += quint64(tfi.linesWithTrailSpaces);
    if (tfi.linesWithTrailSpaces > 0) info->filesWithTrailSpaces++;
    if (tfi.trailingNewline) info->filesWithEol++;
    else info->filesWithNoEol++;
    if (tfi.newlines == Newlines::Windows) info->filesWithWindowsNewlines++;
    else if (tfi.newlines == Newlines::Unix) info->filesWithUnixNewlines++;
    else if (tfi.newlines == Newlines::Mixed) info->filesWithMixedNewlines++;
    if (tfi.indentation == Indentation::Tabs) info->filesWithTabIndent++;
    else if (tfi.indentation == Indentation::Spaces) info->filesWithSpaceIndent++;
    else if (tfi.indentation == Indentation::Mixed) info->filesWithMixedIndent++;
}

void FolderProcessor::addLanguage(const Language* const language)
{
    languages.push_back(language);
}

void FolderProcessor::process(const QString folderPath)
{
    emit startingProcessing(folderPath);
    auto *info = new FolderInfoData;
    info->folderPath = folderPath;
    QDir folder{folderPath};
    QDirIterator it{folder, QDirIterator::Subdirectories};
    int totalFiles = 0;
    QSemaphore filesProcessed{0};
    QMutex mutex;
    while (it.hasNext())
    {
        it.next();
        const QFileInfo& fileInfo = it.fileInfo();
        if (fileInfo.isFile())
        {
            totalFiles++;
            QtConcurrent::run([&, fileInfo](){
                processFile(languages, info, mutex, folder, fileInfo);
                filesProcessed.release();
            });
        }
    }
    filesProcessed.acquire(totalFiles);
    emit doneProcessing(FolderInfo{info});
}
