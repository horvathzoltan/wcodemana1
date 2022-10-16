#include "dowork.h"
#include "helpers/httphelper.h"

#include "helpers/filenamehelper.h"
#include "helpers/log.h"
#include "helpers/textfilehelper.h"
#include "helpers/commandlineparserhelper.h"
#include <QCommandLineParser>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "models/wcode.h"

DoWork::DoWork(QObject *parent) :QObject(parent)
{

}

// git push 1

// command line arguments
// -i C:\Users\Zoli4\wcodemana\testdata\wcodes.txt -o C:\Users\Zoli4\wcodemana\testdata\wcodes_out.txt -u C:\Users\Zoli4\wcodemana\testdata\used_wcodes.csv

auto DoWork::Params::Parse(const QCoreApplication& app) -> DoWork::Params
{
    QCommandLineParser parser;

    parser.setApplicationDescription(QStringLiteral("command line test1 app."));
    parser.addHelpOption();
    parser.addVersionOption();

    const QString OPTION_IN = QStringLiteral("input"); //translations: ami az osztálydefinícióból jön
    const QString OPTION_OUT = QStringLiteral("output");
    const QString OPTION_BACKUP = QStringLiteral("backup");
    const QString OPTION_TEST = QStringLiteral("test");
    const QString OPTION_UWC = QStringLiteral("usedWcodes"); //usedwcodes: amit a translationsból használunk, ez jelenik meg zölddel
    const QString OPTION_SQLWC = QStringLiteral("sqlWcodes"); //usedwcodes: amit a translationsból használunk, ez jelenik meg zölddel

    helper::CommandLineParserHelper::addOption(&parser, OPTION_IN, QStringLiteral("geometry file as input"));
    helper::CommandLineParserHelper::addOption(&parser, OPTION_OUT, QStringLiteral("g-code file as output"));
    helper::CommandLineParserHelper::addOptionBool(&parser, OPTION_BACKUP, QStringLiteral("set if backup is needed"));
    helper::CommandLineParserHelper::addOptionBool(&parser, OPTION_TEST, QStringLiteral("set to activate test mode"));
    helper::CommandLineParserHelper::addOption(&parser, OPTION_UWC, QStringLiteral("usedWordCodes"));
    helper::CommandLineParserHelper::addOption(&parser, OPTION_SQLWC, QStringLiteral("SQLWordCodes"));

    parser.process(app);

    return {
        parser.value(OPTION_IN),
        parser.value(OPTION_OUT),
        parser.value(OPTION_UWC),
        parser.value(OPTION_SQLWC),
        parser.isSet(OPTION_BACKUP),
        parser.isSet(OPTION_TEST)
    };
}

auto DoWork::Params::IsValid() -> bool
{
    QStringList err;
//    if(inFile.isEmpty())
//    {
//        err.append(QStringLiteral("inFile is empty"));
//    }
//    if(inFile.isEmpty())
//    {
//        err.append(QStringLiteral("outFile is empty"));
//    }
    if(!err.isEmpty()) zInfo(err)
            return err.isEmpty();
}


auto DoWork::init(DoWork::Params p) -> bool
{
    _isInited = false;
    if(!p.IsValid()) return false;

    params = p;    

    //_result = { Result::State::NotCalculated, -1};
    QObject::connect(&_h, SIGNAL(ResponseOk(QByteArray, QUuid)),
                     this, SLOT(ResponseOkAction(QByteArray, QUuid)));

    _isInited = true;
    return true;
}


auto DoWork::GetWorkingFolder() -> QString
{
    QString workingFolder = params.isTest?helper::FileNameHelper::GetTestFolderPath():qApp->applicationDirPath();
    return workingFolder;
}

