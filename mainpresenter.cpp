#include "mainpresenter.h"
#include "mainviewmodel.h"
#include "dowork.h"

#include <QFileDialog>
#include <QDateTime>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QUuid>

MainPresenter::MainPresenter(QObject *parent) :QObject(parent)
{

}

void MainPresenter::appendView(IMainView *w)
{
    if(_views.contains(w)) return;
    _views.append(w);

    auto *view_obj = dynamic_cast<QObject*>(w);

    QObject::connect(view_obj, SIGNAL(PushButtonActionTriggered(IMainView *)),
                     this, SLOT(processPushButtonAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(ListItemChangedTriggered(IMainView *)),
                     this, SLOT(processListItemChangedAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(RogzitTriggered(IMainView *)),
                     this, SLOT(RogzitAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(SaveTriggered(IMainView *)),
                     this, SLOT(SaveAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(SaveToCodeTriggered(IMainView *)),
                     this, SLOT(SaveToCodeAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(GenerateTriggered(IMainView *)),
                     this, SLOT(GenerateAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(TranslateTriggered(IMainView *)),
                     this, SLOT(TranslateAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(EnToDeTriggered(IMainView *)),
                     this, SLOT(EnToDeAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(HuToEnTriggered(IMainView *)),
                     this, SLOT(HuToEnAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(HuToDeTriggered(IMainView *)),
                     this, SLOT(HuToDeAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(EnToHuTriggered(IMainView *)),
                     this, SLOT(EnToHuAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(GenerateTrTriggered(IMainView *)),
                     this, SLOT(GenerateTrAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(SearchNextTriggered(IMainView *)),
                     this, SLOT(SearchNextAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(SearchPrevTriggered(IMainView *)),
                     this, SLOT(SearchPrevAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(HuToLowerTriggered(IMainView *)),
                     this, SLOT(HuToLowerAction(IMainView *)));

    QObject::connect(view_obj, SIGNAL(EnToLowerTriggered(IMainView *)),
                     this, SLOT(EnToLowerAction(IMainView *)));




    //refreshView(w);
}

void MainPresenter::refreshView(IMainView *w) const { Q_UNUSED(w) };


auto MainPresenter::init(DoWork::Params params) -> bool
{
    _isInited = false;
    _w.init(params);
    connect(&_w,SIGNAL(ResponseOkAction2(QString)),
            this,SLOT(onResponseOkAction2(QString)));

    //_result = { Result::State::NotCalculated, -1};
    _isInited = true;
    return true;
}

void MainPresenter::AutoStartAction(IMainView *sender)
{
    MainPresenter::processPushButtonAction(sender);
}

void MainPresenter::initView(IMainView *w) const {
    MainViewModel::DoWorkRModel rm{"started",{}};
    w->set_DoWorkRModel(rm);
};

void MainPresenter::processPushButtonAction(IMainView *sender){
    qDebug() << "processPushButtonAction";
    auto m = sender->get_DoWorkModel();
    //DoWork w;
    auto rm = _w.Work1(m);
    sender->set_DoWorkRModel(rm);
}

void MainPresenter::processListItemChangedAction(IMainView *sender)
{
    qDebug() << "processListItemChangedAction";
    auto m = sender->get_SelectedWcode();
    auto rm = _w.GetSelected(m);
    sender->set_MessageEditor(rm);

    //auto tokens = rm.wcode.wcode.toLower().replace('_','.').split('.');
    //QString searchToken = tokens.isEmpty()?"":tokens.last();
    QString searchToken = GetSearchToken(rm.wcode.wcode);
    if(!searchToken.isEmpty()) Search(sender, searchToken);
}

QString MainPresenter::GetSearchToken(QString txt){
    auto tokens = txt.toLower().replace('_','.').split('.');
    QString searchToken = tokens.isEmpty()?"":tokens.last();
    return searchToken;
}

void MainPresenter::RogzitAction(IMainView *sender)
{
    qDebug() << "RogzitAction";
    auto m = sender->get_MessageEditor();
    MainViewModel::RogzitStatusR r;
    r.isOk = _w.SetSelected(m);
    r.wcode = m.wcode;

    sender->set_RogzitStatus(r);
}

void MainPresenter::SaveAction(IMainView *sender)
{
    qDebug() << "SaveAction";
    bool isOk = _w.SaveCSV();
    sender->set_SaveStatus(isOk);
}

void MainPresenter::SaveToCodeAction(IMainView *sender)
{
    qDebug() << "SaveToCodeAction";
    bool isOk = _w.SaveToCode();
    sender->set_SaveToCodeStatus(isOk);
}

void MainPresenter::GenerateAction(IMainView *sender)
{
    qDebug() << "GenerateAction";
    auto m = sender->get_SelectedWcode();
    auto m2 = _w.GetSelected(m);
    if(!m2.wcode.isValid()) return;
    auto code = m2.wcode.ToCode().join('\n');
    MainViewModel::GenerateR rm = {code};
    sender->set_GenerateResult(rm);
}

void MainPresenter::TranslateAction(IMainView *sender)
{
    qDebug() << "TranslateAction";
    auto m = sender->get_SelectedWcode();
    auto m2 = _w.GetSelected(m);
    if(!m2.wcode.isValid()) return;
    auto code = m2.wcode.ToTranslate().join('\n');
    MainViewModel::GenerateR rm = {code};
    sender->set_TranslateResult(rm);
}

//EnToDeAction
void MainPresenter::EnToDeAction(IMainView *sender)
{
    qDebug() << "EnToDeAction";
    auto m = sender->get_EnText();
//    auto m2 = _w.GetSelected(m);
//    if(!m2.wcode.isValid()) return;
    _enToDeActionSender = sender;
    _dest_lang = "de";
    QString msg_de = _w.DeepLTranslate("en", "de", m.text);
}

void MainPresenter::onResponseOkAction2(QString msg_de)
{
    MainViewModel::GenerateR rm = {msg_de};
    if(_enToDeActionSender){
        if(_dest_lang == "de")
            _enToDeActionSender->set_EnToDeResult(rm);
        else if(_dest_lang == "en")
            _enToDeActionSender->set_HuToEnResult(rm);
        else if(_dest_lang == "hu")
            _enToDeActionSender->set_EnToHuResult(rm);
    }
    _enToDeActionSender = nullptr;
    _dest_lang.clear();
}


void MainPresenter::HuToEnAction(IMainView *sender)
{
    qDebug() << "HuToEnAction";
    auto m = sender->get_HuText();
    //auto m2 = _w.GetSelected(m);
    //if(!m2.wcode.isValid()) return;
    _enToDeActionSender = sender;
    _dest_lang = "en";
    QString msg_en = _w.DeepLTranslate("hu", "en", m.text);
}


void MainPresenter::GenerateTrAction(IMainView *sender)
{
    qDebug() << "HuToEnAction";
    auto m = sender->get_GenerateTr();
    QString a = _w.ReplaceTr(m.text);
    sender->set_GenerateTr({a});
}

void MainPresenter::HuToDeAction(IMainView *sender)
{
    qDebug() << "HuToDeAction";
    auto m = sender->get_HuText();
    _enToDeActionSender = sender;
    _dest_lang = "de";
    QString msg_de = _w.DeepLTranslate("hu", "de", m.text);
}

void MainPresenter::EnToHuAction(IMainView *sender)
{
    qDebug() << "EnToHuAction";
    auto m = sender->get_EnText();
    //auto m2 = _w.GetSelected(m);
    //if(!m2.wcode.isValid()) return;
    _enToDeActionSender = sender;
    _dest_lang = "hu";
    QUuid id = QUuid::createUuid();
    QString msg_hu = _w.DeepLTranslate("en", "hu", m.text);
}

void MainPresenter::SearchAction(IMainView *sender){
    qDebug() << "SearchAction";
    auto textModel = sender->get_SearchText();
    Search(sender, textModel.text);
//    MainViewModel::Search m { textModel.text, true };
//    DoWork::SearchM s = _w.Search(m);
//    if(s.wcode.isEmpty()) return;
//    MainViewModel::SearchR r {s.wcode, s.ix, s.count};
//    r.similarWcodes = _w.GetSimilar(s.wcode);
//    sender->set_SearchCounter(r);
}

void MainPresenter::Search(IMainView *sender, const QString& txt)
{
    qDebug() << "SearchAction";

    MainViewModel::Search m { txt, true };
    DoWork::SearchM s = _w.Search(m);
    if(s.wcode.isEmpty()) return;
    auto similarWcodes = _w.GetSimilar(s.wcode);

    MainViewModel::SearchTokenR rt {txt};
    sender->set_SearchToken(rt);
    MainViewModel::SearchR r {s.wcode, s.ix, s.count};
    sender->set_SearchCounter(r);
    MainViewModel::SearchR2 r2 { similarWcodes};
    sender->set_Search(r2);
}

void MainPresenter::SearchNextAction(IMainView *sender)
{
    qDebug() << "SearchNextAction";
    auto textModel = sender->get_SearchText();
    MainViewModel::Search m { textModel.text, true };
    DoWork::SearchM s = _w.Search(m);
    if(s.wcode.isEmpty()) return;
    // ha megváltozik a wcode, akkor set_Search, ezt a _w.Search detektálja
    MainViewModel::SearchR r {s.wcode, s.ix, s.count};
    sender->set_SearchCounter(r);
    sender->set_SearchNext(r);
    auto wcode = _w.Get(s.wcode);
    MainViewModel::ListItemChangedModelR rm {wcode};
    sender->set_MessageEditor(rm);
}

void MainPresenter::SearchPrevAction(IMainView *sender)
{
    qDebug() << "SearchPrevAction";
    auto textModel = sender->get_SearchText();
    MainViewModel::Search m { textModel.text, false };
    DoWork::SearchM s = _w.Search(m);
    if(s.wcode.isEmpty()) return;
    MainViewModel::SearchR r {s.wcode, s.ix, s.count};
    sender->set_SearchCounter(r);
    sender->set_SearchNext(r);
    auto wcode = _w.Get(s.wcode);
    MainViewModel::ListItemChangedModelR rm {wcode};
    sender->set_MessageEditor(rm);
}


void MainPresenter::HuToLowerAction(IMainView *sender)
{
    qDebug() << "HuToLowerAction";
    auto m = sender->get_HuText();
    QString m1 = _w.ToLower(m.text);
    MainViewModel::GenerateR r = {m1};
    sender->set_EnToHuResult(r);
}

void MainPresenter::EnToLowerAction(IMainView *sender)
{
    qDebug() << "EnToLowerAction";
    auto m = sender->get_EnText();
    QString m1 = _w.ToLower(m.text);
    MainViewModel::GenerateR r = {m1};
    sender->set_HuToEnResult(r);
}
