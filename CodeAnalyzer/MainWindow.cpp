#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FileListWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include "CommonLanguage.h"

#define CONNECT_LABEL_LINK(label) connect(label, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)))

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(openClicked()));
    CONNECT_LABEL_LINK(ui->label);
    CONNECT_LABEL_LINK(ui->label_2);
    CONNECT_LABEL_LINK(ui->label_3);
    CONNECT_LABEL_LINK(ui->label_4);
    CONNECT_LABEL_LINK(ui->label_5);
    CONNECT_LABEL_LINK(ui->label_6);
    CONNECT_LABEL_LINK(ui->label_7);
    CONNECT_LABEL_LINK(ui->label_8);
    CONNECT_LABEL_LINK(ui->label_9);

    processor.addLanguage(CommonLanguage::CPlusPlus);
    processor.addLanguage(CommonLanguage::CSharp);
    processor.addLanguage(CommonLanguage::Makefile);
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

void MainWindow::linkActivated(const QString& link)
{
    if (folderInfo == nullptr)
    {
        return;
    }

    FileListWindow *wnd = new FileListWindow{folderInfo->folderPath, this};
    if (link == "binaryFiles")
    {
        wnd->setFileList(folderInfo->binaryFiles);
        wnd->setTitle(tr("Binary files"));
    }
    else if (link == "textFiles")
    {
        wnd->setFileList(folderInfo->textFiles);
        wnd->setTitle(tr("Text files"));
    }
    else if (link == "inaccessibleFiles")
    {
        wnd->setFileList(folderInfo->inaccessibleFiles);
        wnd->setTitle(tr("Inaccessible files"));
    }
    else if (link == "filesWithEol")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.trailingNewline; });
        wnd->setTitle(tr("Files with newline at the end"));
    }
    else if (link == "filesWithNoEol")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return !i.trailingNewline; });
        wnd->setTitle(tr("Files without newline at the end"));
    }
    else if (link == "filesWithWindowsNewlines")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.newlines == Newlines::Windows; });
        wnd->setTitle(tr("Files with Windows newlines"));
    }
    else if (link == "filesWithUnixNewlines")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.newlines == Newlines::Unix; });
        wnd->setTitle(tr("Files with Unix newlines"));
    }
    else if (link == "filesWithMixedNewlines")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.newlines == Newlines::Mixed; });
        wnd->setTitle(tr("Files with mixed newlines"));
    }
    else
    {
        qFatal("Broken link");
    }
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->setWindowFlag(Qt::Window);
    wnd->show();
}

void MainWindow::updateFolderInfo(FolderInfo info)
{
    folderInfo = info;
    ui->statusBar->clearMessage();
    ui->label_textFiles->setText(QString::number(info->textFiles.count()));
    ui->label_binaryFiles->setText(QString::number(info->binaryFiles.count()));
    ui->label_inaccessibleFiles->setText(QString::number(info->inaccessibleFiles.count()));
    ui->label_totalLines->setText(QString::number(info->totalLines));
    ui->label_filesWithEol->setText(QString::number(info->filesWithEol));
    ui->label_filesWithNoEol->setText(QString::number(info->filesWithNoEol));
    ui->label_filesWithWindowsNewlines->setText(QString::number(info->filesWithWindowsNewlines));
    ui->label_filesWithUnixNewlines->setText(QString::number(info->filesWithUnixNewlines));
    ui->label_filesWithMixedNewlines->setText(QString::number(info->filesWithMixedNewlines));
    ui->statusBar->showMessage(tr("%1 files analyzed").arg(info->textFiles.count() + info->binaryFiles.count() + info->inaccessibleFiles.count()));
}

MainWindow::~MainWindow()
{
    delete ui;
    processingThread.quit();
    processingThread.wait();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.count() > 0) {
        QString path = urls.at(0).toLocalFile();
        if (!path.isEmpty() && QDir(path).exists()) {
            QMetaObject::invokeMethod(&processor, "process", Q_ARG(QString, path));
        }
    }
    event->acceptProposedAction();
}
