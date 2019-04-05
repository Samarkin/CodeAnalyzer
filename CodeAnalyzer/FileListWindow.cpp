#include "FileListWindow.h"
#include "ui_FileListWindow.h"
#include "FileInfo.h"
#include "TextUtils.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDir>

enum ItemDataRole
{
    LanguageRole = Qt::UserRole + 1,
};

template<>
QVariant FileListItem<FileInfo>::data(int role) const
{
    if (role == Qt::DisplayRole) return fileInfo.path();
    if (role == LanguageRole) return "Unknown";
    else return QStandardItem::data(role);
}

template<>
QVariant FileListItem<BinaryFileInfo>::data(int role) const
{
    if (role == Qt::DisplayRole) return fileInfo.path();
    if (role == LanguageRole) return "Binary file";
    else return QStandardItem::data(role);
}

template<>
QVariant FileListItem<TextFileInfo>::data(int role) const
{
    if (role == Qt::DisplayRole) return fileInfo.path();
    if (role == LanguageRole) return fileInfo.language ? fileInfo.language->name : "Plain text";
    else return QStandardItem::data(role);
}

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
    QString language = current.data(LanguageRole).toString();
    ui->languageValue->setText(language);
    QString fullPath = QDir{folderPath}.filePath(path);
    QFile file{fullPath};
    QTextDocument *doc = ui->textBrowser->document();
    // Read file
    if (!file.open(QFile::ReadOnly))
    {
        doc->setHtml(tr("<span style='color:red'>&lt;Failed to open file&gt;</span>"));
        return;
    }

    if (!readAllText(file, doc)) {
        doc->setHtml(tr("<span style='color:red'>&lt;Binary file&gt;</span>"));
    }
}
