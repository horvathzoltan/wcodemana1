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

    void set_DoWorkRModel(const MainViewModel::DoWorkRModel& m);
    MainViewModel::DoWorkModel get_DoWorkModel();
    void ShowStatusMsg(QString msg);
    void ClearStatusMsg();
    void AppendCodeEditor(QString msg);
    void AppendCodeEditor2(QString msg);
    QClipboard *_clipboard;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainViewModel::ListItemChangedModel get_SelectedWcode();
    MainViewModel::TextModel get_EnText();
    MainViewModel::TextModel get_HuText();
    MainViewModel::TextModel get_SearchText();

    void set_MessageEditor(const MainViewModel::ListItemChangedModelR &m);
    MainViewModel::ListItemChangedModelR get_MessageEditor();
    void set_RogzitStatus(MainViewModel::RogzitStatusR m);
    void set_SaveStatus(bool isOk);
    void set_SaveToCodeStatus(bool isOk);
    void closeEvent(QCloseEvent *event);
    void set_GenerateResult(const MainViewModel::GenerateR& m);
    void set_TranslateResult(const MainViewModel::GenerateR& m);
    void set_EnToDeResult(const MainViewModel::GenerateR &m);
    void set_HuToEnResult(const MainViewModel::GenerateR &m);
    void set_EnToHuResult(const MainViewModel::GenerateR &m);
    static bool isWcodeOk(Wcode *m);

    MainViewModel::TextModel get_GenerateTr();
    void set_GenerateTr(const MainViewModel::GenerateTrR &m);
    void set_SearchNext(const MainViewModel::SearchR &m);
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
    void SearchNextTriggered(IMainView *sender);
    void SearchPrevTriggered(IMainView *sender);
    void GenerateTrTriggered(IMainView *sender);

    void HuToLowerTriggered(IMainView *sender);
    void EnToLowerTriggered(IMainView *sender);

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
};
#endif // MAINWINDOW_H
