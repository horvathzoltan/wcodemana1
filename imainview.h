#ifndef IMAINVIEW_H
#define IMAINVIEW_H

#include "mainviewmodel.h"

#include <QColor>
#include <QPointF>
#include <QRect>
#include <QSet>

class IMainView
{
public:
    virtual void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m) = 0;
    virtual MainViewModel::DoWorkModel get_DoWorkModel() =0;

    virtual MainViewModel::ListItemChangedModel get_SelectedWcode() = 0;
    virtual MainViewModel::Text get_EnText() = 0;
    virtual MainViewModel::Text get_HuText() = 0;
    virtual MainViewModel::Text get_SearchText() = 0;

    virtual void set_MessageEditor(const MainViewModel::WCode &m) = 0;
    virtual MainViewModel::WCode get_WCodeEditor() = 0;
    virtual void set_RogzitStatus(MainViewModel::RogzitStatusR m) = 0;
    virtual void set_SaveStatus(bool isOk) = 0;
    virtual void set_SaveToCodeStatus(bool isOk) = 0;       
    virtual void set_GenerateResult(const MainViewModel::Text& m) = 0;
    virtual void set_TranslateResult(const MainViewModel::Text& m) = 0;
    virtual void set_EnToDeResult(const MainViewModel::Text &m) = 0;
    virtual void set_HuToEnResult(const MainViewModel::Text &m) = 0;
    virtual void set_EnToHuResult(const MainViewModel::Text &m) = 0;
/*search*/
    virtual void set_Search(const MainViewModel::SearchR2 &m) = 0;
    virtual void set_SearchCounter(const MainViewModel::SearchCounterR &m) = 0;
    virtual void set_SearchToken(const MainViewModel::Text& m) = 0;
    virtual void set_SearchNext(const MainViewModel::SearchCounterR& m) = 0;

    virtual void GenerateTriggered(IMainView *sender) = 0;
    virtual void TranslateTriggered(IMainView *sender) = 0;
    virtual void EnToDeTriggered(IMainView *sender) = 0;
    virtual void HuToEnTriggered(IMainView *sender) = 0;
    virtual void HuToDeTriggered(IMainView *sender) = 0;
    virtual void EnToHuTriggered(IMainView *sender) = 0;
    virtual void SearchNextTriggered(IMainView *sender) = 0;
    virtual MainViewModel::Text get_GenerateTr() =0;
    virtual void set_GenerateTr(const MainViewModel::Text &m)=0;

public: // signals
    virtual void PushButtonActionTriggered(IMainView *sender) = 0;
    virtual void ListItemChangedTriggered(IMainView *sender) = 0;
    virtual void RogzitTriggered(IMainView *sender) = 0;
    virtual void SaveTriggered(IMainView *sender) = 0;
    virtual void SaveToCodeTriggered(IMainView *sender) =0;
    virtual void GenerateTrTriggered(IMainView *sender) =0;
};

#endif // IMAINVIEW_H
