#ifndef TABDIALOG_H
#define TABDIALOG_H

#include "QTabWidget"
#include "QDialog"
#include "QThread"
class TabDialog:public QDialog
{
    Q_OBJECT
public:
    explicit TabDialog(QString searchString, int tabIndex, QWidget *parent);

signals:
    void tabChanged(int tabIndex);
private slots:
    void updateWindowTitle(int tab_Id);
    void setTabAlpha(int value);
private:
    QTabWidget *tabWidget;
    QThread thread;
    double currentAlpa;
protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);
};

class FindTab:public QWidget
{
    Q_OBJECT
public:
    explicit FindTab(QString searchString = 0);
signals:
    void notifyTabWidget(int value);
public slots:
    void valueChange(int value);
};

class ReplaceTab:public QWidget
{
    Q_OBJECT
public:
    explicit ReplaceTab(QString searchString = 0);
};

class DocumentSearchTab:public QWidget
{
    Q_OBJECT
public:
    explicit DocumentSearchTab(QString searchString = 0);
};

#endif // TABDIALOG_H
