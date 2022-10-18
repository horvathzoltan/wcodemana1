#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "imainview.h"

#include <QTimer>
#include <QObject>
#include <QSet>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);   
    _statusLabelTimer.setParent(this);
    _statusLabelTimer.setSingleShot(true);

    QObject::connect(&_statusLabelTimer, &QTimer::timeout, this, &MainWindow::ClearStatusMsg);

    _clipboard = QGuiApplication::clipboard();
    _listItemChanged_Disabled=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_DoWorkRModel(const MainViewModel::DoWorkRModel& m)
{
    ui->label->setText(m.txt);
    ui->lineEdit_hu->clear();
    ui->lineEdit_en->clear();
    ui->lineEdit_de->clear();
    ui->listWidget->clear();

    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_DialogOkButton);

    auto keys = m.wcodes.keys();
    //ui->listWidget->addItems(keys);
    for(auto&k:keys){
        auto i = new QListWidgetItem(ui->listWidget, 0);
        i->setText(k);
        auto a = m.wcodes[k];
        if(a.isUsed){
            if(a.isInDb) i->setBackground(Qt::green);
        }
        if(a.tr_hu.startsWith("*")&&a.tr_en.startsWith("*")&&a.tr_de.startsWith("*"))
            i->setBackground(Qt::yellow);

        if(a.isWcodeOk()) i->setIcon(icon);
        //i->setIcon()
        //ui->listWidget->addItem(&i);
    }

};



auto MainWindow::get_DoWorkModel() -> MainViewModel::DoWorkModel
{
    auto t = ui->label->text();
    bool isok;
    int i = t.toInt(&isok, 10);
    if(!isok) return {-1};
    return {i};
};

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "PushButtonActionTriggered";
    emit PushButtonActionTriggered(this);
}


auto MainWindow::get_SelectedWcode() -> MainViewModel::ListItemChangedModel
{
    auto t = ui->listWidget->currentItem();
    if(!t) return{};
    return {t->text()};
};


auto MainWindow::get_EnText() -> MainViewModel::Text
{
    auto t = ui->lineEdit_en->text();
    if(t.isEmpty()) return{};
    return {t};
};

auto MainWindow::get_SearchText() -> MainViewModel::Text
{
    auto t = ui->lineEdit_search->text();
    if(t.isEmpty()) return{};
    return {t};
};

auto MainWindow::get_ContentSearcText() -> MainViewModel::Text
{
    auto t = ui->lineEdit_contentSearch->text();
    if(t.isEmpty()) return{};
    return {t};
};


auto MainWindow::get_HuText() -> MainViewModel::Text
{
    auto t = ui->lineEdit_hu->text();
    if(t.isEmpty()) return{};
    return {t};
};

void MainWindow::set_MessageEditor(const MainViewModel::WCode& m)
{
    ui->label_wcode->setText(m.wcode.wcode);
    ui->lineEdit_de->setText(m.wcode.tr_de);
    ui->lineEdit_en->setText(m.wcode.tr_en);
    ui->lineEdit_hu->setText(m.wcode.tr_hu);
    QString msg = m.wcode.isUsed?(m.wcode.fileName+" ("+QString::number(m.wcode.lineNumber)+")"):"not used";
    ui->label_using->setText(msg);    
}

void MainWindow::set_RogzitStatus(MainViewModel::RogzitStatusR m)
{
    ShowStatusMsg(QStringLiteral("Rögzítés ")+(m.isOk?"ok":"error"));
    auto items = ui->listWidget->findItems(m.wcode.wcode, Qt::MatchFlag::MatchExactly);
    if(items.isEmpty()) return;
    auto item = items.first();

    auto isOk = m.wcode.isWcodeOk();

    if(isOk) item->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton)); else item->setIcon({});
    if(m.wcode.tr_hu.startsWith("*")&&m.wcode.tr_en.startsWith("*")&&m.wcode.tr_de.startsWith("*"))
        item->setBackground(Qt::yellow);
}

void MainWindow::set_SaveStatus(bool isOk)
{
    ShowStatusMsg(QStringLiteral("Mentés ")+(isOk?"ok":"error"));
}

void MainWindow::set_SaveToCodeStatus(bool isOk)
{
    ShowStatusMsg(QStringLiteral("Kódgenerálás ")+(isOk?"ok":"error"));
}

