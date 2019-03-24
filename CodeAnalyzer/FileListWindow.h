#ifndef FILELISTWINDOW_H
#define FILELISTWINDOW_H

#include <QWidget>
#include <QStandardItem>

namespace Ui {
class FileListWindow;
}

class FileListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FileListWindow(QString folderPath, QWidget *parent = nullptr);
    ~FileListWindow();

    template<template<class> class TCollection, class TFileInfo, class TPredicate = std::function<bool(const TFileInfo&)>>
    void setFileList(const TCollection<TFileInfo>& collection, TPredicate pred = [](const TFileInfo&) {return true;});
    void setTitle(QString title);

public slots:
   void selectionChanged(const QModelIndex&, const QModelIndex&);

private:
    QStandardItem *modelRoot;
    Ui::FileListWindow *ui;
    QString folderPath;
};

template<template<class> class TCollection, class TFileInfo, class TPredicate>
void FileListWindow::setFileList(const TCollection<TFileInfo>& collection, TPredicate pred)
{
    for (const TFileInfo& fileInfo : collection)
    {
        if (!pred(fileInfo)) continue;
        auto *item = new QStandardItem{fileInfo.path()};
        modelRoot->appendRow(item);
    }
}

#endif // FILELISTWINDOW_H
