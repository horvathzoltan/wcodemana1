#ifndef LOG_H
#define LOG_H

#include "locinfo.h"
#include <QString>

#if defined(__GNUC__)
#define getLocInfo helper::LocInfo(static_cast<const char*>(__PRETTY_FUNCTION__) ,__FILE__,__LINE__)
#elif defined(__MINGW32__)
#define getLocInfo LocInfo(__PRETTY_FUNCTION__ ,__FILE__,__LINE__)
#elif defined (_MSC_VER )
#define getLocInfo LocInfo(__FUNCSIG__ ,__FILE__,__LINE__)
#endif

#define zInfo(msg) helper::Log::info2((msg), getLocInfo);

namespace Errlevels{
enum Levels:int {
    ERROR_,
    WARNING,
    INFO,
};

[[gnu::unused]]
static QString toString  (const Levels &l){
    switch(l)
    {
    case ERROR_: return QStringLiteral("ERROR");
    case WARNING: return QStringLiteral("WARNING");
    case INFO: return QStringLiteral("INFO");
    default: return QStringLiteral("unknown");
    }
};
};

namespace Dbglevels{
enum Levels:int {
    NONE=0,
    TRACE,
    DEBUG,
//    INFOAPPEND,
//    INFOCLOSE
};

[[gnu::unused]]
static QString toString  (const Levels &l){
    switch(l){
        case NONE: return QStringLiteral("TRACE");
        case TRACE: return QStringLiteral("TRACE");
        case DEBUG: return QStringLiteral("DEBUG");
//    case INFOAPPEND: return QStringLiteral("INFO");
//    case INFOCLOSE: return QStringLiteral("INFO");
        default: return QStringLiteral("unknown");
    }
};
}

namespace GUIModes{
enum Modes:int{ INFO, INFOAPPEND, INFOCLOSE, WARNING, ERROR, DEBUG};
}

typedef void (*zLogGUIfn)(GUIModes::Modes mode, const QString &msg, const QString &loci, const QString &st, void *ui, int type);

namespace helper
{
class Log
{
private:
    static zLogGUIfn _GUILogger;
    static void *_ui;
    static bool _isVerbose;
    static Errlevels::Levels _errlevel;
    static Dbglevels::Levels _dbglevel;

    static QString ToErrorString(Errlevels::Levels, const QString&, const QString&, const QString&);
    static QString ToDebugString(Dbglevels::Levels level, const QString &msg, const QString &loci, const QString &st);

    static void err_message(Errlevels::Levels level, const QString& msg, int flag =0);
    static void dbg_message(Dbglevels::Levels level, const QString& msg, int flag =0);

public:
    static void info2(const QString& msg, const LocInfo& l, int flag = 0);
    static void info2(const QStringList& msg, const LocInfo& l, int flag = 0);
};
}
#endif // LOG_H
