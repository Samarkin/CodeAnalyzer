#include "FileListWindow.h"
#include "ui_FileListWindow.h"
#include "TextUtils.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDir>

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

    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
}

FileListWindow::~FileListWindow()
{
    delete ui;
}

void FileListWindow::setTitle(QString title)
{
    setWindowTitle(tr("%1 - %2").arg(folderPath).arg(title));
}

void FileListWindow::selectionChanged(const QModelIndex& current, const QModelIndex&)
{
    QString path = current.data().toString();
    if (path == folderPath)
    {
        // Root selected
        return;
    }
    QString fullPath = QDir{folderPath}.filePath(path);
    QFile file{fullPath};
    // Read file
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open %1").arg(path));
        return;
    }

    QTextDocument *doc = new QTextDocument{ui->textBrowser};
    if (!readAllText(file, doc)) {
        doc->setHtml(tr("<span style='color:red'>&lt;Failed to open file&gt;</span>"));
    }
    ui->textBrowser->setDocument(doc);
}
