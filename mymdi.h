#ifndef MYMDI_H
#define MYMDI_H

#include "QMdiArea"
#include "QPlainTextEdit"
#include "QTextDocument"
#include "QTextCursor"
#include "QDockWidget"
#include "QObject"
#include "QTextBlock"
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
    void setSearchString();
    void setSearchStringFromTab(QString text);
    QTextBlock getFirstVisibleBlock();

public:
    explicit MyMdi(QWidget *parent = 0);
    QString GetCurrFileName();
    void SetCurrFileName(QString fileName);
    bool GetSaveStatus();
    bool GetIsUntitled();
    int GetTotalCount(QTextDocument::FindFlags options);
    void lineNumberPaintEvent(QPaintEvent *event);
    int lineNumberWidth();
    void MarkLines(QList<QPoint> lineHolder);
    void highlightSearchString();
    void clearMark();
    QList<QString> searchCurrentFile(QString searchString);
    void resetCursorPosition();
    void removeDuplicateExtraSelection(QTextEdit::ExtraSelection extraSelection);
    QString getFilePath();
    QString CurrFileName;
    QString CurrFilePath;
    bool IsUntitled;
    bool IsFileSaved;
    bool IsDefaultWindow = false;
    QTextDocument *textDocument;
    QString searchString = 0;
    bool isFirstSearch = true;
    QTextCursor textcursor;
    int totalCount = 0;
    QWidget *lineNumberArea;
    QTextCursor currentCursor;

    QList<QString> Find(QString searchString,QTextDocument::FindFlags options, bool ifMarkNeed);

    void FindNext(QString searchString,QTextDocument::FindFlags options, bool searchLoop, bool searchDirection);
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
