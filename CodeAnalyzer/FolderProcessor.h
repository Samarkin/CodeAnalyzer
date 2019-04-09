#ifndef FOLDERPROCESSOR_H
#define FOLDERPROCESSOR_H

#include <QObject>
#include <QLinkedList>
#include <FolderInfo.h>

class FolderProcessor : public QObject
{
    Q_OBJECT

public:
    void addLanguage(const Language* const language);
    template<size_t count>
    void addLanguages(const Language* const (&languages)[count])
    {
        for (size_t i = 0; i < count; i++)
        {
            addLanguage(languages[i]);
        }
    }


public slots:
    void process(const QString folderPath);

signals:
    void startingProcessing(const QString folderPath);
    void doneProcessing(FolderInfo info);

private:
    QLinkedList<const Language*> languages;
};

#endif // FOLDERPROCESSOR_H
