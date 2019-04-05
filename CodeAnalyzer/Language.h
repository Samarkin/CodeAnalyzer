#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QString>
#include <QRegExp>
#include <QList>

struct Language
{
    Language(QString name, const QList<QRegExp>& files) : name(name), files(files) {}
    QString name;
    QList<QRegExp> files;

    bool checkFile(QString filename) const;

private:
    Language(const Language&);
};

#endif // LANGUAGE_H
