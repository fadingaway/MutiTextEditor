#include "mymdi.h"
#include "QMessageBox"
#include "QFileInfo"
#include "QFileDialog"
#include "QTextStream"
#include "QPlainTextEdit"
#include "QPushButton"
#include "QCloseEvent"
#include "QPrintDialog"
#include "QPainter"
#include "QPrinter"
#include "QTextBlock"
#include "QDebug"
MyMdi::MyMdi(QWidget *parent):QPlainTextEdit(parent)
{
    qDebug()<<"MyMdi()";
    lineNumberArea = new LineNumberArea(this);

    setAttribute(Qt::WA_DeleteOnClose);
    textDocument = this->document();

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    setAccessibleName(CurrFileName);
    updateLineNumberWidth(0);
}
void MyMdi::highlightCurrentLine()
{
    qDebug()<<"highlightCurrentLine()";
    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections = this->extraSelections();

    QTextEdit::ExtraSelection extraSelection;
    QTextCursor textcursor(textCursor());
    extraSelection.cursor = textcursor;
    extraSelection.format = textCursor().charFormat();
    extraSelection.format.setBackground(QColor(200,255,255,255));
    extraSelection.format.setForeground(Qt::black);
    extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
    extraSelections.append(extraSelection);
    setExtraSelections(extraSelections);
}

