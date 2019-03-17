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
        : totalFiles(0)
    {
    }

    FolderInfoData(const FolderInfoData& other)
        : QSharedData(other), folderPath(other.folderPath), totalFiles(other.totalFiles)
    {
    }

    QString folderPath;
    int totalFiles;
};

#endif // FOLDERINFO_H