void MainWindow::set_SetListWidget(const MainViewModel::SearchCounterR& m)
{    
    auto items = ui->listWidget->findItems(m.txt, Qt::MatchFlag::MatchExactly);
    if(items.empty()){
        AppendCodeEditor("egysem: "+m.txt);
        return;
    }
    if(items.length()>1){
        AppendCodeEditor("több mint egy: "+m.txt);
        return;
    }
    _listItemChanged_Disabled=true;
    ui->listWidget->setCurrentItem(items[0]);
    _listItemChanged_Disabled=false;
}

void MainWindow::set_SearchCounter(const MainViewModel::SearchCounterR& m)
{
    if(m.count>0){
        QString stxt = QString::number(m.ix+1)+'/'+QString::number(m.count);
        ui->label_search->setText(stxt);
    }
}

void MainWindow::set_SearchContentCounter(const MainViewModel::SearchCounterR& m)
{
    if(m.count>0){
        QString stxt = QString::number(m.ix+1)+'/'+QString::number(m.count);
        ui->label_search2->setText(stxt);
    }
}

void MainWindow::set_SearchToken(const MainViewModel::Text& m){
    ui->lineEdit_search->setText(m.txt);
}

void MainWindow::set_Search(const MainViewModel::SearchR2& m){
    if(m.similarWcodes.empty()){
        ui->plainTextEdit_2->clear();
    } else {
        ui->plainTextEdit_2->clear();

        QSet<QString> huSet;
        QSet<QString> enSet;
        QSet<QString> deSet;
        foreach (auto a,m.similarWcodes) {
            if(a.tr_hu.startsWith('*')&&a.tr_en.startsWith('*')&&a.tr_de.startsWith('*')) continue;
            huSet.insert(a.tr_hu);
            enSet.insert(a.tr_en);
            deSet.insert(a.tr_de);
        }
        QString b;
        foreach (auto a, huSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor2("hu:\n"+b);

        b="";
        foreach (auto a, enSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor2("en\n"+b);

        b="";
        foreach (auto a, deSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor2("de:\n"+b);
    }
}

void MainWindow::set_SearchContent(const MainViewModel::SearchR2& m){
    if(m.similarWcodes.empty()){
        ui->plainTextEdit_3->clear();
    } else {
        ui->plainTextEdit_3->clear();

        QSet<QString> huSet;
        QSet<QString> enSet;
        QSet<QString> deSet;
        foreach (auto a,m.similarWcodes) {
            if(a.tr_hu.startsWith('*')&&a.tr_en.startsWith('*')&&a.tr_de.startsWith('*')) continue;
            huSet.insert(a.tr_hu);
            enSet.insert(a.tr_en);
            deSet.insert(a.tr_de);
        }
        QString b;
        foreach (auto a, huSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor3("hu:\n"+b);

        b="";
        foreach (auto a, enSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor3("en\n"+b);

        b="";
        foreach (auto a, deSet) {
            if(!b.isEmpty())b+="\n";
            b+=a;
        }
        if(!b.isEmpty()) AppendCodeEditor3("de:\n"+b);
    }
}

MainViewModel::WCode MainWindow::get_WCodeEditor()
{
    MainViewModel::WCode m;

    m.wcode.wcode = ui->label_wcode->text();
    m.wcode.tr_de = ui->lineEdit_de->text();
    m.wcode.tr_en = ui->lineEdit_en->text();
    m.wcode.tr_hu = ui->lineEdit_hu->text();

    return m;
};

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    //qDebug() << "ListItemClicked";
    //emit RogzitTriggered(this);
    //emit ListItemChangedTriggered(this);
}


void MainWindow::on_pushButton_ok_clicked()
{
    qDebug() << "RogzitClicked";
    emit RogzitTriggered(this);
}


void MainWindow::on_pushButton_save_clicked()
{
    qDebug() << "SaveClicked";
    emit SaveTriggered(this);
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    qDebug() << "closeEvent";
    //emit SaveTriggered(this);
}



void MainWindow::on_pushButton_code_clicked()
{
    qDebug() << "SaveToCodeClicked";
    emit SaveToCodeTriggered(this);
}


void MainWindow::on_lineEdit_hu_textChanged(const QString &arg1)
{
    //ui->label_status->setText(QStringLiteral("hu changed"));
}

void MainWindow::ShowStatusMsg(QString msg){
    if(msg.isEmpty()) ClearStatusMsg();
    else{
        ui->label_status->setText(msg);
        _statusLabelTimer.start(1500);
    }
}

void MainWindow::ClearStatusMsg(){
    ui->label_status->clear();
}


void MainWindow::on_pushButton_clear_clicked()
{
    ui->plainTextEdit->clear();
}


/*Generate*/

void MainWindow::on_pushButton_generate_clicked()
{
    qDebug() << "GenerateClicked";
    emit GenerateTriggered(this);
}

void MainWindow::set_GenerateResult(const MainViewModel::Text& m)
{
    if(!m.txt.isEmpty())
    {
        AppendCodeEditor("");
        AppendCodeEditor(m.txt);
        _clipboard->setText(m.txt);
    }
}

void MainWindow::set_EnToDeResult(const MainViewModel::Text& m)
{
    if(!m.txt.isEmpty()) ui->lineEdit_de->setText(m.txt);
}

void MainWindow::set_HuToEnResult(const MainViewModel::Text& m)
{
    if(!m.txt.isEmpty()) ui->lineEdit_en->setText(m.txt);
}

void MainWindow::set_EnToHuResult(const MainViewModel::Text& m)
{
    if(!m.txt.isEmpty()) ui->lineEdit_hu->setText(m.txt);
}

MainViewModel::Text MainWindow::get_GenerateTr()
{
    return {ui->lineEdit_generateTr->text()};

}

void MainWindow::set_GenerateTr(const MainViewModel::Text &m)
{
    AppendCodeEditor("");
    AppendCodeEditor(m.txt);
    _clipboard->setText(m.txt);
}


void MainWindow::AppendCodeEditor(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::AppendCodeEditor2(QString msg)
{
    ui->plainTextEdit_2->appendPlainText(msg);
}

void MainWindow::AppendCodeEditor3(QString msg)
{
    ui->plainTextEdit_3->appendPlainText(msg);
}


void MainWindow::on_pushButton_copytoclipboard_clicked()
{
    auto txt = ui->plainTextEdit->toPlainText();
    if(txt.isEmpty()) return;
    _clipboard->setText(txt);
}


void MainWindow::on_pushButton_en_to_de_clicked()
{
    qDebug() << "EnToDeClicked";
    emit EnToDeTriggered(this);
}


void MainWindow::on_pushButton_hu_to_en_clicked()
{
    qDebug() << "HuToEnClicked";
    emit HuToEnTriggered(this);
}


void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{    
    qDebug() << "ListItemCurrentItemChanged";
    if(_listItemChanged_Disabled) return;
    //emit RogzitTriggered(this);
    emit ListItemChangedTriggered(this);
}


void MainWindow::on_pushButton_GenerateTr_clicked()
{
    emit GenerateTrTriggered(this);
}


void MainWindow::on_pushButton_hu_to_de_clicked()
{
    qDebug() << "HuToDeClicked";
    emit HuToDeTriggered(this);
}

void MainWindow::on_pushButton_translate_clicked()
{
    qDebug() << "TranslateClicked";
    emit TranslateTriggered(this);
}

void MainWindow::set_TranslateResult(const MainViewModel::Text& m)
{
    if(!m.txt.isEmpty())
    {
        AppendCodeEditor("");
        AppendCodeEditor(m.txt);
        _clipboard->setText(m.txt);
    }
}

void MainWindow::on_pushButton_en_to_hu_clicked()
{
    qDebug() << "EnToHuClicked";
    emit EnToHuTriggered(this);
}


void MainWindow::on_pushButton_searchNext_clicked()
{
    qDebug() << "searchNextClicked";
    emit SearchNextTriggered(this);
}


void MainWindow::on_pushButton_searchPrev_clicked()
{
    qDebug() << "searchPrevClicked";
    emit SearchPrevTriggered(this);
}


void MainWindow::on_pushButton_cleartr_clicked()
{
    ui->lineEdit_hu->setText("*");
    ui->lineEdit_en->setText("**");
    ui->lineEdit_de->setText("***");
}


void MainWindow::on_pushButton_huToLower_clicked()
{
    qDebug() << "huToLowerClicked";
    emit HuToLowerTriggered(this);
}


void MainWindow::on_pushButton_enToLower_clicked()
{
    qDebug() << "enToLowerClicked";
    emit EnToLowerTriggered(this);
}


void MainWindow::on_pushButton_contentSearchNext_clicked()
{
    qDebug() << "contentSearchNextClicked";
    emit ContentSearchNextTriggered(this);
}


void MainWindow::on_pushButton_contentSearchPrev_clicked()
{
    qDebug() << "contentSearchPrevClicked";
    emit ContentSearchNextTriggered(this);
}

