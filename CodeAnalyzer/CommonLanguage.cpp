#include "CommonLanguage.h"

namespace
{
    Language* createCpp()
    {
        QRegExp cRegex{"*.c"};
        cRegex.setPatternSyntax(QRegExp::Wildcard);
        QRegExp cppRegex{"*.cpp"};
        cppRegex.setPatternSyntax(QRegExp::Wildcard);
        QRegExp hRegex{"*.h"};
        hRegex.setPatternSyntax(QRegExp::Wildcard);
        // TODO: Ever destroy the object?
        return new Language{"C/C++", QList<QRegExp>{cppRegex, cRegex, hRegex}};
    }

    Language* createCSharp()
    {
        QRegExp csRegex{"*.cs"};
        csRegex.setPatternSyntax(QRegExp::Wildcard);
        // TODO: Ever destroy the object?
        return new Language{"C#", QList<QRegExp>{csRegex}};
    }
}

Language* CommonLanguage::CPlusPlus{createCpp()};
Language* CommonLanguage::CSharp{createCSharp()};
