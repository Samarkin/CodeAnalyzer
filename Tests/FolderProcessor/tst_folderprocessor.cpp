#include <QtTest>
#include <QSignalSpy>
#include "FolderProcessor.h"

class FolderProcessorTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testFileCount();
    void testEncodings();

private:
    QDir dir{QFINDTESTDATA("TestData")};
};

void FolderProcessorTests::initTestCase()
{
    qRegisterMetaType<FolderInfo>();

    QVERIFY2(dir.exists(), "Failed to find test data");
}

#define TEST_DIR(DIRNAME) \
    FolderProcessor processor;\
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

    QCOMPARE(result->filesByExt.keys().count(), 3);
    QVERIFY(result->filesByExt.contains("csproj"));
    QCOMPARE(result->filesByExt.value("csproj"), 1);
    QVERIFY(result->filesByExt.contains("cs"));
    QCOMPARE(result->filesByExt.value("cs"), 2);
    QVERIFY(result->filesByExt.contains("dll"));
    QCOMPARE(result->filesByExt.value("dll"), 1);
    QCOMPARE(result->textFiles.count() + result->binaryFiles.count() + result->inaccessibleFiles.count(), 4);
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
    }
}

QTEST_APPLESS_MAIN(FolderProcessorTests)

#include "tst_folderprocessor.moc"
