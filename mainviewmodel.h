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

    struct ListItemChangedModelR{
        Wcode wcode;
    };

    struct GenerateR{
        QString code;
    };

    struct TextModel{
        QString text;
    };

    struct RogzitStatusR{
        Wcode wcode;
        bool isOk;
    };

    struct GenerateTrR{
        QString msg;
    };

    struct Search{
        QString text;
        bool isNext;
    };

    struct SearchR{        
        QString msg;
        int ix;
        int count;        
    };

    struct SearchR2{
        QList<Wcode> similarWcodes;
    };

    struct SearchTokenR{
        QString searchToken;
    };
};


#endif // MAINVIEWMODEL_H