auto DoWork::Work1(MainViewModel::DoWorkModel m) -> MainViewModel::DoWorkRModel
{
    if(!_isInited) return {QStringLiteral("not inited"),{}};

    _wcodes.clear();

    QString workingFolder = GetWorkingFolder();

    if(params.inFile.isEmpty()){
        return {QStringLiteral("no input file"),{}};
    }

    if(params.outFile.isEmpty()){
        QFileInfo fi(params.inFile);
        QString bn = fi.fileName(); //baseName();

        params.outFile=bn;// + ".txt";
    }
    zInfo("workingFolder: "+workingFolder);
    zInfo(QStringLiteral("params: %1, %2, %3, %4 %5 %6")
          .arg(params.inFile) //translations hu-en-de stb...
          .arg(params.outFile)
          .arg(params.isBackup)
          .arg(params.isTest)
          .arg(params.usedWcodes)
          .arg(params.sqlWcodes)); // amit használunk - ez lesz a zöld - több ilyen fájl is kellene
    auto d = QDir(workingFolder);
    auto file = d.filePath(params.inFile);
    auto wcodes_csv = helper::TextFileHelper::loadLines(file);
    if(wcodes_csv.isEmpty()) return {QStringLiteral("file is empty"),{}};

/*
select WordCode, LanguageCode, "Text" from lang.Translations t1 where exists
(
    select  WordCode, LanguageCode, "Text" from  lang.Translations t2 where
        t1."Text" = t2."Text"
        and
        t1.LanguageCode <> t2.LanguageCode
)
*/
    _wcodes = Wcode::FromCSV(wcodes_csv);

    // a zölddel jelöltek beolvasása
    QString usedWcodesFileName = d.filePath(params.usedWcodes);
    QStringList usedWcodeLines = helper::TextFileHelper::loadLines(usedWcodesFileName);
    QString sqlWcodesFileName = d.filePath(params.sqlWcodes);
    QStringList sqlWcodeLines = helper::TextFileHelper::loadLines(sqlWcodesFileName);

    QStringList usedAll = usedWcodeLines+sqlWcodeLines;
    if(!usedWcodeLines.isEmpty()){
        Wcode::AssertByUsing2(&_wcodes, usedAll);
    }
    //return {QString::number(m.i+1)};
    return
    {
        QStringLiteral("lines: ")+QString::number(_wcodes.count()),
                _wcodes
    };
}

auto DoWork::GetSelected(MainViewModel::ListItemChangedModel m) -> MainViewModel::WCode
{
    if(!_wcodes.contains(m.selectedItemKey)) return {};
    auto a = _wcodes[m.selectedItemKey];
    return {a};
}

auto DoWork::GetTail(const QString& m) -> QString
{
    QString tail = m.toLower();
    int ix1 = tail.lastIndexOf('_');
    int ix2 = tail.lastIndexOf('.');
    if(ix2>ix1) ix1=ix2;
    if(ix1!=-1) tail = tail.mid(ix1).replace('_','.');
    return tail;
}

auto DoWork::GetSimilar(const QString& m) -> QList<Wcode>
{
    QList<Wcode> wl;
    QString tail = GetTail(m);
    foreach (auto wcode, _wcodes) {
        QString tail1  = GetTail(wcode.wcode);
        if(tail == tail1) wl.append(wcode);
    }
    return wl;
}

auto DoWork::Search(const MainViewModel::Search& m) -> SearchM
{
    static QString lastM;
    static int lastIx = 0;
    static QList<QString> hits;
    bool isChanged = lastM != m.txt;
    if(m.txt.isEmpty()){
        hits.clear();
        lastM="";
        lastIx = 0;
        return {"", -1, 0, isChanged};
    }

    if(isChanged){
        auto mLower = m.txt.toLower();
        hits.clear();
        lastM=m.txt;
        foreach(auto wcode, _wcodes){
            auto tokens = wcode.wcode.toLower().replace('_','.').split('.');
            //if(!tokens.contains(m.text.toLower())) continue;
            if(tokens.last()!=mLower) continue;
            hits.append(wcode.wcode);
        }
        if(!hits.empty()){
            if(m.isNext){
                lastIx=0;
            } else {
                lastIx=hits.count()-1;
            }
        } else{
            lastIx = -1;
        }
    } else {
        if(!hits.empty()){
            if(m.isNext){
                lastIx++;
                if(lastIx>=hits.count()) lastIx=0;
            } else {
                lastIx--;
                if(lastIx<0) lastIx=hits.count()-1;
            }
        }
    }

    if(hits.empty()) return {"", -1, 0, isChanged};
    if(lastIx==-1) return {"", -1, 0, isChanged};
    int size = hits.count();
    return {hits[lastIx], lastIx, size, isChanged};
}

