#include "FolderProcessor.h"
#include <QDir>
#include <QDirIterator>

namespace
{
    #define IS_TEXT(ch) (ch == '\r' || ch == '\n' || ch == '\t' || ch > 31)

    template<typename char_type>
    bool getChar(QFile& file, char_type* ch)
    {
        return file.read(reinterpret_cast<char*>(ch), sizeof *ch) == sizeof(char_type);
    }

    template<typename char_type>
    bool getCharAndSwapBits(QFile& file, char_type* ch);

    template<>
    bool getCharAndSwapBits(QFile& file, quint32* ch)
    {
        if (file.read(reinterpret_cast<char*>(ch), sizeof *ch) < 4)
        {
            return false;
        }
        *ch = ((*ch & 0xFF) << 24)
                | ((*ch & 0xFF00) << 8)
                | ((*ch & 0xFF0000) >> 8)
                | ((*ch & 0xFF000000) >> 24);
        return true;
    }

    template<>
    bool getCharAndSwapBits(QFile& file, quint16* ch)
    {
        if (file.read(reinterpret_cast<char*>(ch), sizeof *ch) < 2)
        {
            return false;
        }
        *ch = quint16((*ch & 0xFF) << 8)
                | ((*ch & 0xFF00) >> 8);
        return true;
    }

    template<typename char_type>
    bool analyzeTextFile(QFile& file, TextFileInfo& fileInfo, bool (*get_char)(QFile& file, char_type* ch))
    {
        bool unixNewlines = false;
        bool windowsNewlines = false;
        char_type prevCh = '\0';
        char_type ch;
        while (get_char(file, &ch))
        {
            if (ch == '\n')
            {
                fileInfo.totalLines++;
                if (prevCh == '\r') windowsNewlines = true;
                else unixNewlines = true;
            }
            if (!IS_TEXT(ch)) return false;
            prevCh = ch;
        }
        if (ch == '\n')
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
} // private namespace

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
            struct buffer {
                union {
                    char raw[4];
                    quint8 bytes[4];
                    quint32 char32;
                    quint16 char16;
                    quint8 char8;
                };
            } buffer;
            bool isTextFile;

            qint64 buffer_size = file.read(buffer.raw, sizeof(buffer));
            if (buffer_size >= 4 && buffer.char32 == 0xFFFE0000U) {
                // REVIEW: Is it safe to always assume Little Endian CPU?
                tfi.encoding = Encoding::Utf32BE;
                isTextFile = analyzeTextFile(file, tfi, getCharAndSwapBits<quint32>);
            }
            else if (buffer_size >= 4 && buffer.char32 == 0x0000FEFFU) {
                tfi.encoding = Encoding::Utf32LE;
                isTextFile = analyzeTextFile(file, tfi, getChar<quint32>);
            }
            else if (buffer_size >= 2 && buffer.char16 == 0xFFFEU) {
                tfi.encoding = Encoding::Utf16BE;
                file.seek(2);
                isTextFile = analyzeTextFile(file, tfi, getCharAndSwapBits<quint16>);
            }
            else if (buffer_size >= 2 && buffer.char16 == 0xFEFFU) {
                tfi.encoding = Encoding::Utf16LE;
                file.seek(2);
                isTextFile = analyzeTextFile(file, tfi, getChar<quint16>);
            }
            else if (buffer_size >= 3 && buffer.bytes[0] == 0xEF && buffer.bytes[1] == 0xBB && buffer.bytes[2] == 0xBF) {
                tfi.encoding = Encoding::Utf8;
                file.seek(3);
                isTextFile = analyzeTextFile(file, tfi, getChar<quint8>);
            }
            else {
                tfi.encoding = Encoding::NoBom;
                file.seek(0);
                isTextFile = analyzeTextFile(file, tfi, getChar<quint8>);
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
