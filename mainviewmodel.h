#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QtGlobal>
#include <QList>
#include <QColor>
#include "models/wcode.h"
#include <QMap>

class MainViewModel
{
public:

    struct DoWorkModel{
        int i;
    };

    struct DoWorkRModel{
        QString txt;
        QMap<QString, Wcode> wcodes;
    };

    struct ListItemChangedModel{
        QString selectedItemKey;
    };

    struct WCode{
        Wcode wcode;
    };

//    struct GenerateR{
//        QString code;
//    };

    struct Text{
        QString txt;
    };

    struct RogzitStatusR{
        Wcode wcode;
        bool isOk;
    };

//    struct GenerateTrR{
//        QString msg;
//    };

    enum SearchDirection {
      Prev, Next
    };

    struct Search{
        QString txt;
        bool isNext;
    };

    struct SearchCounterR{
        QString txt;
        int ix;
        int count;        
    };

    struct SearchR2{
        QList<Wcode> similarWcodes;
    };

//    struct SearchTokenR{
//        QString searchToken;
//    };
};


#endif // MAINVIEWMODEL_H
