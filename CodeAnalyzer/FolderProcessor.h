#ifndef FOLDERPROCESSOR_H
#define FOLDERPROCESSOR_H

#include <QObject>
#include <FolderInfo.h>

class FolderProcessor : public QObject
{
    Q_OBJECT

public slots:
    void process(const QString folderPath);

signals:
    void startingProcessing(const QString folderPath);
    void doneProcessing(FolderInfo info);
};

#endif // FOLDERPROCESSOR_H
