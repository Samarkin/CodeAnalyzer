#include "Language.h"

bool Language::checkFile(QString filename) const
{
    for (const QRegExp& re : files)
    {
        if (re.exactMatch(filename))
        {
            return true;
        }
    }
    return false;
}
