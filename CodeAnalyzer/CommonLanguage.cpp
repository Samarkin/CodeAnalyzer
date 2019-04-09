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
        regexFromWildcard("*.ipp"),
        regexFromWildcard("*.moc"),
        regexFromWildcard("*.inl"),
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

const Language css{"CSS", QList<QRegExp>{regexFromWildcard("*.css")}};
const Language* const CommonLanguage::CSS = &css;

const Language go{"Go", QList<QRegExp>{regexFromWildcard("*.go")}};
const Language* const CommonLanguage::Go = &go;

const Language makefile{"Makefile", QList<QRegExp>
    {
        regexFromWildcard("[mM]akefile"),
        regexFromWildcard("*.mk"),
        regexFromWildcard("*.mak"),
    }};
const Language* const CommonLanguage::Makefile = &makefile;

const Language java{"Java", QList<QRegExp>
    {
        regexFromWildcard("*.java"),
        regexFromWildcard("*.jav"),
    }};
const Language* const CommonLanguage::Java = &java;

const Language javascript{"JavaScript", QList<QRegExp>
    {
        regexFromWildcard("*.js"),
        regexFromWildcard("*.mjs"),
        regexFromWildcard("*.jsx"),
        regexFromWildcard("*.es"),
        regexFromWildcard("*.es5"),
        regexFromWildcard("*.es6"),
    }};
const Language* const CommonLanguage::JavaScript = &javascript;

const Language json{"JSON", QList<QRegExp>
    {
        regexFromWildcard("*.json"),
        regexFromWildcard("*.jsonp"),
    }};
const Language* const CommonLanguage::Json = &json;

const Language python{"Python", QList<QRegExp>
    {
        regexFromWildcard("*.py"),
        regexFromWildcard("*.pyw"),
        regexFromWildcard("*.pyi"),
    }};
const Language* const CommonLanguage::Python = &python;

const Language swift{"Swift", QList<QRegExp>{regexFromWildcard("*.swift")}};
const Language* const CommonLanguage::Swift = &swift;

const Language markdown{"Markdown", QList<QRegExp>
    {
        regexFromWildcard("*.md"),
        regexFromWildcard("*.mdwn"),
        regexFromWildcard("*.markdown"),
        regexFromWildcard("*.mdown"),
        regexFromWildcard("*.mkdn"),
        regexFromWildcard("*.mkd"),
    }};
const Language* const CommonLanguage::Markdown = &markdown;

const Language xml{"XML/HTML", QList<QRegExp>
    {
        regexFromWildcard("*.xml"),
        regexFromWildcard("*.html"),
        regexFromWildcard("*.xaml"),
        regexFromWildcard("*.csproj"),
        regexFromWildcard("*.vbproj"),
        regexFromWildcard("*.plist"),
        regexFromWildcard("*.storyboard"),
        regexFromWildcard("*.xcscheme"),
        regexFromWildcard("*.xcbkptlist"),
        regexFromWildcard("*.xcworkspacedata"),
    }};
const Language* const CommonLanguage::Xml = &xml;

const Language* const CommonLanguage::All[] = {CSharp, CPlusPlus, CSS, Go, Makefile, Java, JavaScript, Json, Python, Swift, Markdown, Xml};
