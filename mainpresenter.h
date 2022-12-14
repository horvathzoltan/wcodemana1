#ifndef MAINPRESENTER_H
#define MAINPRESENTER_H

//#include "actions.h"
//#include "updates.h"

#include <QList>
#include <QObject>
#include "imainview.h"
#include "dowork.h"
#include <QUuid>

class IMainView;

class MainPresenter : public QObject
{
    Q_OBJECT

public:
    explicit MainPresenter(QObject *parent = nullptr);
    void appendView(IMainView *w);
    void initView(IMainView *w) const;
    bool init(DoWork::Params params);
    void AutoStartAction(IMainView *sender);

private:
    QList<IMainView*> _views;
    void refreshView(IMainView *w) const;
    QString GetSearchToken(QString wcode);

    struct SearchR{
        SearchR(){
            rt = {};
            r={};
            r2={};
            isChanged=false;
           isValid = false;
        }

        SearchR(MainViewModel::Text _rt,
                MainViewModel::SearchCounterR _r,
                MainViewModel::SearchR2 _r2,
                bool _isChanged,
                bool _isValid
                ){
            rt = _rt;
            r=_r;
            r2=_r2;
            isChanged=_isChanged;
            isValid = _isValid;
        }
        MainViewModel::Text rt;
        MainViewModel::SearchCounterR r;
        MainViewModel::SearchR2 r2;
        bool isChanged;
        bool isValid;
    };

    SearchR Search(const QString &txt);

    enum ActionType{
        Hu,  En, De
    };

    struct ApiActionParms{
        ActionType actionType;
        IMainView *sender;
        };

    QUuid AddNewApiParams(ActionType t, IMainView *sender);
    //void RemoveApiActionParms(ApiActionParms p);

    QMap<QUuid, ApiActionParms> _apiActionParams;

private slots:
    void processPushButtonAction(IMainView *sender);
    void processListItemChangedAction(IMainView *sender);
    void RogzitAction(IMainView *sender);
    void SaveAction(IMainView *sender);
    void SaveToCodeAction(IMainView *sender);
    void GenerateAction(IMainView *sender);
    void TranslateAction(IMainView *sender);
    void EnToDeAction(IMainView *sender);
    void HuToDeAction(IMainView *sender);
    void EnToHuAction(IMainView *sender);
    void onResponseOkAction2(const QString&, QUuid id);
    void HuToEnAction(IMainView *sender);
    void GenerateTrAction(IMainView *sender);
    void SearchAction(IMainView *sender);
    void SearchNextAction(IMainView *sender);
    void SearchPrevAction(IMainView *sender);

    void HuToLowerAction(IMainView *sender);
    void EnToLowerAction(IMainView *sender);

    void ContentSearchNextAction(IMainView *sender);
    void ContentSearchPrevAction(IMainView *sender);

private:
    DoWork _w;
    bool _isInited = false;
    //IMainView *_enToDeActionSender = nullptr;
    //QString _dest_lang;




};
#endif // MAINPRESENTER_H
