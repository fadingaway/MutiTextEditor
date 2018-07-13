#ifndef MYPLAINTEXTEDIT_H
#define MYPLAINTEXTEDIT_H
#include "QPlainTextEdit"
#include "QTextBlock"

class QTextBlock;
class MyPlainTextEdit: public QPlainTextEdit
{
    Q_OBJECT
public:
    MyPlainTextEdit(QWidget *parent = 0);
    void MySetViewportMargins(int left, int top, int right, int bottom);
    void MySetViewportMargins(const QMargins &margins);
    QString getFirstBlockInfo();
    QString getNextBlockInfo();
    QTextBlock getFirstVisiableBlock();
    QPointF getContentOffSet();
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    QRectF getblockBoundingGeometry(QTextBlock &block);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void paintLine(QList<QString> List);
signals:
    void blockDoubleClick(QString);
private:
    QTextBlock currentBlock;
    QList<QString> lineList;
};

#endif // MYPLAINTEXTEDIT_H
