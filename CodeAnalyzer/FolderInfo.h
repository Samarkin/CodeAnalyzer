#ifndef FOLDERINFO_H
#define FOLDERINFO_H

#include <QSharedData>
#include <QMetaType>
#include <QString>

class FolderInfo;

typedef QSharedDataPointer<FolderInfo> FolderInfoPointer;
Q_DECLARE_METATYPE(FolderInfoPointer);

class FolderInfo : public QSharedData
{
public:
    FolderInfo()
        : totalFiles(0)
    {
    }

    FolderInfo(const FolderInfo& other)
        : QSharedData(other), folderPath(other.folderPath), totalFiles(other.totalFiles)
    {
    }

    QString folderPath;
    int totalFiles;
};

#endif // FOLDERINFO_H
