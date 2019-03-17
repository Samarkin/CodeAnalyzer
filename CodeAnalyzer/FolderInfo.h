#ifndef FOLDERINFO_H
#define FOLDERINFO_H

#include <QSharedData>
#include <QMetaType>
#include <QString>

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
        totalFiles(other.totalFiles)
    {
    }

    QString folderPath;
    QHash<QString, int> filesByExt;
    int totalFiles{0};
};

#endif // FOLDERINFO_H
