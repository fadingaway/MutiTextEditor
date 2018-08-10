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
    MyDockWidget(QString title, QWidget *parent = 0);
    void LeftLinePaintEvent(QPaintEvent *event);
    void onPlusMinusMouseClicked(QMouseEvent *event);
    void updateTextArea(QList<QString> result);
    void updateTextArea();
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void setResult(QList<QString> result);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void updateLeftColumnArea(int value);
    void getDoubleClickBlockText(QString);
    void highlightCurrentLine();
    void paintLine();
    void markSearchString();

    MyPlainTextEdit *textEdit;
private:
    QString searchString = 0;
    QList<QString> searchResult;
    QList<QString> plusCaseSearchResult;
    QList<QString> currentSearchResult;
    QList<QString> lineCordinateList;
    QList<viewHolder> viewholders;
    QString doubleClickBlockText;
};


#endif // MYDOCKWIDGET_H
