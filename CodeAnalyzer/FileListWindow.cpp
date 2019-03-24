#include "FileListWindow.h"
#include "ui_FileListWindow.h"
#include <QStandardItemModel>

FileListWindow::FileListWindow(QString folderPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileListWindow),
    folderPath(folderPath)
{
    ui->setupUi(this);

    auto *model = new QStandardItemModel{this};
    auto *root = new QStandardItem{folderPath};
    model->appendRow(root);
    model->setHorizontalHeaderLabels(QStringList{tr("Filename")});
    ui->treeView->setModel(model);
    ui->treeView->expandAll();
    modelRoot = root;
}

FileListWindow::~FileListWindow()
{
    delete ui;
}

void FileListWindow::setTitle(QString title)
{
    setWindowTitle(tr("%1 - %2").arg(folderPath).arg(title));
}
