#ifndef MYMDI_H
#define MYMDI_H

#include "QMdiArea"
#include "QTextEdit"
#include "QTextDocument"
#include "QTextCursor"

class QPrinter;
class MyMdi: public QTextEdit
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
public:
    MyMdi();
    QString GetCurrFileName();
    void SetCurrFileName(QString fileName);
    bool GetSaveStatus();
    bool GetIsUntitled();
    void Find(QString searchString);
    int GetTotalCount();
    void FindNext(QString searchString);
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
};


#endif // MYMDI_H