int MyMdi::lineNumberWidth()
{
    qDebug()<<"lineNumberWidth()";
    int digits = 1;
    int max = qMax(1, blockCount());
    while(max >= 10)
    {
        max /= 10;
        ++ digits;
    }
    int space =  6 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;

}
void MyMdi::updateLineNumberWidth(int newBlockCount)
{
    qDebug()<<"updateLineNumberWidth()";
    newBlockCount = 0;
    setViewportMargins(lineNumberWidth(), 0 , 0,  0);
    this->repaint();
}
void MyMdi::updateLineNumberArea(const QRect &rect, int dy)
{
    qDebug()<<"updateLineNumberArea()";
    if(dy)
    {
        lineNumberArea->scroll(0,dy);
    }
    else
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.x());
    }

    if(rect.contains(viewport()->rect()))
    {
        updateLineNumberWidth(0);
    }
}
void MyMdi::resizeEvent(QResizeEvent *e)
{
    qDebug()<<"resizeEvent()";
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberWidth(),cr.height()));
}
void MyMdi::lineNumberPaintEvent(QPaintEvent *event)
{
    qDebug()<<"lineNumberPaintEvent()";
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = 0;

    if(blockNumber == 0)
    {
        bottom = (int)blockBoundingRect(block).height() + contentOffset().y();
    }
    else
    {
        bottom = (int)blockBoundingRect(block).height();
    }
    while(block.isValid() && top <= event->rect().bottom())
    {
        qDebug()<<"top = "<<top;
        qDebug()<<"bottom = "<<bottom;
        if(block.isVisible() && bottom>= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            qDebug()<<"number = "<<number;
            painter.setPen(Qt::black);
            painter.drawText(0,top, lineNumberArea->width(),
                             fontMetrics().height(),
                             Qt::AlignCenter,
                             number);
        }
        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}
bool MyMdi::NewFile()
{
    qDebug()<<"NewFile()";
    IsUntitled = true;
    static int sequence_no = 1;
    CurrFileName = tr("Untitled_%1.txt").arg(sequence_no++);
    CurrFilePath = QFileInfo(CurrFileName).canonicalFilePath();
    setWindowTitle(CurrFileName);
    setAccessibleName(CurrFileName);
    return true;
}
bool MyMdi::OpenFile( QString fileName)
{
    qDebug()<<"OpenFile()";
    if(fileName.isEmpty()&&fileName != QString("0"))
    {
        CurrFileName = QFileDialog::getOpenFileName(this);
        qDebug()<<"CurrFileName = "<<CurrFileName;
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
        return LoadFile(fileName);
    }
}
bool MyMdi::LoadFile(QString filename)
{
    qDebug()<<"LoadFile()";
    QFile in(filename);
    if(!in.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"),tr("Open File Error"));
        return false;
    }
    QTextStream instream(&in);
    this->appendPlainText(instream.readAll());
    CurrFileName = QFileInfo(filename).fileName();
    CurrFilePath = QFileInfo(filename).filePath();
    qDebug()<<"CurrFileName = "<<CurrFileName;
    connect(this, &QPlainTextEdit::textChanged, this, &MyMdi::fileModified);
    setAccessibleName(CurrFileName);
    return true;
}
bool MyMdi::Save()
{
    qDebug()<<"Save()";
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
    qDebug()<<"SaveAs()";
    CurrFileName = QFileDialog::getSaveFileName(this, tr("Save As"), CurrFilePath, tr("Text Document(*.txt)"));
    qDebug()<<"CurrFileName = "<<CurrFileName;
    if(!CurrFileName.isEmpty())
    {
        return SaveFile(CurrFileName);
    }
    else
    {
        return false;
    }
}
bool MyMdi::CopySaveAs()
{
    qDebug()<<"CopySaveAs()";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    CurrFilePath,
                                                    tr("Text Document(*.txt)"));
    qDebug()<<"fileName = "<<fileName;
    QFile out(fileName);
    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
        return false;
    }
    QTextStream outStream(&out);
    outStream<<this->toPlainText();
    out.close();
    return true;
}
bool MyMdi::SaveFile(QString fileName)
{
    qDebug()<<"SaveFile()";
    QFile out(fileName);
    if(!out.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Save File Error"));
        return false;
    }
    QTextStream outStream(&out);
    outStream<<this->toPlainText();
    setWindowTitle(fileName);
    CurrFileName = QFileInfo(fileName).fileName();
    setAccessibleName(CurrFileName);
    IsUntitled = false;
    out.close();
    return true;
}
void MyMdi::fileModified()
{
    qDebug()<<"fileModified()";
    setWindowModified(true);
    IsFileSaved = false;
    setWindowTitle("*"+CurrFileName);
}
void MyMdi::closeEvent(QCloseEvent *event)
{
    qDebug()<<"closeEvent()";
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
        if(ret == QMessageBox::Save)
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
    qDebug()<<"GetCurrFileName()";
    return CurrFileName;
}
void MyMdi::SetCurrFileName(QString fileName)
{
    qDebug()<<"SetCurrFileName()";
    CurrFileName = fileName;
}
void MyMdi::Print()
{
    qDebug()<<"Print()";
    QPrinter printer;
    if(QPrintDialog(&printer, this).exec() == QPrintDialog::Accepted)
    {
        print(&printer);
    }
}
bool MyMdi::GetSaveStatus()
{
    qDebug()<<"GetSaveStatus()";
    return IsFileSaved;
}
bool MyMdi::GetIsUntitled()
{
    qDebug()<<"GetIsUntitled()";
    return IsUntitled;
}
bool MyMdi::RenameFile()
{
    qDebug()<<"RenameFile()";
    QString newFileName = QFileDialog::getSaveFileName(this, tr("Save As"), CurrFilePath, tr("Text Document(*.txt)"));
    QFile del(CurrFileName);
    newFileName = QFileInfo(newFileName).fileName();
    return del.rename(CurrFileName, newFileName);
}
void MyMdi::setTextColor()
{
    qDebug()<<"setTextColor()";
    if(textCursor().hasSelection())
    {
        searchString = textCursor().selectedText();
        if(searchString != prevSearchString)
        {
            Find(searchString, QTextDocument::FindWholeWords);
        }
    }
}
void MyMdi::unSetTextColor()
{
    qDebug()<<"unSetTextColor()";
    if(!textCursor().hasSelection())
    {
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection extraSelection;
        QTextCursor textcursor = this->textCursor();
        extraSelection.cursor = textcursor;
        extraSelection.cursor.clearSelection();
        extraSelection.format = textCursor().charFormat();
        extraSelection.format.setBackground(QColor(200,255,255,255));
        extraSelection.format.setForeground(Qt::black);
        extraSelection.format.setProperty(QTextFormat::FullWidthSelection,true);
        extraSelections.append(extraSelection);
        setExtraSelections(extraSelections);
    }
}
QList<int> MyMdi::Find(QString searchString, QTextDocument::FindFlag options)
{
    qDebug()<<"Find()";
    QList<int> lineNoHolder;
    QTextCursor cursor(textDocument);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);

    QTextEdit::ExtraSelection extraSelection_block;
    textcursor = textCursor();
    textcursor.movePosition(QTextCursor::StartOfLine);
    textcursor = textDocument->find(searchString, textcursor, options);
    lineNoHolder.append(textcursor.blockNumber());
    extraSelection_block.cursor = textcursor;
    extraSelection_block.cursor.clearSelection();
    extraSelection_block.format.setBackground(QColor(200,255,255,255));
    extraSelection_block.format.setForeground(Qt::black);
    extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = textDocument->find(searchString, cursor, options);
        lineNoHolder.append(cursor.blockNumber());
        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);
    return lineNoHolder;
}
void MyMdi::FindNext(QString searchString, QTextDocument::FindFlag options)
{
    qDebug()<<"FindNext()";
    QTextCursor cursor(textDocument);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);
    isFirstSearch = true;

    QTextEdit::ExtraSelection extraSelection_block;
    textcursor = textCursor();
    if(isFirstSearch)
    {
        textcursor = textDocument->find(searchString, textcursor, options);
        if(!textcursor.isNull())
        {
            extraSelection_block.cursor = textcursor;
            extraSelection_block.cursor.clearSelection();
            extraSelection_block.format.setBackground(QColor(200,255,255,255));
            extraSelection_block.format.setForeground(Qt::black);
            extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);
            isFirstSearch = false;
            //in case of there are two search result in same line.
            textcursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor);
            textcursor.clearSelection();
            setTextCursor(textcursor);
        }
    }

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = textDocument->find(searchString, cursor, options);

        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
            totalCount ++;
        }
    }
    setExtraSelections(extraSelections);
}
int MyMdi::GetTotalCount()
{
    qDebug()<<"GetTotalCount()";
    return totalCount;
}
void MyMdi::MarkLines(QList<QPoint> lineHolder)
{
    qDebug()<<"MarkLines()";
    QPainter painter(this);
    painter.setBrush(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing);
    for( int i = 0; i< lineHolder.size(); i++)
    {
        painter.drawEllipse(lineHolder.at(i).x(), lineHolder.at(i).y(), fontMetrics().height()/3, fontMetrics().height()/3);
    }
}
void MyMdi::highlightSearchString(QString searchString)
{
    qDebug()<<"highlightSearchString()";
    QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();


    QTextCursor cursor(textDocument);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::red);

    while (!cursor.isNull() && !cursor.atEnd())
    {
        QTextEdit::ExtraSelection extraSelection;
        cursor = textDocument->find(searchString, cursor, QTextDocument::FindWholeWords);

        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);
}
void MyMdi::clearMark()
{
    qDebug()<<"clearMark()";
    setTextColor();
    unSetTextColor();
}

QList<int> MyMdi::searchCurrentFile(QString searchString)
{
    qDebug()<<"searchCurrentFile()";
    return Find(searchString, QTextDocument::FindWholeWords);
}
