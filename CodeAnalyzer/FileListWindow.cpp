#include "FileListWindow.h"
#include "ui_FileListWindow.h"

FileListWindow::FileListWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileListWindow)
{
    ui->setupUi(this);
}

FileListWindow::~FileListWindow()
{
    delete ui;
}
