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
    QTemporaryDir dir;
};

void FolderProcessorTests::initTestCase()
{
    qRegisterMetaType<FolderInfo>();

    QVERIFY2(dir.isValid(), "Failed to create temporary directory: ");

    QFile prFile{dir.filePath("Hello.csproj")};
    prFile.open(QFile::WriteOnly);
    prFile.write("One-line ASCII file");

    QFile srcFile1{dir.filePath("Main.cs")};
    srcFile1.open(QFile::WriteOnly);
    srcFile1.write("Multi-line\nfile\nwith\nunix line-endings\nand a trailing endline");
}

void FolderProcessorTests::count_files()
{
    FolderProcessor processor;
    QSignalSpy spy{&processor, SIGNAL(doneProcessing(FolderInfo))};
    QString dirPath = dir.path();

    processor.process(dirPath);

    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::UserType);
    FolderInfo result = qvariant_cast<FolderInfo>(arguments.at(0));

    QCOMPARE(result->folderPath, dirPath);
    QCOMPARE(result->totalFiles, 2);
}

QTEST_APPLESS_MAIN(FolderProcessorTests)

#include "tst_folderprocessor.moc"
