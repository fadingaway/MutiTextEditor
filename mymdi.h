#ifndef MYMDI_H
#define MYMDI_H

#include "QMdiArea"
#include "QPlainTextEdit"
#include "QTextDocument"
#include "QTextCursor"
#include "QDockWidget"
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
private slots:
    void fileModified();
    bool NewFile();
    bool OpenFile();
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
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
public:
    MyMdi(QWidget *parent = 0);
    QString GetCurrFileName();
    void SetCurrFileName(QString fileName);
    bool GetSaveStatus();
    bool GetIsUntitled();
    void Find(QString searchString,FindFlags options = FindFlags());
    int GetTotalCount();
    void FindNext(QString searchString,FindFlags options = FindFlags());
    void lineNumberPaintEvent(QPaintEvent *event);
    int lineNumberWidth();
    void MarkLines(QList<QPoint> lineHolder);
    void highlightSearchString(QString searchString);
    void clearMark();
    void createSearchDockWidget(QString searchString);
private:
    QString CurrFileName;
    QString CurrFilePath;
    bool IsUntitled;
    bool IsFileSaved;
    QTextDocument *document;
    QString searchString = 0;
    QString prevSearchString = 0;
    bool isFirstSearch = true;
    QTextCursor textCursor;
    int totalCount = 0;
    QWidget *LineNumberArea;
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
