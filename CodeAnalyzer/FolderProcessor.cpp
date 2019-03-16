#include "FolderProcessor.h"
#include <QDir>
#include <QDirIterator>

void FolderProcessor::process(const QString folderPath)
{
    FolderInfoPointer info{new FolderInfo};
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
