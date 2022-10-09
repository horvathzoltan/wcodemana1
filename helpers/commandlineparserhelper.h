#ifndef COMMANLINEPARSERHELPER_H
#define COMMANLINEPARSERHELPER_H

#include <QCommandLineParser>
namespace helper{
class CommandLineParserHelper
{
public:
    CommandLineParserHelper();
    static bool addOption(QCommandLineParser *p, const QString& o, const QString& d);
    static bool addOptionBool(QCommandLineParser *p, const QString& o, const QString& d);
};
}

#endif // COMMANLINEPARSERHELPER_H
