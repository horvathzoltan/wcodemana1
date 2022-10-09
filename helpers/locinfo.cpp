#include "locinfo.h"

namespace helper
{
LocInfo::LocInfo(const char *func, const char *file, int line)
{
    this->func=QString(func);
    this->file =QString(file);
    this->line=line;
}

QString LocInfo::toString() const
{
    return file+':'+QString::number(line)+' '+func;
}
}
