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

const Language makefile{"Makefile", QList<QRegExp>{regexFromWildcard("Makefile")}};
const Language* const CommonLanguage::Makefile = &makefile;

const Language javascript{"JavaScript", QList<QRegExp>
    {
        regexFromWildcard("*.js"),
        regexFromWildcard("*.jsx"),
        regexFromWildcard("*.es5"),
        regexFromWildcard("*.es6"),
    }};
const Language* const CommonLanguage::JavaScript = &javascript;

const Language xml{"XML/HTML", QList<QRegExp>
    {
        regexFromWildcard("*.xml"),
        regexFromWildcard("*.html"),
        regexFromWildcard("*.xaml"),
        regexFromWildcard("*.csproj"),
        regexFromWildcard("*.vbproj"),
    }};
const Language* const CommonLanguage::Xml = &xml;

const Language* const CommonLanguage::All[] = {CSharp, CPlusPlus, Makefile, JavaScript, Xml};
