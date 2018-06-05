#ifndef MYMDI_H
#define MYMDI_H

#include "QMdiArea"
#include "QTextEdit"

class MyMdi: public QTextEdit
{
    Q_OBJECT
private slots:
    void fileModified();

public:
    MyMdi();
    bool NewFile();
    bool OpenFile();
    bool Save();
    bool SaveAs();
    bool SaveFile(QString fileName);
    bool LoadFile(QString filename);
    void closeEvent(QCloseEvent *event);
    QString GetCurrFileName();
private:
    QString CurrFileName;
    QString CurrFilePath;
    bool IsUntitled;
    bool IsFileSaved;
};


#endif // MYMDI_H
