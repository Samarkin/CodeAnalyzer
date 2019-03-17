#include "FolderProcessor.h"
#include <QDir>
#include <QDirIterator>

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
        if (fileInfo.isFile()) {
            info->totalFiles++;
            info->filesByExt[fileInfo.suffix()]++;
        }

    }
    emit doneProcessing(info);
}
