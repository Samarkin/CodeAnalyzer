#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QString>
#include <QRegExp>
#include <QList>

struct Language
{
    QString name;
    QList<QRegExp> files;

    bool checkFile(QString filename);
};

#endif // LANGUAGE_H
