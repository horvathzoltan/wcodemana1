#include "log.h"
#include "qdebug.h"
#include <QList>

namespace helper
{
zLogGUIfn Log::_GUILogger = nullptr;
void* Log::_ui = nullptr;
bool Log::_isVerbose = false;
Errlevels::Levels Log:: _errlevel = Errlevels::INFO;
Dbglevels::Levels Log:: _dbglevel = Dbglevels::TRACE;

QString Log::ToDebugString(Dbglevels::Levels level, const QString &msg, const QString &loci, const QString &st){
    auto txt = Dbglevels::toString(level);
    QString msg3;
    switch(level){
    case Dbglevels::TRACE:
        msg3= txt+": "+loci;
        break;
    case Dbglevels::DEBUG:
        msg3= txt+": "+msg+"\n"+loci+"\n"+st;
        break;
    default: break;
    }
    return msg3;
}

QString Log::ToErrorString(Errlevels::Levels errlevel, const QString &msg, const QString &loci, const QString &st)
{
    auto level = Errlevels::toString(errlevel);
    QString msg3;

    switch(errlevel)
    {
    case Errlevels::ERROR_:
        msg3= level+": "+msg+"\n"+loci+"\n"+st;
        break;
    case Errlevels::WARNING:
        msg3= level+": "+msg+"\n"+loci;
        break;

    case Errlevels::INFO:
        msg3= level+": "+msg;
        if(_isVerbose) msg3+="\n"+loci;
        break;
    default: break;
    }

    return msg3;
}

void Log::info2(const QString& msg, const LocInfo& locinfo, int flag)
{
    if(_errlevel>Errlevels::INFO) return;
    QString li;
    if(_isVerbose)
    {
        li = locinfo.toString();
    }

    auto msg2 = ToErrorString(Errlevels::INFO, msg, li, nullptr);
    err_message(Errlevels::INFO, msg, flag);
}

void Log::info2(const QStringList& msgl, const LocInfo& locinfo, int flag)
{
    QString li;
    if(_isVerbose)
    {
        li = locinfo.toString();
    }

    for(auto&msg:msgl)
    {
        auto msg2 = ToErrorString(Errlevels::INFO, msg, nullptr, nullptr);
        err_message(Errlevels::INFO, msg2, flag);
    }
}

void Log::err_message(Errlevels::Levels level, const QString& msg, int flag)
{
    GUIModes::Modes guimode;

#ifdef QT_DEBUG
#ifdef Q_OS_WIN
    auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    switch(level)
    {
    case Errlevels::ERROR_:
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).critical().noquote()<<msg;
        guimode = GUIModes::ERROR;
        break;
    case Errlevels::WARNING:
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).warning().noquote()<<msg;
        guimode = GUIModes::WARNING;
        break;
    case Errlevels::INFO:
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).info().noquote()<<msg;
        guimode = GUIModes::INFO;
        break;
    default:
        guimode = GUIModes::INFO;
        break;
    }
    if(_GUILogger) {
        _GUILogger(guimode, msg, "", "", _ui, flag);
    }
#ifdef Q_OS_LINUX
    if((level==Errlevels::ERROR_) && _isBreakOnError) std::raise(SIGTRAP);
#endif
#endif
}


void Log::dbg_message(Dbglevels::Levels level, const QString& msg, int flag)
{
    GUIModes::Modes guimode;
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
        auto a = __FUNCTION__;
#elif defined(Q_OS_LINUX)
    auto a = static_cast<const char*>(__PRETTY_FUNCTION__);
#endif
    switch(level){
    case Dbglevels::DEBUG:
    case Dbglevels::TRACE:
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, a).debug().noquote()<<msg;
        guimode = GUIModes::DEBUG;
        break;
    default:
        guimode = GUIModes::DEBUG;
        break;
    }

    if(_GUILogger){
        _GUILogger(guimode, msg, "", "", _ui, flag);
    }
#ifdef Q_OS_LINUX
    if((level ==Dbglevels::DEBUG) && _isBreakOnError) std::raise(SIGTRAP);
#endif
#endif
}

}