bool DoWork::SetSelected(const MainViewModel::WCode &m)
{
    if(!_wcodes.contains(m.wcode.wcode)) return false;
    _wcodes[m.wcode.wcode]=m.wcode;
    return true;
}

bool DoWork::SaveCSV(){

    if(!_isInited) return false;

    QString workingFolder = GetWorkingFolder();

    zInfo("workingFolder: "+workingFolder);
    zInfo(QStringLiteral("params: %1, %2, %3, %4")
              .arg(params.inFile)
              .arg(params.outFile)
              .arg(params.isBackup)
              .arg(params.isTest));

    auto d = QDir(workingFolder);
    auto file = d.filePath(params.outFile);
    QStringList wcodes_csv = Wcode::ToCSV(_wcodes);
    bool isOk = helper::TextFileHelper::save(wcodes_csv.join('\n'),file);

    return isOk;
}

bool DoWork::SaveToCode(){

    if(!_isInited) return false;

    QString workingFolder = GetWorkingFolder();

    zInfo("workingFolder: "+workingFolder);
    zInfo(QStringLiteral("params: %1, %2, %3, %4")
              .arg(params.inFile)
              .arg(params.outFile)
              .arg(params.isBackup)
              .arg(params.isTest));

    auto fn = params.outFile.replace(".txt",".cs");
    auto d = QDir(workingFolder);
    auto file = d.filePath(fn);
    QStringList wcodes_csv = Wcode::ToCode(_wcodes);
    bool isOk = helper::TextFileHelper::save(wcodes_csv.join('\n'),file);

    return isOk;
}

QString DoWork::DeepLTranslate(const QString &source_lang,
                               const QString &dest_lang,
                               const QString &msg,
                               QUuid id)
{
    _h.SendPost(id, source_lang, dest_lang, msg);
    zInfo("request sent");
    return QString("request sent");
}

void DoWork::ResponseOkAction(QByteArray s, QUuid id){
    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(s, &errorPtr);
    QJsonObject rootobj = doc.object();
    auto tr = rootobj.value("translations").toArray();
    QString translation;
    foreach(const QJsonValue & val, tr){
        translation=val.toObject().value("text").toString();
    }

    zInfo("ResponseOkAction:" + translation);

    emit ResponseOkAction2(translation, id);
}


const QString DoWork::COMORSTR = R"(GlobalTranslation.Translate\((\"([^"\\]*(?:\\.[^"\\]*)*)\"|\@\"((?:[^\"]|(?:\"\"))*)\")|([\s]*(?:\/\*([\s\S]*?)\*\/|\/\/(.*))))";
const QString DoWork::COMORSTR2 = R"(GlobalTranslation.Translate\(((?!new\s)[^\)]+)\))";


QString DoWork::ReplaceTr(const QString& msg){
    QRegularExpression reg(COMORSTR);
    QString r(msg);

    bool ok;
    do{
        QString s(r);
        QRegularExpressionMatchIterator i = reg.globalMatch(s);
        ok = false;
        while(i.hasNext()){
            ok = true;
            QRegularExpressionMatch match = i.next();
            if(match.hasMatch()){
                auto index = match.capturedStart(0);
                auto length = match.capturedLength(0);

                QString newstring = "GlobalTranslation.Translate<W."+match.captured(2)+">(";
                r = s.left(index)+newstring+s.mid(index+length);
            }
        }
    }while(ok);

    QRegularExpression reg2(COMORSTR2);
    do{
        QString s(r);
        QRegularExpressionMatchIterator i = reg2.globalMatch(s);
        ok = false;
        while(i.hasNext()){
            ok = true;
            QRegularExpressionMatch match = i.next();
            if(match.hasMatch()){
                auto index = match.capturedStart(0);
                auto length = match.capturedLength(0);
                if(match.capturedLength(1) == 0) continue;

                QString newstring = "GlobalTranslation.Translate(new WordCodeMessage("+match.captured(1)+"))";
                r = s.left(index)+newstring+s.mid(index+length);
            }
        }
    }while(ok);

    return r;
}

QString DoWork::ToLower(const QString &msg)
{
    if(msg.isEmpty()) return msg;
    QChar c0 = msg[0];
    QString a = msg.toLower();
    a[0]=c0;
    return a;
}

Wcode DoWork::Get(const QString& w){
    return _wcodes[w];
}
