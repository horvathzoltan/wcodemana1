#include "wcode.h"
#include "helpers/log.h"

/*
WC_UI_Web_Role.Label.Description	de-DE	Description
WC_UI_Web_Role.Label.Description	en-US	Description
WC_UI_Web_Hardware.Label.Descriptor	en-US	Descriptor
WC_UI_Web_Hardware.Label.Descriptor	hu-HU	Descriptor
*/
QMap<QString, Wcode> Wcode::FromCSV(const QStringList &wcodes)
{
    QMap<QString, Wcode> m;
    for(auto &a:wcodes){
        if(!a.startsWith("WC")) continue;
        int ix1 = a.indexOf('\t');
        if(ix1==-1) continue;
        int ix2 = a.indexOf('\t', ix1+1);
        if(ix2==-1) continue;
        QString wc = a.left(ix1);
        QString lang= a.mid(ix1+1,ix2-ix1-1);
        QString msg= a.mid(ix2+1);

        zInfo("wc: "+wc);

        if(m.contains(wc)){
            m[wc].UpdateMessage(lang,msg);
        } else{
            Wcode wcode;
            wcode.wcode=wc;
            wcode.UpdateMessage(lang,msg);
            wcode.isInDb=true;
            m.insert(wc, wcode);
        }
    }
    return m;
}

QStringList Wcode::ToCSV(const QMap<QString, Wcode> &wcodes)
{
    QStringList m;
    for(auto &a:wcodes) m.append(a.ToCSV());
    return m;
}

QStringList Wcode::ToCode(const QMap<QString, Wcode> &wcodes)
{
    QStringList m;
    for(auto &a:wcodes) m.append(a.ToCode());
    return m;
}

QStringList Wcode::ToCSV() const
{
    QStringList m;
    m.append(wcode+"\thu-HU\t"+tr_hu);
    m.append(wcode+"\ten-US\t"+tr_en);
    m.append(wcode+"\tde-DE\t"+tr_de);
    return m;
}

void Wcode::UpdateMessage(const QString& lang, const QString& msg){
    if(lang=="hu-HU") tr_hu = msg;
    if(lang=="en-US") tr_en = msg;
    if(lang=="de-DE") tr_de = msg;
}

/*
    [Translation("", Lang.HU)]
    [Translation("Unable to save unloaded insole pressure values to file", Lang.EN)]
    [Translation("", Lang.DE)]
    public static class UnableToSave { }
*/

QString Wcode::PeelString(const QString& s){

    if(!s.endsWith('*')) return s;
    QString s1 = s.left(s.length()-1);
    if(!s1.endsWith('*')) return s1;
    return s1.left(s1.length()-1);
    if(!s1.endsWith('*')) return s1;
    return s1.left(s1.length()-1);
}

QStringList Wcode::ToCode() const
{
    QStringList m;
    QString trhu = PeelString(tr_hu);
    QString tren = PeelString(tr_en);
    QString trde = PeelString(tr_de);

    m.append("[Translation(\""+trhu+"\", Lang.HU)]");
    m.append("[Translation(\""+tren+"\", Lang.EN)]");
    m.append("[Translation(\""+trde+"\", Lang.DE)]");
    m.append("public static class "+wcode+" { }");
    return m;
}

QStringList Wcode::ToTranslate() const
{
    QStringList m;
    QString trhu = PeelString(tr_hu);
    QString tren = PeelString(tr_en);
    QString trde = PeelString(tr_de);

    m.append("[Translation(\""+trhu+"\", Lang.HU)]");
    m.append("[Translation(\""+tren+"\", Lang.EN)]");
    m.append("[Translation(\""+trde+"\", Lang.DE)]");
    return m;
}

/*
##############################/home/zoli/forditas/tök/MasterGait/MasterMove_ford/MasterMove.Program/Helper/ExaminationHandler.cs##############################
234:"WC_ExaminationHandler"
##############################/home/zoli/forditas/tök/MasterGait/MasterMove_ford/MasterMove.Program/Helper/InsoleHelper.cs##############################
153:"WC_UI_UWP_Pages.Preparation.UnavailableLogger"
154:"WC_UI_UWP_Pages.Preparation.UnknownInsole"
*/
void Wcode::AssertByUsing(QMap<QString, Wcode>* wcodes, const QStringList& used)
{
    QString lastFilePath;
    for(auto& u:used){
        if(u.isEmpty()) continue;
        if(u.startsWith("##")){
            int ix1 = u.indexOf('/');
            if(ix1>-1){
                int ix2 = u.indexOf('#', ix1);
                if(ix2>-1 && ix2>ix1){
                    lastFilePath = u.mid(ix1, ix2-ix1);
                }
            }
        } else if(!u[0].isDigit()) continue;
        int ix1 = u.indexOf("WC_");
        if(ix1>-1)
        {
            QString ln = u.left(ix1);
            int ix2 = u.indexOf('\"', ix1+1);
            if(ix2>-1){
                int ix3 = u.indexOf('\"', ix2+1);
                if(ix3>-1&&ix3>ix2){
                    bool ok;
                    int lineNumber = ln.toInt(&ok);
                    QString wc = u.mid(ix2+1, ix3-ix2-1);
                    if(ok){
                        // ha tartalmazza, azaz használva van, megmondjuk, hogy hol
                        if(wcodes->contains(wc)){
                            Wcode *w = &(*wcodes)[wc];
                            w->isUsed=true;
                            w->fileName = lastFilePath;
                            w->lineNumber = lineNumber;
                        }
                        //amúgy hozzá kell adni, mint nem tartalmazott, de használt wcodeot
                        else{
                            Wcode w;
                            w.wcode=wc;
                            w.isUsed = true;
                            w.isInDb = false;
                            w.fileName = lastFilePath;
                            w.lineNumber = lineNumber;
                            wcodes->insert(wc, w);
                        }
                    }
                }
            }
        }
    }
}

/*
WC_Validation
WC_Validation.Error
WC_Validation.Error.Common
WC_Validation.Error.Common.ArgumentNull
WC_Validation.Error.Common.ArgumentNullOrEmpty
WC_Validation.Error.Common.DecimalIsNotBetweenIntervals
WC_Validation.Error.Common.DecimalIsNegative
*/
void Wcode::AssertByUsing2(QMap<QString, Wcode>* wcodes, const QStringList& used)
{
    QString lastFilePath;
    for(auto& u:used){
        if(u.isEmpty()) continue;
        if(u.startsWith('#')) continue;

        int ix = u.indexOf("WC_");

        if(ix>0){
            zInfo("hutty");
        }
        QString wc = (ix<=0)?u:u.mid(ix);

        if(wcodes->contains(wc)){
            Wcode *w = &(*wcodes)[wc];
            w->isUsed=true;
            w->fileName = lastFilePath;
            w->lineNumber = -1;//lineNumber;
        }
    }
}

bool Wcode::isWcodeOk(){
    if(tr_hu.isEmpty()) return false;
    if(tr_en.isEmpty()) return false;
    if(tr_de.isEmpty()) return false;

    if(tr_hu==tr_en) return false;
    if(tr_hu==tr_de) return false;
    if(tr_en==tr_de) return false;
    return true;
}
