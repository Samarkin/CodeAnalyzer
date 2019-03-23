#ifndef FOLDERINFO_H
#define FOLDERINFO_H

#include <QSharedData>
#include <QMetaType>
#include <QString>
#include <QVector>
#include "FileInfo.h"

class FolderInfoData;

typedef QSharedDataPointer<FolderInfoData> FolderInfo;
Q_DECLARE_METATYPE(FolderInfo);

class FolderInfoData : public QSharedData
{
public:
    FolderInfoData()
    {
    }

    FolderInfoData(const FolderInfoData& other) :
        QSharedData(other),
        folderPath(other.folderPath),
        textFiles(other.textFiles),
        binaryFiles(other.binaryFiles),
        filesByExt(other.filesByExt),
        totalLines(other.totalLines),
        filesWithEol(other.filesWithEol),
        filesWithNoEol(other.filesWithNoEol),
        filesWithWindowsNewlines(other.filesWithWindowsNewlines),
        filesWithUnixNewlines(other.filesWithUnixNewlines),
        filesWithMixedNewlines(other.filesWithMixedNewlines)
    {
    }

    QString folderPath;
    QVector<FileInfo> inaccessibleFiles;
    QVector<TextFileInfo> textFiles;
    QVector<BinaryFileInfo> binaryFiles;

    // Aggregates
    QHash<QString, int> filesByExt;
    quint64 totalLines{0};
    quint32 filesWithEol{0};
    quint32 filesWithNoEol{0};
    quint32 filesWithWindowsNewlines{0};
    quint32 filesWithUnixNewlines{0};
    quint32 filesWithMixedNewlines{0};
};

#endif // FOLDERINFO_H
