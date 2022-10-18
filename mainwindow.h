#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QClipboard>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QTimer>
#include "imainview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public IMainView
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QTimer _statusLabelTimer;
    bool _listItemChanged_Disabled;
    QClipboard *_clipboard;

    void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m);
    MainViewModel::DoWorkModel get_DoWorkModel();
    void ShowStatusMsg(QString msg);
    void ClearStatusMsg();
    void AppendCodeEditor(QString msg);
    void AppendCodeEditor2(QString msg);
    void AppendCodeEditor3(QString msg);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainViewModel::ListItemChangedModel get_SelectedWcode();
    MainViewModel::Text get_EnText();
    MainViewModel::Text get_HuText();
    MainViewModel::Text get_SearchText();
    MainViewModel::Text get_ContentSearcText();

    void set_MessageEditor(const MainViewModel::WCode &m);
    MainViewModel::WCode get_WCodeEditor();
    void set_RogzitStatus(MainViewModel::RogzitStatusR m);
    void set_SaveStatus(bool isOk);
    void set_SaveToCodeStatus(bool isOk);
    void closeEvent(QCloseEvent *event);
    void set_GenerateResult(const MainViewModel::Text& m);
    void set_TranslateResult(const MainViewModel::Text& m);
    void set_EnToDeResult(const MainViewModel::Text &m);
    void set_HuToEnResult(const MainViewModel::Text &m);
    void set_EnToHuResult(const MainViewModel::Text &m);
    static bool isWcodeOk(Wcode *m);

    MainViewModel::Text get_GenerateTr();
    void set_GenerateTr(const MainViewModel::Text &m);
    void set_Search(const MainViewModel::SearchR2 &m);
    void set_SearchCounter(const MainViewModel::SearchCounterR &m);
    void set_SearchContentCounter(const MainViewModel::SearchCounterR &m);

    void set_SearchToken(const MainViewModel::Text& m);
    void set_SetListWidget(const MainViewModel::SearchCounterR& m);
    void set_SearchContent(const MainViewModel::SearchR2& m);
signals:
    void PushButtonActionTriggered(IMainView *sender);
    void ListItemChangedTriggered(IMainView *sender);
    void RogzitTriggered(IMainView *sender);
    void SaveTriggered(IMainView *sender);
    void SaveToCodeTriggered(IMainView *sender);
    void GenerateTriggered(IMainView *sender);
    void TranslateTriggered(IMainView *sender);
    void EnToDeTriggered(IMainView *sender);
    void HuToEnTriggered(IMainView *sender);
    void HuToDeTriggered(IMainView *sender);
    void EnToHuTriggered(IMainView *sender);
    void GenerateTrTriggered(IMainView *sender);

    void HuToLowerTriggered(IMainView *sender);
    void EnToLowerTriggered(IMainView *sender);

    void SearchNextTriggered(IMainView *sender);
    void SearchPrevTriggered(IMainView *sender);

    void ContentSearchPrevTriggered(IMainView *sender);
    void ContentSearchNextTriggered(IMainView *sender);

private slots:    
    void on_pushButton_clicked();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_pushButton_ok_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_code_clicked();
    void on_lineEdit_hu_textChanged(const QString &arg1);
    void on_pushButton_clear_clicked();
    void on_pushButton_generate_clicked();
    void on_pushButton_copytoclipboard_clicked();
    void on_pushButton_en_to_de_clicked();
    void on_pushButton_hu_to_en_clicked();
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pushButton_GenerateTr_clicked();
    void on_pushButton_hu_to_de_clicked();
    void on_pushButton_translate_clicked();
    void on_pushButton_en_to_hu_clicked();
    void on_pushButton_searchNext_clicked();
    void on_pushButton_searchPrev_clicked();
    void on_pushButton_cleartr_clicked();
    void on_pushButton_huToLower_clicked();
    void on_pushButton_enToLower_clicked();
    void on_pushButton_contentSearchNext_clicked();
    void on_pushButton_contentSearchPrev_clicked();
};
#endif // MAINWINDOW_H
