#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>
#include "Language.h"

struct FileInfo
{
    FileInfo() = default;
    FileInfo(QString path, QString ext) : filePath(path), fileExt(ext) {}
    inline QString path() const { return filePath; }
    inline QString ext() const { return fileExt; }
private:
    QString filePath;
    QString fileExt;
};

struct BinaryFileInfo : public FileInfo
{
    BinaryFileInfo() = default;
    BinaryFileInfo(const FileInfo& fileInfo) : FileInfo(fileInfo) {}
    BinaryFileInfo(QString path, QString ext) : FileInfo(path, ext) {}
};

enum class Encoding
{
    NoBom,
    Utf8,
    Utf16LE,
    Utf16BE,
    Utf32LE,
    Utf32BE,
};

enum class Indentation
{
    Unknown,
    Tabs,
    Spaces,
    Mixed,
};

enum class Newlines
{
    Unknown,
    Windows,
    Unix,
    Mixed,
};

struct TextFileInfo : public FileInfo
{
    TextFileInfo() = default;
    TextFileInfo(QString path, QString ext, Language* language) : FileInfo(path, ext), language(language) {}
    Language* language;
    Encoding encoding{Encoding::NoBom};
    Indentation indentation{Indentation::Unknown};
    Newlines newlines{Newlines::Unknown};
    quint32 totalLines{0};
    quint32 emptyLines{0};
    bool trailingNewline{false};
    char __padding[3];
};

#endif // FILEINFO_H
