#include <QtTest>
#include <QSignalSpy>
#include "FolderProcessor.h"
#include "TextUtils.h"
#include "CommonLanguage.h"

class FolderProcessorTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testFileCount();
    void testEncodings();
    void testLines();
    void testRead();
    void testReadBinaryFile();
    void testLanguageDetection();
    void testWhitespaces();
    void testIndentation();

private:
    QDir dir{QFINDTESTDATA("TestData")};
};

void FolderProcessorTests::initTestCase()
{
    qRegisterMetaType<FolderInfo>();

    QVERIFY2(dir.exists(), "Failed to find test data");
}

#define QCOMPARE2(actual, expected, message) \
    do {\
        QByteArray msg = QString{"%1\nExpected: %2\nActual: %3"}.arg(message).arg(expected).arg(actual).toLocal8Bit();\
        QVERIFY2(actual == expected, msg.data());\
    } while(false)

#define TEST_DIR(DIRNAME) \
    FolderProcessor processor;\
    processor.addLanguages(CommonLanguage::All);\
    QSignalSpy spy{&processor, SIGNAL(doneProcessing(FolderInfo))};\
    QString dirPath = dir.filePath(DIRNAME);\
    QVERIFY(QDir(dirPath).exists());\
    \
    processor.process(dirPath);\
    \
    QCOMPARE(spy.count(), 1);\
    auto arguments = spy.takeFirst();\
    QCOMPARE(arguments.at(0).type(), QVariant::UserType);\
    FolderInfo result = qvariant_cast<FolderInfo>(arguments.at(0));\
    \
    QCOMPARE(result->folderPath, dirPath);\
    QCOMPARE(result->inaccessibleFiles.count(), 0);


void FolderProcessorTests::testFileCount()
{
    TEST_DIR("CSharp");

    QCOMPARE(result->filesByLanguage.keys().count(), 3);
    QVERIFY(result->filesByLanguage.contains(nullptr));
    QCOMPARE(result->filesByLanguage.value(nullptr), 1);
    QVERIFY(result->filesByLanguage.contains(CommonLanguage::CSharp));
    QCOMPARE(result->filesByLanguage.value(CommonLanguage::CSharp), 2);
    QVERIFY(result->filesByLanguage.contains(CommonLanguage::Xml));
    QCOMPARE(result->filesByLanguage.value(CommonLanguage::Xml), 1);
    QCOMPARE(result->textFiles.count(), 3);
    QCOMPARE(result->binaryFiles.count(), 1);
}

