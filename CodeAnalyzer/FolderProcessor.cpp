#include "FolderProcessor.h"
#include <QDir>
#include <QDirIterator>

void FolderProcessor::process(const QString folderPath)
{
    emit startingProcessing(folderPath);
    FolderInfoPointer info{new FolderInfo};
    info->folderPath = folderPath;
    QDir folder{folderPath};
    QDirIterator it{folder, QDirIterator::Subdirectories};
    while (it.hasNext())
    {
        it.next();
        if (it.fileInfo().isFile()) {
            info->totalFiles++;
        }
    }
    emit doneProcessing(info);
}
