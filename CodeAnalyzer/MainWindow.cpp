#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(openClicked()));

    processor.moveToThread(&processingThread);
    connect(&processor, SIGNAL(doneProcessing(FolderInfo)),
            this, SLOT(updateFolderInfo(FolderInfo)));
    connect(&processor, SIGNAL(startingProcessing(QString)),
            this, SLOT(indicateProcessing()));
    processingThread.start();
}

void MainWindow::openClicked()
{
    QFileDialog dialog{this, tr("Select folder")};
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec())
    {
        QMetaObject::invokeMethod(&processor, "process", Q_ARG(QString, dialog.selectedFiles()[0]));
    }
}

void MainWindow::indicateProcessing()
{
    ui->statusBar->showMessage(tr("Analyzing..."));
}

void MainWindow::updateFolderInfo(FolderInfo info)
{
    ui->statusBar->clearMessage();
    ui->label_textFiles->setText(QString::number(info->textFiles.count()));
    ui->label_binaryFiles->setText(QString::number(info->binaryFiles.count()));
    ui->label_inaccessibleFiles->setText(QString::number(info->inaccessibleFiles.count()));
    ui->statusBar->showMessage(tr("%1 files found").arg(info->textFiles.count() + info->binaryFiles.count() + info->inaccessibleFiles.count()));
}

MainWindow::~MainWindow()
{
    delete ui;
    processingThread.quit();
    processingThread.wait();
}
