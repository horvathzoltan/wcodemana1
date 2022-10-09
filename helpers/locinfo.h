#ifndef LOCINFO_H
#define LOCINFO_H

#include <QString>
namespace helper
{
class LocInfo
{
public:
    QString func;
    QString file;
    int line;

    LocInfo(const char *func, const char *file, int line);
    QString toString() const;
};
}
#endif // LOCINFO_H
