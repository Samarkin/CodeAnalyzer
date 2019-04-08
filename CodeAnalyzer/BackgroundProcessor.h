#ifndef BACKGROUNDPROCESSOR_H
#define BACKGROUNDPROCESSOR_H

#include <QThread>
#include <QSemaphore>

template<typename T>
class BackgroundProcessor
{
    class BackgroundConsumerThread: public QThread
    {
        void run() override final
        {
            T item;
            while (processor->dequeue(&item))
            {
                consume_one(item);
            }
        }
    public:
        BackgroundConsumerThread() {}
        std::function<void(const T&)> consume_one;
        BackgroundProcessor* processor;
    };

public:
    BackgroundProcessor(std::function<void(const T&)> consumer, int numOfThreads = QThread::idealThreadCount(), int bufferSize = 10000)
        : empty{bufferSize},
          notEmpty{0},
          buffer{new T[bufferSize]},
          bufferHead{0},
          bufferTail{0},
          bufferSize{bufferSize},
          threads{new BackgroundConsumerThread[numOfThreads]},
          numOfThreads(numOfThreads)
    {
        for (int i = 0; i < numOfThreads; i++)
        {
            threads[i].processor = this;
            threads[i].consume_one = consumer;
            threads[i].start();
        }
    }
    ~BackgroundProcessor()
    {
        wait();
        delete[] threads;
        delete[] buffer;
    }
    void enqueue(T item)
    {
        empty.acquire();
        QMutexLocker lock{&bufferMutex};
        buffer[bufferHead] = item;
        bufferHead = (bufferHead+1)%bufferSize;
        notEmpty.release();
    }
    void wait()
    {
        // release all notEmpty semaphores without acquiring empty - exit signal
        notEmpty.release(numOfThreads);
        for (int i = 0; i < numOfThreads; i++)
        {
            threads[i].wait();
        }
    }

private:
    QMutex bufferMutex;
    QSemaphore empty;
    QSemaphore notEmpty;
    T* const buffer;
    int bufferHead;
    int bufferTail;
    const int bufferSize;

    BackgroundConsumerThread* const threads;
    const int numOfThreads;

    bool dequeue(T* item)
    {
        notEmpty.acquire();
        QMutexLocker lock{&bufferMutex};
        if (empty.available() == bufferSize)
        {
            // The entire buffer is empty, but someone released a notEmpty semaphore - exit signal
            return false;
        }
        *item = buffer[bufferTail];
        bufferTail = (bufferTail+1)%bufferSize;
        empty.release();
        return true;
    }
};

#endif // BACKGROUNDPROCESSOR_H
