#ifndef DOWORK_H
#define DOWORK_H

#include "helpers/httphelper.h"
#include "mainviewmodel.h"

#include <QCoreApplication>
#include <QString>

class DoWork: public QObject
{
    Q_OBJECT

public:
    explicit DoWork(QObject *parent = nullptr);

    static const QString COMORSTR;
    static const QString COMORSTR2; 

    struct Params{
    public:
        QString inFile;
        QString outFile;
        QString usedWcodes;
        QString sqlWcodes;
        bool isBackup;
        bool isTest = false;

        bool IsValid();

        static Params Parse(const QCoreApplication& app);
    };

    struct Result{
        enum State:int {NotCalculated = -3, NoResult=-1, Ok=1};
        State state;
        int value;

        QString ToString();
    };        

    bool init(Params p);
    MainViewModel::DoWorkRModel Work1(MainViewModel::DoWorkModel m);
    QString GetWorkingFolder();

    MainViewModel::ListItemChangedModelR GetSelected(MainViewModel::ListItemChangedModel m);
    QList<Wcode> GetSimilar(const QString& m);

    struct SearchM{
        QString wcode;
        int ix;
        int count;
    };

    SearchM Search(const MainViewModel::Search&);
    QString GetTail(const QString& m);

    bool SetSelected(const MainViewModel::ListItemChangedModelR& m);
    bool SaveCSV();
    bool SaveToCode();

    QString DeepLTranslate(const QString& source_lang, const QString& dest_lang, const QString& msg);

    QString ReplaceTr(const QString &msg);
    QString ToLower(const QString &msg);

    Wcode Get(const QString &w);
private:
    bool _isInited = false;
    bool _isEventLoopNeeded = false;
    Params params;
    QMap<QString, Wcode> _wcodes;
    helper::HttpHelper _h;
    //Result doWork2();

signals:
    void ResponseOkAction2(QString);

private slots:
    void ResponseOkAction(QByteArray s);

};

#endif // DOWORK_H