void FolderProcessorTests::testEncodings()
{
    TEST_DIR("Encodings");

    QCOMPARE(result->textFiles.count(), 6);
    QCOMPARE(result->binaryFiles.count(), 0);
    for (TextFileInfo fileInfo : result->textFiles)
    {
        if (fileInfo.path().endsWith("utf-8.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::NoBom);
        }
        else if (fileInfo.path().endsWith("utf-8bom.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::Utf8);
        }
        else if (fileInfo.path().endsWith("utf-16le.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::Utf16LE);
        }
        else if (fileInfo.path().endsWith("utf-16be.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::Utf16BE);
        }
        else if (fileInfo.path().endsWith("utf-32le.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::Utf32LE);
        }
        else if (fileInfo.path().endsWith("utf-32be.txt"))
        {
           QCOMPARE(fileInfo.encoding, Encoding::Utf32BE);
        }
        else
        {
            QFAIL("Unexpected filename");
        }

        QCOMPARE2(fileInfo.totalLines, 2, fileInfo.path());
    }
    QCOMPARE(result->totalLines, 12ul);
}

void FolderProcessorTests::testLines()
{
    TEST_DIR("Lines");

    QCOMPARE(result->textFiles.count(), 9);
    QCOMPARE(result->binaryFiles.count(), 0);
    for (TextFileInfo fileInfo : result->textFiles)
    {
        QString filename = QFileInfo(fileInfo.path()).fileName();
        if (filename == "unknown.txt")
        {
            QCOMPARE(fileInfo.newlines, Newlines::Unknown);
            QCOMPARE2(fileInfo.totalLines, 1, fileInfo.path());
            QCOMPARE(fileInfo.trailingNewline, false);
            continue;
        }

        if (filename.startsWith("dos-"))
        {
            QCOMPARE(fileInfo.newlines, Newlines::Windows);
            QCOMPARE2(fileInfo.totalLines, 3, fileInfo.path());
        }
        else if (filename.startsWith("unix-"))
        {
            QCOMPARE(fileInfo.newlines, Newlines::Unix);
            QCOMPARE2(fileInfo.totalLines, 3, fileInfo.path());
        }
        else if (filename.startsWith("mixed"))
        {
            QCOMPARE(fileInfo.newlines, Newlines::Mixed);
            QCOMPARE2(fileInfo.totalLines, 3, fileInfo.path());
        }
        else
        {
            QFAIL("Unexpected filename");
        }

        if (filename.endsWith("-eol.txt"))
        {
            QCOMPARE(fileInfo.trailingNewline, true);
        }
        else if (filename.endsWith("-noeol.txt"))
        {
            QCOMPARE(fileInfo.trailingNewline, false);
        }
        else
        {
            QFAIL("Unexpected filename");
        }

        QCOMPARE(fileInfo.emptyLines, 1u);
    }
    QCOMPARE(result->emptyLines, 8ul);
}

void FolderProcessorTests::testRead()
{
    TEST_DIR("Encodings");

    QCOMPARE(result->textFiles.count(), 6);
    QCOMPARE(result->binaryFiles.count(), 0);
    QString referenceText;
    QDir folder{result->folderPath};
    for (TextFileInfo fileInfo : result->textFiles)
    {
        QFile file{folder.filePath(fileInfo.path())};
        QVERIFY(file.open(QFile::ReadOnly));
        QString text;
        QVERIFY(readAllText(file, &text));
        if (referenceText.isEmpty())
        {
            referenceText = text;
        }
        else
        {
            QCOMPARE2(text, referenceText, fileInfo.path());
        }
    }
}

void FolderProcessorTests::testReadBinaryFile()
{
    TEST_DIR(".");
    QVERIFY(result->binaryFiles.count() > 0);
    QDir folder{result->folderPath};
    for (BinaryFileInfo fileInfo : result->binaryFiles)
    {
        QFile file{folder.filePath(fileInfo.path())};
        QVERIFY(file.open(QFile::ReadOnly));
        QString text;
        QVERIFY(!readAllText(file, &text));
    }
}

void FolderProcessorTests::testLanguageDetection()
{
    TEST_DIR(".");
    QVERIFY(result->textFiles.count() >= 3);
    for (TextFileInfo fileInfo : result->textFiles)
    {
        if (fileInfo.path().endsWith(".cs"))
        {
            QCOMPARE(fileInfo.language, CommonLanguage::CSharp);
        }
        else if (fileInfo.path().endsWith(".cpp"))
        {
            QCOMPARE(fileInfo.language, CommonLanguage::CPlusPlus);
        }
        else if (fileInfo.path().endsWith(".c"))
        {
            QCOMPARE(fileInfo.language, CommonLanguage::CPlusPlus);
        }
        else
        {
            QVERIFY(fileInfo.language != CommonLanguage::CSharp);
            QVERIFY(fileInfo.language != CommonLanguage::CPlusPlus);
        }
    }
}

void FolderProcessorTests::testWhitespaces()
{
    TEST_DIR("Whitespaces");

    QCOMPARE(result->textFiles.count(), 4);
    QCOMPARE(result->binaryFiles.count(), 0);
    for (TextFileInfo fileInfo : result->textFiles)
    {
        if (fileInfo.path().endsWith("_trailing.txt"))
        {
            QCOMPARE(fileInfo.totalLines, 7u);
            QCOMPARE(fileInfo.linesWithTrailSpaces, 5u);
            QCOMPARE(fileInfo.emptyLines, 2u);
        }
        else
        {
            QFAIL("Unexpected filename");
        }
    }
    QCOMPARE(result->filesWithTrailSpaces, 4u);
    QCOMPARE(result->emptyLines, 8ul);
    QCOMPARE(result->linesWithTrailSpaces, 20ul);
}

void FolderProcessorTests::testIndentation()
{
    TEST_DIR("Indentation");

    QCOMPARE(result->textFiles.count(), 5);
    QCOMPARE(result->binaryFiles.count(), 0);
    for (TextFileInfo fileInfo : result->textFiles)
    {
        QString filename = QFileInfo(fileInfo.path()).fileName();
        if (filename.startsWith("mixed"))
        {
            QCOMPARE(fileInfo.indentation, Indentation::Mixed);
        }
        else if (filename.startsWith("tabs"))
        {
            QCOMPARE(fileInfo.indentation, Indentation::Tabs);
        }
        else if (filename.startsWith("spaces"))
        {
            QCOMPARE(fileInfo.indentation, Indentation::Spaces);
        }
        else if (filename.startsWith("unknown"))
        {
            QCOMPARE(fileInfo.indentation, Indentation::Unknown);
        }
        else
        {
            QFAIL("Unexpected filename");
        }
    }
    QCOMPARE(result->filesWithTabIndent, 1u);
    QCOMPARE(result->filesWithSpaceIndent, 1u);
    QCOMPARE(result->filesWithMixedIndent, 2u);
}


QTEST_APPLESS_MAIN(FolderProcessorTests)

#include "tst_folderprocessor.moc"
