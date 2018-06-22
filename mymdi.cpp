#include "mymdi.h"
#include "QMessageBox"
#include "QFileInfo"
#include "QFileDialog"
#include "QTextStream"
#include "QTextEdit"
#include "QPushButton"
#include "QCloseEvent"
#include "QPrintDialog"
#include "QPrinter"
MyMdi::MyMdi()
{
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->textEdit, &QTextEdit::selectionChanged, this, &MainWindow::setTextColor);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::unSetTextColor);
    document = document();
}

bool MyMdi::NewFile()
{
    IsUntitled = true;
    static int sequence_no = 1;
    CurrFileName = tr("Untitled_%1.txt").arg(sequence_no++);
    CurrFilePath = QFileInfo(CurrFileName).canonicalFilePath();
    setWindowTitle(CurrFileName);
    return true;
}

bool MyMdi::OpenFile( QString fileName = null)
{
    if(fileName.isEmpty())
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
    else
    {
        LoadFile(fileName);
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

bool MyMdi::CopySaveAs()
{
    QString *fileName = QFileDialog::getSaveFileName(this, tr("Save As"), CurrFilePath, tr("Text Document(*.txt)"));
    QFile out(fileName);
    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
        return false;
    }
    QTextStream outStream(&out);
    outStream<< document()->toPlainText();
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

void MyMdi::SetCurrFileName(QString fileName)
{
    CurrFileName = fileName;
}

void MyMdi::Print()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if(printDialog == QPrintDialog::Accepted)
    {
        print(&printer);
    }
}

bool MyMdi::GetSaveStatus()
{
    return IsFileSaved;
}
bool MyMdi::GetIsUntitled()
{
    return IsUntitled;
}
bool MyMdi::RenameFile()
{
    QString newFileName = QFileDialog::getSaveFileName(this, tr("Save As"), CurrFilePath, tr("Text Document(*.txt)"));
    QFile del(&CurrFileName);
    return del.rename(CurrFileName, newFileName);
}

void MyMdi::setTextColor()
{
    if(textCursor().hasSelection())
    {
        searchString = textCursor().selectedText();
        if(searchString != prevSearchString)
        {
            Find(searchString);
        }
    }
}

void MyMdi::unSetTextColor()
{
    if(!textCursor().hasSelection())
    {
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection extraSelection;
        QTextCursor textCursor(textCursor());
        extraSelection.cursor = textCursor;
        extraSelection.cursor.clearSelection();
        extraSelection.format = textCursor().charFormat();
        extraSelection.format.setBackground(QColor(200,255,255,255));
        extraSelection.format.setForeground(Qt::black);
        extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
        extraSelections.append(extraSelection);
        setExtraSelections(extraSelections);
    }
}

void MyMdi::Find(QString searchString)
{
    QTextCursor cursor(document);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);

    QTextEdit::ExtraSelection extraSelection_block;
    textCursor = textCursor();
    textCursor.movePosition(QTextCursor::StartOfLine);
    textCursor = document->find(searchString,textCursor, QTextDocument::FindWholeWords);
    extraSelection_block.cursor = textCursor;
    extraSelection_block.cursor.clearSelection();
    extraSelection_block.format.setBackground(QColor(200,255,255,255));
    extraSelection_block.format.setForeground(Qt::black);
    extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = document->find(searchString, cursor, QTextDocument::FindWholeWords);

        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);

}

void MyMdi::FindNext(QString searchString)
{
    QTextCursor cursor(document);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);
    isFirstSearch = true;

    QTextEdit::ExtraSelection extraSelection_block;
    textCursor = textCursor();
    if(isFirstSearch)
    {
        textCursor = document->find(searchString, textCursor, QTextDocument::FindWholeWords);
        if(!textCursor.isNull())
        {
            extraSelection_block.cursor = textCursor;
            extraSelection_block.cursor.clearSelection();
            extraSelection_block.format.setBackground(QColor(200,255,255,255));
            extraSelection_block.format.setForeground(Qt::black);
            extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);
            isFirstSearch = false;
            textCursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor);
            textCursor.clearSelection();
            setTextCursor(textCursor);
        }
    }

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = document->find(searchString, cursor, QTextDocument::FindWholeWords);

        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);
}
