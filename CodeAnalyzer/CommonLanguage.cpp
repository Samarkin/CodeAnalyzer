#include "CommonLanguage.h"

namespace
{
    QRegExp regexFromWildcard(const QString& wildcard)
    {
        QRegExp regex{wildcard};
        regex.setCaseSensitivity(Qt::CaseInsensitive);
        regex.setPatternSyntax(QRegExp::Wildcard);
        return regex;
    }
}

const Language cPlusPlus{"C/C++", QList<QRegExp>
    {
        regexFromWildcard("*.cpp"),
        regexFromWildcard("*.hpp"),
        regexFromWildcard("*.cxx"),
        regexFromWildcard("*.c++"),
        regexFromWildcard("*.cp"),
        regexFromWildcard("*.cc"),
        regexFromWildcard("*.hh"),
        regexFromWildcard("*.c"),
        regexFromWildcard("*.h"),
    }};
const Language* const CommonLanguage::CPlusPlus = &cPlusPlus;

const Language cSharp{"C#", QList<QRegExp>{regexFromWildcard("*.cs")}};
const Language* const CommonLanguage::CSharp = &cSharp;