#include "FolderProcessor.h"
#include "TextUtils.h"
#include <QDir>
#include <QDirIterator>

template<typename codeUnit_t>
bool analyzeTextFile(QFile& file, TextFileInfo& fileInfo, bool (*getCodeUnit)(QFile& file, codeUnit_t* pUnit))
{
    bool unixNewlines = false;
    bool windowsNewlines = false;
    codeUnit_t prevUnit = '\0';
    codeUnit_t unit;
    while (getCodeUnit(file, &unit))
    {
        if (unit == '\n')
        {
            fileInfo.totalLines++;
            if (prevUnit == '\r') windowsNewlines = true;
            else unixNewlines = true;
        }
        if (!IS_TEXT_CODE_UNIT(unit)) return false;
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
    }
    fileInfo.newlines = windowsNewlines
        ? (unixNewlines ? Newlines::Mixed : Newlines::Windows)
        : (unixNewlines ? Newlines::Unix : Newlines::Unknown);
    return true;
}

void FolderProcessor::process(const QString folderPath)
{
    emit startingProcessing(folderPath);
    FolderInfo info{new FolderInfoData};
    info->folderPath = folderPath;
    QDir folder{folderPath};
    QDirIterator it{folder, QDirIterator::Subdirectories};
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        if (fileInfo.isFile())
        {
            TextFileInfo tfi{folder.relativeFilePath(fileInfo.filePath()), fileInfo.suffix()};
            // TODO: This should eventually be removed
            info->filesByExt[tfi.ext()]++;

            // Read file
            QFile file{fileInfo.filePath()};
            if (!file.open(QFile::ReadOnly))
            {
                info->inaccessibleFiles.append(tfi);
                continue;
            }
            bool isTextFile;
            tfi.encoding = detectEncodingAndAdvanceToFirstCodeUnit(file);
            switch (tfi.encoding) {
            case Encoding::NoBom:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnit<quint8>);
                break;
            case Encoding::Utf8:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnit<quint8>);
                break;
            case Encoding::Utf16BE:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnitAndSwapOctets<quint16>);
                break;
            case Encoding::Utf16LE:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnit<quint16>);
                break;
            case Encoding::Utf32BE:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnitAndSwapOctets<quint32>);
                break;
            case Encoding::Utf32LE:
                isTextFile = analyzeTextFile(file, tfi, getCodeUnit<quint32>);
                break;
            }
            if (!isTextFile)
            {
                info->binaryFiles.append(tfi);
                continue;
            }
            info->textFiles.append(tfi);
            info->totalLines += quint64(tfi.totalLines);
            if (tfi.trailingNewline) info->filesWithEol++;
            else info->filesWithNoEol++;
            if (tfi.newlines == Newlines::Windows) info->filesWithWindowsNewlines++;
            else if (tfi.newlines == Newlines::Unix) info->filesWithUnixNewlines++;
            else if (tfi.newlines == Newlines::Mixed) info->filesWithMixedNewlines++;
        }

    }
    emit doneProcessing(info);
}
