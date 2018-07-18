#ifndef MYMDI_H
#define MYMDI_H

#include "QMdiArea"
#include "QPlainTextEdit"
#include "QTextDocument"
#include "QTextCursor"
#include "QDockWidget"
#include "QObject"

QT_BEGIN_NAMESPACE
class QPrinter;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QTextEdit;
QT_END_NAMESPACE

class LineNumberArea;

class MyMdi: public QPlainTextEdit
{
    Q_OBJECT
public slots:
    void fileModified();
    bool NewFile();
    bool OpenFile(QString fileName = 0);
    bool Save();
    bool SaveAs();
    bool SaveFile(QString fileName);
    bool LoadFile(QString filename);
    void closeEvent(QCloseEvent *event);
    void Print();
    bool CopySaveAs();
    bool RenameFile();
    void setTextColor();
    void unSetTextColor();
    void updateLineNumberWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
    void highlightCurrentLine();
    void resizeEvent(QResizeEvent *e);

public:
    explicit MyMdi(QWidget *parent = 0);
    QString GetCurrFileName();
    void SetCurrFileName(QString fileName);
    bool GetSaveStatus();
    bool GetIsUntitled();
    int GetTotalCount();
    void lineNumberPaintEvent(QPaintEvent *event);
    int lineNumberWidth();
    void MarkLines(QList<QPoint> lineHolder);
    void highlightSearchString(QString searchString);
    void clearMark();
    QList<int> searchCurrentFile(QString searchString);

    QString CurrFileName;
    QString CurrFilePath;
    bool IsUntitled;
    bool IsFileSaved;
    QTextDocument *textDocument;
    QString searchString = 0;
    QString prevSearchString = 0;
    bool isFirstSearch = true;
    QTextCursor textcursor;
    int totalCount = 0;
    QWidget *lineNumberArea;

    QList<int> Find(QString searchString,QTextDocument::FindFlag options);

    void FindNext(QString searchString,QTextDocument::FindFlag options);
};

class LineNumberArea:public QWidget
{
public:
    LineNumberArea(MyMdi *mymdi):QWidget(mymdi)
    {
        myMdi = mymdi;
    }
    QSize sizeHint() const Q_DECL_OVERRIDE
    {
        return QSize(myMdi->lineNumberWidth(),0);
    }
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE
    {
        myMdi->lineNumberPaintEvent(event);
    }
private:
    MyMdi *myMdi;
};

#endif // MYMDI_H
