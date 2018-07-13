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
MyMdi::MyMdi(QWidget *parent):QPlainTextEdit(parent)
{
    LineNumberArea = new LineNumberArea(this);

    setAttribute(Qt::WA_DeleteOnClose);
    connect(this, &QPlainTextEdit::selectionChanged, this, &MainWindow::setTextColor);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::unSetTextColor);
    document = document();

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberWidth(0);
    highlightCurrentLine();
}
int MyMdi::lineNumberWidth()
{
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
    setViewportMargins(lineNumberWidth(), 0 , 0,  0);
}
void MyMdi::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
    {
        LineNumberArea->scroll(0,dy);
    }
    else
    {
        LineNumberArea->update(0, rect.y(), LineNumberArea.width(), rect.x());
    }

    if(rect.contains(viewport()->rect()))
    {
        updateLineNumberWidth(0);
    }
}
void MyMdi::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    LineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberWidth(),cr.height()));
}
void MyMdi::lineNumberPaintEvent(QPaintEvent *event)
{
    QPainter painter(LineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = (int)blockBoundingRect(block).height();

    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom>= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0,top, LineNumberArea->width(),
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
    connect(this, &QPlainTextEdit::textChanged, this, &MyMdi::fileModified);
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
        QList<QPlainTextEdit::ExtraSelection> extraSelections;
        QPlainTextEdit::ExtraSelection extraSelection;
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
void MyMdi::Find(QString searchString, FindFlags options = FindFlags())
{
    QTextCursor cursor(document);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);

    QPlainTextEdit::ExtraSelection extraSelection_block;
    textCursor = textCursor();
    textCursor.movePosition(QTextCursor::StartOfLine);
    textCursor = document->find(searchString,textCursor, options);
    extraSelection_block.cursor = textCursor;
    extraSelection_block.cursor.clearSelection();
    extraSelection_block.format.setBackground(QColor(200,255,255,255));
    extraSelection_block.format.setForeground(Qt::black);
    extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);

    QList<QPlainTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QPlainTextEdit::ExtraSelection extraSelection;
        cursor = document->find(searchString, cursor, options);

        if (!cursor.isNull())
        {
            extraSelection.cursor = cursor;
            extraSelection.format = colorFormat;
            extraSelections.append(extraSelection);
        }
    }
    setExtraSelections(extraSelections);
}
void MyMdi::FindNext(QString searchString,FindFlags options = FindFlags())
{
    QTextCursor cursor(document);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::green);
    isFirstSearch = true;

    QPlainTextEdit::ExtraSelection extraSelection_block;
    textCursor = textCursor();
    if(isFirstSearch)
    {
        textCursor = document->find(searchString, textCursor, options);
        if(!textCursor.isNull())
        {
            extraSelection_block.cursor = textCursor;
            extraSelection_block.cursor.clearSelection();
            extraSelection_block.format.setBackground(QColor(200,255,255,255));
            extraSelection_block.format.setForeground(Qt::black);
            extraSelection_block.format.setProperty(QTextFormat::FullWidthSelection,true);
            isFirstSearch = false;
            //in case of there are two search result in same line.
            textCursor.movePosition(QTextCursor::WordRight, QTextCursor::MoveAnchor);
            textCursor.clearSelection();
            setTextCursor(textCursor);
        }
    }

    QList<QPlainTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(extraSelection_block);
    while (!cursor.isNull() && !cursor.atEnd())
    {
        QPlainTextEdit::ExtraSelection extraSelection;
        cursor = document->find(searchString, cursor, options);

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
    return totalCount;
}
void MyMdi::MarkLines(QList<QPoint> lineHolder)
{
    QPainter painter(this);
    painter.setBrush(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing);
    QList<QPoint>::iterator it;
    for(it = lineHolder.begin(); it != lineHolder.end(); it ++)
    {
        painter.drawEllipse(it, fontMetrics().height()/3, fontMetrics().height()/3);
    }
}
void MyMdi::highlightSearchString(QString searchString)
{
    QList<QPlainTextEdit::ExtraSelection> extraSelections = this->extraSelections();


    QTextCursor cursor(document);
    QTextCharFormat colorFormat(cursor.charFormat());
    colorFormat.setBackground(Qt::red);

    while (!cursor.isNull() && !cursor.atEnd())
    {
        QPlainTextEdit::ExtraSelection extraSelection;
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
void MyMdi::clearMark()
{
    setTextColor();
    unSetTextColor();
}
void MyMdi::createSearchDockWidget(QString searchString)
{

}
