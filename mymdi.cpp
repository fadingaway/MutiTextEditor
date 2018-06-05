#include "mymdi.h"
#include "QMessageBox"
#include "QFileInfo"
#include "QFileDialog"
#include "QTextStream"
#include "QTextEdit"
#include "QPushButton"
#include "QCloseEvent"
bool MyMdi::NewFile()
{
    IsUntitled = true;
    static int sequence_no = 1;
    CurrFileName = tr("Untitled_%1.txt").arg(sequence_no++);
    CurrFilePath = QFileInfo(CurrFileName).canonicalFilePath();
    setWindowTitle(CurrFileName);
    return true;
}

MyMdi::MyMdi()
{
    setAttribute(Qt::WA_DeleteOnClose);
}

bool MyMdi::OpenFile()
{
    CurrFileName = QFileDialog::getOpenFileName(this);
    if(!CurrFileName.isEmpty())
    {
        return LoadFile(CurrFileName);
    }
    else
    {
        return false;
    }
}

bool MyMdi::LoadFile(QString filename)
{
    QFile in(filename);
    if(!in.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"),tr("Open File Error"));
        return false;
    }
    QTextStream instream(&in);
    setText(instream.readAll());
    CurrFileName = filename;
    CurrFilePath = QFileInfo(filename).filePath();
    connect(this, &QTextEdit::textChanged, this, &MyMdi::fileModified);
    return true;
}

bool MyMdi::Save()
{
    if(IsUntitled)
    {
        return SaveAs();

    }
    else
    {
        return SaveFile(CurrFileName);
    }
}

bool MyMdi::SaveAs()
{
    CurrFileName = QFileDialog::getSaveFileName(this, tr("Save As"), CurrFilePath, tr("Text Document(*.txt)"));
    if(!CurrFileName.isEmpty())
    {
        return SaveFile(CurrFileName);
    }
    return true;
}

bool MyMdi::SaveFile(QString fileName)
{
    QFile out(fileName);
    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
        return false;
    }
    QTextStream outStream(&out);
    outStream<< document()->toPlainText();
    setWindowTitle(fileName);
    IsUntitled = false;
    return true;
}

void MyMdi::fileModified()
{
    setWindowModified(true);
    IsFileSaved = false;
    setWindowTitle("*"+CurrFileName);
}

void MyMdi::closeEvent(QCloseEvent *event)
{
    if(IsFileSaved)
    {
        event->accept();
    }
    else
    {
        QMessageBox::StandardButton  ret = QMessageBox::warning(this,
                             tr("Save File"),
                             tr("File has been changed, Do you want to save the file?"),
                             QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
        if(ret = QMessageBox::Save)
        {
            SaveAs();
            event->ignore();
        }
        else
        {
            event->accept();
        }
    }
}

QString MyMdi::GetCurrFileName()
{
    return CurrentFileName;
}
