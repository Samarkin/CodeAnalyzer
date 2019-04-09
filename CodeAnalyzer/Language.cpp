#include "Language.h"

#include <QUrl>

bool Language::checkFile(QString filename) const
{
    for (QRegExp re : files)
    {
        if (re.exactMatch(filename))
        {
            return true;
        }
    }
    return false;
}

Language::Language(QString name, const QList<QRegExp>& files)
    : name(name), urlSafeName(QUrl::toPercentEncoding(name)), files(files)
{
}
