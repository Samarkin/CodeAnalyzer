#include "FileListWindow.h"
#include "ui_FileListWindow.h"
#include "FileInfo.h"
#include "TextUtils.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QFontDatabase>
#include <QDir>

namespace
{
    template<bool (*getCodePoint)(QFile& file, QChar* pChar)>
    struct readAsDocument
    {
        typedef QTextDocument return_t;
        static bool read(QFile& file, return_t* document)
        {
            document->clear();
            QTextCursor cursor{document};
            QTextCharFormat clear{};
            clear.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
            QTextCharFormat highlighted{};
            highlighted.setForeground(QBrush{QColor{Qt::white}});
            highlighted.setBackground(QBrush{QColor{Qt::red}});
            QTextCharFormat trailing{};
            trailing.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
            trailing.setForeground(QBrush{QColor{Qt::gray}});
            trailing.setBackground(QBrush{QColor{Qt::yellow}});
            QTextCharFormat invisibles{};
            invisibles.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
            invisibles.setForeground(QBrush{QColor{Qt::gray}});

            QVarLengthArray<QChar> chars;
            QVarLengthArray<QChar> whiteSpaces;
            QChar codePoint{};
            auto flush = [&chars,&cursor,&clear]() {
                if (!chars.empty())
                {
                    chars.append(QChar{});
                    cursor.insertText(QString{chars.data()}, clear);
                    chars.clear();
                }
            };
            auto flushWhitespaces = [&whiteSpaces,&cursor,&trailing]() {
                for (int i = 0; i < whiteSpaces.count(); i++)
                {
                    if (whiteSpaces.at(i) == '\t') whiteSpaces.replace(i, 0x27F6); // arrow for tab
                    else if (whiteSpaces.at(i) == ' ') whiteSpaces.replace(i, 0xB7); // dot for space
                    else if (whiteSpaces.at(i) == 160) whiteSpaces.replace(i, 0xBA); // circle for nbsp
                }
                whiteSpaces.append(QChar{});
                cursor.insertText(QString{whiteSpaces.data()}, trailing);
                whiteSpaces.clear();
            };
            while (getCodePoint(file, &codePoint))
            {
                if (!isTextCodePoint(codePoint)) return false;
                if (isWhitespaceCodePoint(codePoint))
                {
                    whiteSpaces.append(codePoint);
                    continue;
                }
                if (!whiteSpaces.empty())
                {
                    if (codePoint == '\r' || codePoint == '\n')
                    {
                        flush();
                        flushWhitespaces();
                    }
                    else
                    {
                        for (const QChar& ch : whiteSpaces)
                        {
                            if (ch == '\t')
                            {
                                flush();
                                cursor.insertText(QStringLiteral("⟶"), invisibles);
                                chars.append('\t');
                            }
                            else
                            {
                                chars.append(ch);
                            }
                        }
                    }
                    whiteSpaces.clear();
                }
                if (codePoint == '\r')
                {
                    flush();
                    cursor.insertText(QStringLiteral("\\r"), highlighted);
                }
                else
                {
                    chars.append(codePoint);
                }
            }
            flush();
            if (codePoint != '\n')
            {
                if (!whiteSpaces.empty())
                {
                    flushWhitespaces();
                }
                cursor.insertText(QStringLiteral("\nNo newline at end of file"), highlighted);
            }
            return true;
        }
    };

    inline bool readAllText(QFile& file, QTextDocument *document)
    {
        return readAll<readAsDocument>(file, document);
    }
}

template<>
QString FileListItem<FileInfo>::getLanguage(const QObject& translator) const
{
    return translator.tr("Unknown");
}

template<>
QString FileListItem<BinaryFileInfo>::getLanguage(const QObject& translator) const
{
    return translator.tr("Binary file");
}

template<>
QString FileListItem<TextFileInfo>::getLanguage(const QObject& translator) const
{
    return fileInfo.language ? fileInfo.language->name : translator.tr("Plain text");
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
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
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
    QString language = current.data(FileListItemDataRole::LanguageRole).toString();
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
