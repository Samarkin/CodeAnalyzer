#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "FileListWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include "CommonLanguage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(width(), minimumSizeHint().height());

    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(openClicked()));
    connectLinkHandler(ui->label);
    connectLinkHandler(ui->label_2);
    connectLinkHandler(ui->label_3);
    connectLinkHandler(ui->label_4);
    connectLinkHandler(ui->label_5);
    connectLinkHandler(ui->label_6);
    connectLinkHandler(ui->label_7);
    connectLinkHandler(ui->label_8);
    connectLinkHandler(ui->label_9);
    connectLinkHandler(ui->label_10);
    connectLinkHandler(ui->label_11);
    connectLinkHandler(ui->label_12);
    connectLinkHandler(ui->label_13);
    connectLinkHandler(ui->label_14);
    connectLinkHandler(ui->label_15);

    processor.addLanguages(CommonLanguage::All);
    processor.moveToThread(&processingThread);
    connect(&processor, SIGNAL(doneProcessing(FolderInfo)),
            this, SLOT(updateFolderInfo(FolderInfo)));
    connect(&processor, SIGNAL(startingProcessing(QString)),
            this, SLOT(indicateProcessing()));
    processingThread.start();
}

inline void MainWindow::connectLinkHandler(const QObject *object)
{
    connect(object, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)));
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
    processingTimer.start();
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
    else if (link == "filesWithTrailSpaces")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.linesWithTrailSpaces > 0; });
        wnd->setTitle(tr("Files with trailing whitespaces"));
    }
    else if (link == "filesWithTabIndent")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.indentation == Indentation::Tabs; });
        wnd->setTitle(tr("Files indented with tabs"));
    }
    else if (link == "filesWithSpaceIndent")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.indentation == Indentation::Spaces; });
        wnd->setTitle(tr("Files indented with spaces"));
    }
    else if (link == "filesWithMixedIndent")
    {
        wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.indentation == Indentation::Mixed; });
        wnd->setTitle(tr("Files with mixed indentation"));
    }
    else if (link.startsWith("language"))
    {
        QString langName = link.mid(8);
        const Language* language = nullptr;
        for (auto lang : CommonLanguage::All)
        {
            if (lang->urlSafeName == langName)
            {
                language = lang;
            }
        }
        if (language == nullptr)
        {
            wnd->setFileList(folderInfo->textFiles, [](const TextFileInfo& i) { return i.language == nullptr; });
            wnd->setTitle(tr("Plain text"));
        }
        else
        {
            wnd->setFileList(folderInfo->textFiles, [language](const TextFileInfo& i) { return i.language == language; });
            wnd->setTitle(language->name);
        }
    }
    else if (link.startsWith("encoding"))
    {
        QString encodingNumber = link.mid(8);
        bool ok;
        int encodingInt = encodingNumber.toInt(&ok);
        if (!ok || encodingInt < 0 || encodingInt >= TotalNumberOfEncodings) qFatal("Invalid encoding link");
        auto encoding = Encoding(encodingInt);
        wnd->setFileList(folderInfo->textFiles, [encoding](const TextFileInfo& i) { return i.encoding == encoding; });
        wnd->setTitle(getEncodingName(encoding));
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
    qint64 elapsed = processingTimer.elapsed();
    folderInfo = info;
    ui->statusBar->clearMessage();
    QLocale systemLocale = QLocale::system();
    ui->label_textFiles->setText(systemLocale.toString(info->textFiles.count()));
    ui->label_binaryFiles->setText(systemLocale.toString(info->binaryFiles.count()));
    ui->label_inaccessibleFiles->setText(systemLocale.toString(info->inaccessibleFiles.count()));
    ui->label_totalLines->setText(systemLocale.toString(info->totalLines));
    ui->label_emptyLines->setText(systemLocale.toString(info->emptyLines));
    ui->label_linesWithTrailSpaces->setText(systemLocale.toString(info->linesWithTrailSpaces));
    ui->label_filesWithTrailSpaces->setText(systemLocale.toString(info->filesWithTrailSpaces));
    ui->label_filesWithEol->setText(systemLocale.toString(info->filesWithEol));
    ui->label_filesWithNoEol->setText(systemLocale.toString(info->filesWithNoEol));
    ui->label_filesWithWindowsNewlines->setText(systemLocale.toString(info->filesWithWindowsNewlines));
    ui->label_filesWithUnixNewlines->setText(systemLocale.toString(info->filesWithUnixNewlines));
    ui->label_filesWithMixedNewlines->setText(systemLocale.toString(info->filesWithMixedNewlines));
    ui->label_filesWithTabIndent->setText(systemLocale.toString(info->filesWithTabIndent));
    ui->label_filesWithSpaceIndent->setText(systemLocale.toString(info->filesWithSpaceIndent));
    ui->label_filesWithMixedIndent->setText(systemLocale.toString(info->filesWithMixedIndent));
    int totalFiles = info->textFiles.count() + info->binaryFiles.count() + info->inaccessibleFiles.count();
    ui->statusBar->showMessage(
        tr("%1 files analyzed in %2 milliseconds")
            .arg(systemLocale.toString(totalFiles))
            .arg(systemLocale.toString(elapsed))
        );

    int rowCount = ui->languageFormLayout->rowCount();
    for (int i = 0; i < rowCount; i++)
    {
        ui->languageFormLayout->removeRow(0);
    }
    for (auto it = info->filesByLanguage.begin(), end = info->filesByLanguage.end(); it != end; ++it)
    {
        auto language = it.key();
        auto numOfFiles = it.value();
        auto langNameLabel = new QLabel{
            language
                ? QStringLiteral("<a href=\"language%2\">%1</a>")
                    .arg(language->name)
                    .arg(language->urlSafeName)
                : QStringLiteral("<a href=\"language_unknown\">%1</a>")
                    .arg(tr("Plain text"))
        };
        auto numLabel = new QLabel{systemLocale.toString(numOfFiles)};
        connectLinkHandler(langNameLabel);
        ui->languageFormLayout->addRow(langNameLabel, numLabel);
    }
    auto spacer = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    ui->languageFormLayout->addItem(spacer);
    for (int i = 0; i < TotalNumberOfEncodings; i++)
    {
        int files = info->filesByEncoding[i];
        if (files != 0)
        {
            auto encodingLabel = new QLabel{
                QStringLiteral("<a href=\"encoding%2\">%1</a>")
                    .arg(getEncodingName(Encoding(i)))
                    .arg(i)
            };
            auto numLabel = new QLabel{systemLocale.toString(files)};
            connectLinkHandler(encodingLabel);
            ui->languageFormLayout->addRow(encodingLabel, numLabel);
        }
    }
}

QString MainWindow::getEncodingName(Encoding encoding)
{
    switch (encoding) {
    case Encoding::NoBom: return tr("No BOM");
    case Encoding::Utf8: return tr("UTF-8");
    case Encoding::Utf16BE: return tr("UTF-16BE");
    case Encoding::Utf16LE: return tr("UTF-16LE");
    case Encoding::Utf32BE: return tr("UTF-32BE");
    case Encoding::Utf32LE: return tr("UTF-32LE");
    }
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
