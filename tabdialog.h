#ifndef TABDIALOG_H
#define TABDIALOG_H

#include "QTabWidget"
#include "QDialog"

class FindTab:public QWidget
{
    Q_OBJECT
public:
    explicit FindTab(QString searchString = 0);
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

class TabDialog:public QDialog
{
    Q_OBJECT
public:
    explicit TabDialog(QString searchString = 0, int tabIndex, QWidget *parent = 0);

signals:
    void tabChanged(int tabIndex);
private slots:
    void updateWindowTitle(int tab_Id);
private:
    QTabWidget *tabWidget;
};


#endif // TABDIALOG_H
