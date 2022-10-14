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


auto MainWindow::get_EnText() -> MainViewModel::TextModel
{
    auto t = ui->lineEdit_en->text();
    if(t.isEmpty()) return{};
    return {t};
};

auto MainWindow::get_SearchText() -> MainViewModel::TextModel
{
    auto t = ui->lineEdit_search->text();
    if(t.isEmpty()) return{};
    return {t};
};


auto MainWindow::get_HuText() -> MainViewModel::TextModel
{
    auto t = ui->lineEdit_hu->text();
    if(t.isEmpty()) return{};
    return {t};
};

void MainWindow::set_MessageEditor(const MainViewModel::ListItemChangedModelR& m)
{
    ui->label_wcode->setText(m.wcode.wcode);
    ui->lineEdit_de->setText(m.wcode.tr_de);
    ui->lineEdit_en->setText(m.wcode.tr_en);
    ui->lineEdit_hu->setText(m.wcode.tr_hu);
    QString msg = m.wcode.isUsed?(m.wcode.fileName+" ("+QString::number(m.wcode.lineNumber)+")"):"not used";
    ui->label_using->setText(msg);
    //_clipboard->setText(m.wcode.wcode);

    if(m.similarWcodes.empty()){
        ui->plainTextEdit_2->clear();
    } else {
        ui->plainTextEdit_2->clear();
        //int wclength = 0;
//        foreach (auto a,m.similarWcodes){
//            if(a.tr_hu.startsWith('*')&&a.tr_en.startsWith('*')&&a.tr_de.startsWith('*')) continue;
//            int alength = a.wcode.length();
//            if(alength>wclength) wclength = alength;
//        }
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

//        foreach (auto a,m.similarWcodes) {
//            QString l1 = a.wcode;//+QString(wclength-a.wcode.length()+1, ' ');
//            QString line1 = "hu_HU "+a.tr_hu;
//            QString line2 = "en_US "+a.tr_en;
//            QString line3 = "de_DE "+a.tr_de;
//            AppendCodeEditor2(l1);
//            AppendCodeEditor2(line1);
//            AppendCodeEditor2(line2);
//            AppendCodeEditor2(line3);
//        }
    }
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

void MainWindow::set_SearchNext(const MainViewModel::SearchR& m)
{
    auto items = ui->listWidget->findItems(m.msg, Qt::MatchFlag::MatchExactly);    
    if(m.count>0){
        QString stxt = QString::number(m.ix+1)+'/'+QString::number(m.count);
        ui->label_search->setText(stxt);
    }
    if(items.length()==0){
        AppendCodeEditor("egysem: "+m.msg);
        return;
    }
    if(items.length()>1){
        AppendCodeEditor("több mint egy: "+m.msg);
        return;
    }
    //ui->listWidget->clearSelection();
    ui->listWidget->setCurrentItem(items[0]);
}

MainViewModel::ListItemChangedModelR MainWindow::get_MessageEditor()
{
    MainViewModel::ListItemChangedModelR m;

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

void MainWindow::set_GenerateResult(const MainViewModel::GenerateR& m)
{
    if(!m.code.isEmpty())
    {
        AppendCodeEditor("");
        AppendCodeEditor(m.code);
        _clipboard->setText(m.code);
    }
}

void MainWindow::set_EnToDeResult(const MainViewModel::GenerateR& m)
{
    if(!m.code.isEmpty()) ui->lineEdit_de->setText(m.code);
}

void MainWindow::set_HuToEnResult(const MainViewModel::GenerateR& m)
{
    if(!m.code.isEmpty()) ui->lineEdit_en->setText(m.code);
}

void MainWindow::set_EnToHuResult(const MainViewModel::GenerateR& m)
{
    if(!m.code.isEmpty()) ui->lineEdit_hu->setText(m.code);
}

MainViewModel::TextModel MainWindow::get_GenerateTr()
{
    return {ui->lineEdit_generateTr->text()};

}

void MainWindow::set_GenerateTr(const MainViewModel::GenerateTrR &m)
{
    AppendCodeEditor("");
    AppendCodeEditor(m.msg);
    _clipboard->setText(m.msg);
}


void MainWindow::AppendCodeEditor(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::AppendCodeEditor2(QString msg)
{
    ui->plainTextEdit_2->appendPlainText(msg);
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

void MainWindow::set_TranslateResult(const MainViewModel::GenerateR& m)
{
    if(!m.code.isEmpty())
    {
        AppendCodeEditor("");
        AppendCodeEditor(m.code);
        _clipboard->setText(m.code);
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

