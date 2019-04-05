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

template<class TFileInfo>
class FileListItem : public QStandardItem
{
    TFileInfo fileInfo;
public:
    FileListItem(TFileInfo fileInfo) : fileInfo(fileInfo) {}
    virtual QVariant data(int role = Qt::UserRole + 1) const override;
    virtual int type() const override { return QStandardItem::UserType; }
};

template<template<class> class TCollection, class TFileInfo, class TPredicate>
void FileListWindow::setFileList(const TCollection<TFileInfo>& collection, TPredicate pred)
{
    for (const TFileInfo& fileInfo : collection)
    {
        if (!pred(fileInfo)) continue;
        auto *item = new FileListItem<TFileInfo>{fileInfo};
        modelRoot->appendRow(item);
    }
}

#endif // FILELISTWINDOW_H
