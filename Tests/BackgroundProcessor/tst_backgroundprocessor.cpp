#include <QtTest>
#include "BackgroundProcessor.h"

class BackgroundProcessorTests : public QObject
{
    Q_OBJECT

private slots:
    void testSingleThread();
    void testMultiThread();
    void testSlowProducer();
    void testSlowConsumers();
    void testExtraWait();
};

void BackgroundProcessorTests::testSingleThread()
{
    QMutex mu;
    int sum{0};
    BackgroundProcessor<int> processor{[&mu, &sum](const int& a)
    {
        QMutexLocker lock{&mu};
        qDebug() << "Processing " << a << "...\n";
        sum += a;
    }, 1};

    for (int i = 0; i < 10; i++)
    {
        processor.enqueue(i+1);
    }
    processor.wait();
    QCOMPARE(sum, 55);
}

void BackgroundProcessorTests::testMultiThread()
{
    QMutex mu;
    int sum{0};
    BackgroundProcessor<int> processor{[&mu, &sum](const int& a)
    {
        QMutexLocker lock{&mu};
        qDebug() << "Processing " << a << "...\n";
        sum += a;
    }};

    for (int i = 0; i < 10; i++)
    {
        processor.enqueue(i+1);
    }
    processor.wait();
    QCOMPARE(sum, 55);
}

void BackgroundProcessorTests::testSlowProducer()
{
    QMutex mu;
    int sum{0};
    BackgroundProcessor<int> processor{[&mu, &sum](const int& a)
    {
        QMutexLocker lock{&mu};
        qDebug() << "Processing " << a << "...\n";
        sum += a;
    }};

    for (int i = 0; i < 10; i++)
    {
        QThread::msleep(100);
        processor.enqueue(i+1);
    }
    processor.wait();
    QCOMPARE(sum, 55);
}

void BackgroundProcessorTests::testSlowConsumers()
{
    QMutex mu;
    int sum{0};
    BackgroundProcessor<int> processor{[&mu, &sum](const int& a)
    {
        QMutexLocker lock{&mu};
        qDebug() << "Processing " << a << "...\n";
        QThread::msleep(100);
        sum += a;
    }};

    for (int i = 0; i < 10; i++)
    {
        processor.enqueue(i+1);
    }
    processor.wait();
    QCOMPARE(sum, 55);
}

void BackgroundProcessorTests::testExtraWait()
{
    QMutex mu;
    int sum{0};
    BackgroundProcessor<int> processor{[&mu, &sum](const int& a)
    {
        QMutexLocker lock{&mu};
        qDebug() << "Processing " << a << "...\n";
        sum += a;
    }};

    for (int i = 0; i < 10; i++)
    {
        processor.enqueue(i+1);
    }
    processor.wait();
    processor.wait();
    QCOMPARE(sum, 55);
}


QTEST_APPLESS_MAIN(BackgroundProcessorTests)

#include "tst_backgroundprocessor.moc"
