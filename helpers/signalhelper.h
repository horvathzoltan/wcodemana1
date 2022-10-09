#ifndef SIGNALHELPER_H
#define SIGNALHELPER_H


#include <csignal>

namespace helper
{
class SignalHelper
{
public:
    static void setShutDownSignal(int);
    static constexpr int SIGINT_ = SIGINT;
    static constexpr int SIGTERM_  =SIGTERM;
private:
    static void handleShutDownSignal(int);
};
}


#endif // SIGNALHELPER_H
