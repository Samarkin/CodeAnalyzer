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
        filesByExt(other.filesByExt),
        textFiles(other.textFiles),
        binaryFiles(other.binaryFiles)
    {
    }

    QString folderPath;
    QHash<QString, int> filesByExt;
    QVector<FileInfo> inaccessibleFiles;
    QVector<TextFileInfo> textFiles;
    QVector<BinaryFileInfo> binaryFiles;
};

#endif // FOLDERINFO_H
