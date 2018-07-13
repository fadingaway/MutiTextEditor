#ifndef MYDOCKWIDGET_H
#define MYDOCKWIDGET_H
#include "QDockWidget"
#include "QPlainTextEdit"
#include "myplaintextedit.h"

struct viewHolder
{
    int top = 0;
    int bottom = 0;
    bool plusInd = true;
};

class MyPlainTextEdit;
class MyDockWidget:public QDockWidget
{
public:
    explicit MyDockWidget(const QString &title, QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
    void LeftLinePaintEvent(QPaintEvent *event);
    void onPlusMinusMouseClicked(QMouseEvent *event);
    void updateTextArea(QList<QString> result);
    void updateTextArea();
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void setResult(QList<QString> result);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    MyPlainTextEdit *textEdit;
    QList<QString> searchResult;
    QList<QString> plusCaseSearchResult;
    QList<QString> currentSearchResult;
    QList<QString> lineCordinateList;
    QList<viewHolder> viewholders;
    QString doubleClickBlockText;
public slots:
    void updateLeftColumnArea(int value);
    void getDoubleClickBlockText(QString);
};

#endif // MYDOCKWIDGET_H
