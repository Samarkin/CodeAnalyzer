#include <QtTest>
#include <QSignalSpy>
#include "FolderProcessor.h"

class FolderProcessorTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void count_files();

private:
    QDir dir{QFINDTESTDATA("TestData")};
};

void FolderProcessorTests::initTestCase()
{
    qRegisterMetaType<FolderInfo>();

    QVERIFY2(dir.exists(), "Failed to find test data");
}

void FolderProcessorTests::count_files()
{
    FolderProcessor processor;
    QSignalSpy spy{&processor, SIGNAL(doneProcessing(FolderInfo))};
    QString dirPath = dir.filePath("CSharp");
    QVERIFY(QDir(dirPath).exists());

    processor.process(dirPath);

    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::UserType);
    FolderInfo result = qvariant_cast<FolderInfo>(arguments.at(0));

    QCOMPARE(result->folderPath, dirPath);
    QCOMPARE(result->filesByExt.keys().count(), 3);
    QVERIFY(result->filesByExt.contains("csproj"));
    QCOMPARE(result->filesByExt.value("csproj"), 1);
    QVERIFY(result->filesByExt.contains("cs"));
    QCOMPARE(result->filesByExt.value("cs"), 2);
    QVERIFY(result->filesByExt.contains("dll"));
    QCOMPARE(result->filesByExt.value("dll"), 1);
    QCOMPARE(result->totalFiles, 4);
}

QTEST_APPLESS_MAIN(FolderProcessorTests)

#include "tst_folderprocessor.moc"
