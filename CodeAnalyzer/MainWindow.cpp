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
    connect(&processor, SIGNAL(doneProcessing(FolderInfoPointer)),
            this, SLOT(updateFolderInfo(FolderInfoPointer)));
    processingThread.start();
}

void MainWindow::openClicked()
{
    QFileDialog dialog{this, tr("Select folder")};
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec())
    {
        ui->statusBar->showMessage(tr("Analyzing..."));
        QMetaObject::invokeMethod(&processor, "process", Q_ARG(QString, dialog.selectedFiles()[0]));
    }
}

void MainWindow::updateFolderInfo(FolderInfoPointer info)
{
    ui->statusBar->clearMessage();
    QMessageBox::information(this, tr("Done"), tr("I scanned %1 and found %2 files").arg(info->folderPath).arg(info->totalFiles));
    ui->statusBar->showMessage(tr("%1 files found").arg(info->totalFiles));
}

MainWindow::~MainWindow()
{
    delete ui;
    processingThread.quit();
    processingThread.wait();
}
